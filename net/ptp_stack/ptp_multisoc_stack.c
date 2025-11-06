/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <net/data_pkt.h>
#include <net/net_debug.h>
#include <net/net_switch.h>
#include <net/vport_software.h>
#include <net/net_types.h>
#include <os/stdlib.h>

#include "genavb/error.h"
#include "genavb/qos.h"
#include "genavb/socket.h"

#include "ptp_stack_os.h"
#include "ptp_stack_priv.h"
#include "lib_port_genavb.h"
#include "dsa_net_protocol.h"

#define MAX_FRAME_LEN (1518)

static char ptp_multicast_addr[6] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x0e};

static void insert_vlan_tag(struct data_pkt *pkt, uint8_t src_port)
{
	uint32_t vlan_tag = VLAN_TAG(src_port);
	uint8_t *vlan_addr = (uint8_t *)&vlan_tag;
	uint8_t *data = pkt->data;
	uint8_t insert_size = VLAN_TAG_LENGTH;
	int i;

	/* Move DST and SRC MAC address forward */
	for (i = 0; i < MAC_ADDR_TOTAL_LENGTH; i++) {
		*(data - insert_size) = *data;
		data++;
	}

	/* Insert vlan tag */
	for (i = 0; i < VLAN_TAG_LENGTH; i++)
		*(data++ - insert_size) = *(vlan_addr++);

	pkt->data_len += insert_size;
	pkt->data -= insert_size;
}

static void insert_dsa_tag_ts(struct data_pkt *pkt, uint8_t src_port, uint64_t ns, uint64_t cycles)
{
	uint32_t dsa_tag = DSA_TO_HOST_RX_TS_TAG(src_port);
	uint8_t *dsa_addr = (uint8_t *)&dsa_tag;
	uint64_t timestamp;
	uint8_t *ts_addr;
	uint8_t *data = pkt->data;
	uint8_t insert_size = DSA_TAG_LENGTH + TIMESTAMP_LENGTH + TIMESTAMP_LENGTH;
	int i;

	/* Move DST and SRC MAC address forward */
	for (i = 0; i < MAC_ADDR_TOTAL_LENGTH; i++) {
		*(data - insert_size) = *data;
		data++;
	}

	/* Insert dsa tag */
	for (i = 0; i < DSA_TAG_LENGTH; i++)
		*(data++ - insert_size) = *(dsa_addr++);

	/* Insert timestamp - ns */
	timestamp = htonll(ns);
	ts_addr = (uint8_t *)&timestamp;
	for (i = 0; i < TIMESTAMP_LENGTH; i++)
		*(data++ - insert_size) = *(ts_addr++);

	/* Insert timestamp - cycles */
	timestamp = htonll(cycles);
	ts_addr = (uint8_t *)&timestamp;
	for (i = 0; i < TIMESTAMP_LENGTH; i++)
		*(data++ - insert_size) = *(ts_addr++);

	pkt->data_len += insert_size;
	pkt->data -= insert_size;
}

static int remove_tag(struct data_pkt *pkt, uint8_t tag_len)
{
	uint8_t *data_src, *data_dst;
	int i;

	/* check whether it is valid frame */
	if (pkt->data_len <= (MAC_ADDR_TOTAL_LENGTH + tag_len))
		return -1;

	/* Move DST and SRC MAC address backward by "tag_len" bytes */
	data_src = pkt->data + MAC_ADDR_TOTAL_LENGTH - 1;
	data_dst = pkt->data + MAC_ADDR_TOTAL_LENGTH + tag_len - 1;
	for (i = 0; i < MAC_ADDR_TOTAL_LENGTH; i++)
		*data_dst-- = *data_src--;

	pkt->data_len -= tag_len;
	pkt->data += tag_len;

	return 0;
}

static inline int find_switch_port_idx(struct gen_avb_device *genavb_dev, uint16_t logical_id)
{
	int i;

	for (i = 0; i < genavb_dev->bridge_port_num; i++) {
		if (logical_id == genavb_dev->bridge_port[i].hardware_id)
			return i;
	}

	return -1;
}

/* ToDo, handle Meta frames */
void meta_rx(struct gen_avb_device *genavb_dev, struct data_pkt *pkt) {
	genavb_dev->priv_stats.meta_rx_pkts++;
}

/* Tx to stack from host */
static int genavb_host_to_sw_stack(struct soft_vport_dev *sw_vport_dev, void *data_paket)
{
	struct data_pkt *pkt = data_paket;
	struct gen_avb_device *genavb_dev = sw_vport_dev->priv;
	struct eth_hdr *hdr;
	unsigned int ts_priv = 0;
	uint16_t ethertype;
	uint16_t dsa_custom;
	uint8_t dst_port = 0xFF;
	uint8_t tag_len;
	bool to_sw_port = false;
	bool take_ts = false;
	int port_idx;
	int rc = 0;

	hdr = (struct eth_hdr *)pkt->data;
	ethertype = ntohs(hdr->ether_type);
	dsa_custom = ntohs(hdr->dsa_custom);

#ifdef DEBUG
	os_printf("Received frames from host port:\r\n");
	dbg_hexdump(pkt->data, pkt->data_len, FMT_2BYTE);
#endif

	/* Handle VLan frames */
	if ((ethertype == ETHERTYPE_VLAN) &&
		genavb_dev->flags & GENAVB_PORT_FLAGS_TX_REMOVE_VLAN) {
		dst_port = dsa_custom & VLAN_VID_PORT_MASK;
		rc = remove_tag(pkt, VLAN_TAG_LENGTH);
		if (rc)
			goto fmt_err;
		to_sw_port = true;
	}

	/* Handle DSA frames */
	if ((ethertype == DSA_8021Q_TPID) &&
		genavb_dev->flags & GENAVB_PORT_FLAGS_TX_REMOVE_DSA) {
		/* Meta frame */
		if (DSA_TO_SW_META(dsa_custom)) {
			meta_rx(genavb_dev, pkt);

			return 0;
		}

		/* PTP frames */
		dst_port = dsa_custom & DSA_TO_SW_PORT_MASK;
		take_ts =  DSA_TO_SW_TAKE_TS(dsa_custom);
		if (take_ts) {
			ts_priv = ntohl(hdr->u.dsa_to_sw_ts_frame.ts_id);
			tag_len = DS_TO_SW_TS_TAG_LEN;
		} else {
			tag_len = DS_TO_SW_NONTS_TAG_LEN;
		}
		rc = remove_tag(pkt, tag_len);
		if (rc)
			goto fmt_err;
		to_sw_port = true;
	}

	if (to_sw_port && dst_port != 0xFF) {
		port_idx = find_switch_port_idx(genavb_dev, dst_port);
		if (port_idx < 0)
			goto fmt_err;

#ifdef DEBUG
	os_printf("Transmit host frame to switch port #%d %s retrieving timestamp:\r\n",
				genavb_dev->bridge_port[port_idx].hardware_id,
				take_ts ? "with" : "without");
	if (take_ts)
		os_printf("timestamp priavte ID is %x\r\n", ts_priv);
	dbg_hexdump(pkt->data, pkt->data_len, FMT_2BYTE);
#endif

		if (take_ts && (genavb_dev->flags & GENAVB_PORT_FLAGS_PTP_SOCKET)) {
			struct genavb_socket_tx_send_params params = { .flags = GENAVB_SOCKET_TX_TS, .priv = ts_priv };
			rc = genavb_socket_tx_send(genavb_dev->bridge_port[port_idx].sock_tx_ptp, pkt->data, pkt->data_len, &params);
			if (rc != GENAVB_SUCCESS)
				goto err;
		} else {
			rc = genavb_socket_tx(genavb_dev->bridge_port[port_idx].sock_tx, pkt->data, pkt->data_len);
			if (rc != GENAVB_SUCCESS)
				goto err;
		}
	} else if (genavb_dev->ep_port_num) {
		rc = genavb_socket_tx(genavb_dev->endpoint_port[0].sock_tx, pkt->data, pkt->data_len);
		if (rc != GENAVB_SUCCESS)
			goto err;
	}

	genavb_dev->priv_stats.tx_pkts++;

	return 0;

err:
	genavb_dev->priv_stats.tx_err_pkts++;
	return -1;

fmt_err:
	genavb_dev->priv_stats.invalid_host_pkts++;
	return -1;
}

static void net_callback(void *data)
{
	struct socket_rx_dev *port_dev = (struct socket_rx_dev *)data;
	struct gen_avb_device *genavb_dev = port_dev->port->genavb_dev;
	struct genavb_port_event event;

	event.data = port_dev;
	event.type = GENAVB_RCV;
	os_mq_send(&genavb_dev->mqueue, &event, 0, 0);
}

void genavb_receive_pkts(void *param)
{
	struct gen_avb_device *genavb_dev = param;
	void *port_dev = genavb_dev->vport_dev;
	struct socket_rx_dev *avb_port_dev;
	struct genavb_port_event event;
	struct data_pkt *pkt;
	uint64_t ts, gptp_time;
	uint8_t src_port;
	int len;
	int status;
	int rc;

	do {
		if (!os_mq_receive(&genavb_dev->mqueue, &event, 0, OS_QUEUE_EVENT_TIMEOUT_MAX)) {
			if (event.type != GENAVB_RCV)
				continue;
			avb_port_dev = event.data;
			do {
				len = MAX_FRAME_LEN;
				status = NET_NO_FRAME;
				pkt = data_pkt_alloc(len, DATA_PKT_FLAG_CAN_FREE);
				if (!pkt) {
					os_printf("%s: failed to alloc data pkt\r\n", __func__);
					continue;
				}
				len = genavb_socket_rx(avb_port_dev->sock, pkt->data, len, &ts);
				if (len > 0) {
					data_pkt_set_len(pkt, len);
					/* ToDo, only can insert vlan or dsa tag for bridge port */
					if (avb_port_dev->port->is_bridge_port) {
						src_port = avb_port_dev->port->bridge_port_index;
#ifdef DEBUG
						os_printf("Rcved frame from switch port %d, ts=%016llx\r\n", src_port, ts);
						dbg_hexdump(pkt->data, pkt->data_len, FMT_2BYTE);
#endif
						if ((genavb_dev->flags & GENAVB_PORT_FLAGS_RX_INSERT_VLAN)
								&& !avb_port_dev->is_ptp_sock)
							insert_vlan_tag(pkt, src_port);
						if ((genavb_dev->flags & GENAVB_PORT_FLAGS_RX_INSERT_DSA)
								&& avb_port_dev->is_ptp_sock) {
							genavb_clock_convert(GENAVB_CLOCK_LOCAL_BR_0, ts,
								genavb_dev->gptp_clock_id, &gptp_time);
							insert_dsa_tag_ts(pkt, src_port, gptp_time, ts);
						}
#ifdef DEBUG
						os_printf("Rcved frame after inserting vlan or dsa header\r\n");
						dbg_hexdump(pkt->data, pkt->data_len, FMT_2BYTE);
#endif
					}
					soft_vport_in_nocpy(port_dev, pkt);
					genavb_dev->priv_stats.rx_pkts++;
					soft_vport_in_kick(port_dev);
					status = NET_OK;
				} else if (len == -GENAVB_ERR_SOCKET_AGAIN) {
					data_pkt_free(pkt);
					status = NET_NO_FRAME;
				} else {
					data_pkt_free(pkt);
					genavb_dev->priv_stats.rx_err_pkts++;
					status = NET_ERR;
				}
			} while (status == NET_OK);
			rc = genavb_socket_rx_enable_callback(avb_port_dev->sock);
			if (rc != GENAVB_SUCCESS) {
				os_printf("genavb_socket_rx_set_callback failed, rx=%d\r\n", rc);
			}
		}
	} while (1);
}

/* Tx to host */
static int genavb_socket_output(struct soft_vport_dev *sw_vport_dev, void *data_paket)
{
	struct data_pkt *pkt = data_paket;
	struct gen_avb_device *genavb_dev = sw_vport_dev->priv;
	struct genavb_socket_tx *sock_tx;
	int rc = 0;

	if (genavb_dev->bridge_port_num == 1)
		sock_tx = genavb_dev->bridge_port[0].sock_tx;
	else if (genavb_dev->ep_port_num == 1)
		sock_tx = genavb_dev->endpoint_port[0].sock_tx;
	else
		return -1;
	rc = genavb_socket_tx(sock_tx, pkt->data, pkt->data_len);
	if (rc != GENAVB_SUCCESS) {
		genavb_dev->priv_stats.tx_err_pkts++;
		return -1;
	}

	genavb_dev->priv_stats.tx_pkts++;

	return 0;
}

static void ptp_ts_callback(void *data)
{
	struct gen_avb_port_dev *port_dev = (struct gen_avb_port_dev *)data;
	struct gen_avb_device *genavb_dev = port_dev->genavb_dev;
	struct genavb_port_event event;

	event.data = port_dev;
	event.type = GENAVB_RCV;
	os_mq_send(&genavb_dev->ptp_ts_mqueue, &event, 0, 0);
}

static void handle_tx_ts(struct gen_avb_device *genavb_dev, uint64_t ts, unsigned int ts_priv,
							uint16_t hardware_id, uint16_t logicial_id)
{
	void *port_dev = genavb_dev->vport_dev;
	struct data_pkt *pkt;
	struct eth_hdr *hdr;
	uint32_t frame_len;
	uint64_t gptp_time;

	frame_len = MAC_ADDR_TOTAL_LENGTH + DSA_TO_HOST_TX_TS_TAG_LEN;
	pkt = data_pkt_alloc(frame_len, DATA_PKT_FLAG_CAN_FREE);
	if (!pkt) {
		os_printf("%s: failed to alloc data pkt\r\n", __func__);
		return;
	}

	hdr = (struct eth_hdr *)pkt->data;
	pkt->data_len = frame_len;
	memcpy(hdr->dst, ptp_multicast_addr, MAC_ADDR_LEN);
	genavb_socket_get_hwaddr(logicial_id, hdr->src);

	hdr->ether_type = htons(DSA_8021Q_TPID);
	hdr->dsa_custom = htons(DSA_TO_HOST_TX_TS_DEFAULT_DSA |
						((hardware_id & DSA_TO_HOST_PORT_MASK) << DSA_TO_HOST_PORT_OFFSET));
	genavb_clock_convert(GENAVB_CLOCK_LOCAL_BR_0, ts, genavb_dev->gptp_clock_id, &gptp_time);

	hdr->u.dsa_to_host_tx_ts_frame.ts = htonll(gptp_time);
	hdr->u.dsa_to_host_tx_ts_frame.cycles = htonll(ts);
	hdr->u.dsa_to_host_tx_ts_frame.ts_id = htonl(ts_priv);

#ifdef DEBUG
	os_printf("Transmit TX timestamp frame back to host ts_id: 0x%08x, ts: 0x%016llx gptp: 0x%016llx\r\n",
			  ts_priv, ts, gptp_time);

	dbg_hexdump(pkt->data, pkt->data_len, FMT_2BYTE);
#endif

	soft_vport_in_nocpy(port_dev, pkt);
	soft_vport_in_kick(port_dev);
	genavb_dev->priv_stats.tx_ts_pkts++;
}

void genavb_retrieve_ptp_ts(void *param)
{
	struct gen_avb_device *genavb_dev = param;
	struct gen_avb_port_dev *avb_port_dev;
	struct genavb_port_event event;
	uint64_t ts;
	unsigned int ts_priv;
	int rc;

	do {
		if (!os_mq_receive(&genavb_dev->ptp_ts_mqueue, &event, 0, OS_QUEUE_EVENT_TIMEOUT_MAX)) {
			if (event.type != GENAVB_RCV)
				continue;
			avb_port_dev = event.data;
			do {
				rc = genavb_socket_tx_get_ts(avb_port_dev->sock_tx_ptp, &ts, &ts_priv);
				if (rc)
					break;
				handle_tx_ts(genavb_dev, ts, ts_priv, avb_port_dev->hardware_id, avb_port_dev->logical_port_id);
			} while (1);
			rc = genavb_socket_tx_enable_callback(avb_port_dev->sock_tx_ptp);
			if (rc != GENAVB_SUCCESS) {
				os_printf("genavb_socket_rx_set_callback failed, rx=%d\r\n", rc);
			}
		}
	} while (1);
}

static void print_priv_stats(struct soft_vport_dev *sw_vport_dev)
{
	struct gen_avb_device *genavb_dev = sw_vport_dev->priv;

	os_printf("[GENAVB_Dev_%s]:\r\t\t\tRX packets\t%llu\tRX Err packets\t%llu\tTX packets\t%llu\tTX Err packets\t%llu\tRX Meta packets\t%llu\r\n",
			  genavb_dev->is_hostport ? "host" : "switch",
			  genavb_dev->priv_stats.rx_pkts,
			  genavb_dev->priv_stats.rx_err_pkts,
			  genavb_dev->priv_stats.tx_pkts,
			  genavb_dev->priv_stats.tx_err_pkts,
			  genavb_dev->priv_stats.meta_rx_pkts);
}

void port_genavb_link_up(void *dev, bool up)
{
	struct gen_avb_device *genavb_dev = dev;

	genavb_dev->link_up = up;
}

void port_genavb_setup_host_mac(void *dev, void *host_mac)
{
	struct gen_avb_device *genavb_dev = dev;

	memcpy(genavb_dev->host_mac, host_mac, MAC_ADDR_LEN);
}


void port_genavb_update_addr(void *dev, void *mac_addr)
{
	struct gen_avb_device *genavb_dev = dev;

	soft_vport_update_addr(genavb_dev->vport_dev, mac_addr);
}

int port_genavb_add_fdb_entry(void *dev, void *mac_addr)
{
	struct gen_avb_device *genavb_dev = dev;

	return soft_vport_add_fdb_entry(genavb_dev->vport_dev, mac_addr);
}

int port_genavb_remove_fdb_entry(void *dev, void *mac_addr)
{
	struct gen_avb_device *genavb_dev = dev;

	return soft_vport_remove_fdb_entry(genavb_dev->vport_dev, mac_addr);
}

static int port_genavb_setup_sockets(struct gen_avb_port_dev *port, bool enable_ptp)
{
	struct genavb_socket_tx_params tx_params = {
		.addr = {
			.vlan_id = 0xffff, // no vlan tag
			.priority = 0,	   // Best Effort
			.port = port->logical_port_id,
		},
	};
	struct genavb_socket_rx_params rx_params = {
		.addr = {
			.port = port->logical_port_id,
		},
	};
	int rc;

	tx_params.addr.ptype = PTYPE_L2;
	rc = genavb_socket_tx_open(&port->sock_tx, GENAVB_SOCKF_RAW, &tx_params);
	if (rc != GENAVB_SUCCESS) {
		os_printf("genavb_socket_tx_open failed, rx=%d\r\n", rc);
		goto err;
	}
	port->sock_tx_opened = true;

	rx_params.addr.ptype = PTYPE_OTHER;
	rc = genavb_socket_rx_open(&port->sock_rx.sock, GENAVB_SOCKF_RAW | GENAVB_SOCKF_NONBLOCK , &rx_params);
	if (rc != GENAVB_SUCCESS) {
		os_printf("genavb_socket_rx_open failed, rx=%d\r\n", rc);
		goto err;
	}
	rc = genavb_socket_rx_set_callback(port->sock_rx.sock, net_callback, &port->sock_rx);
	if (rc != GENAVB_SUCCESS) {
		os_printf("genavb_socket_rx_set_callback failed, rx=%d\r\n", rc);
		goto err;
	}
	port->sock_rx.port = port;
	port->sock_rx.is_ptp_sock = false;
	port->sock_rx_opened = true;

	if (!enable_ptp)
		return 0;

	tx_params.addr.ptype = PTYPE_PTP;
	rc = genavb_socket_tx_open(&port->sock_tx_ptp, GENAVB_SOCKF_RAW, &tx_params);
	if (rc != GENAVB_SUCCESS) {
		os_printf("genavb_socket_tx_open failed, rx=%d\r\n", rc);
		goto err;
	}
	rc = genavb_socket_tx_set_callback(port->sock_tx_ptp, ptp_ts_callback, port);
	if (rc != GENAVB_SUCCESS) {
		os_printf("genavb_socket_tx_set_callback failed, rx=%d\r\n", rc);
		goto err;
	}
	port->sock_tx_ptp_opened = true;

	rx_params.addr.ptype = PTYPE_PTP;
	rx_params.addr.u.ptp.version = 2;
	rx_params.addr.priority = PTPV2_DEFAULT_PRIORITY;
	rc = genavb_socket_rx_open(&port->sock_rx_ptp.sock, GENAVB_SOCKF_RAW | GENAVB_SOCKF_NONBLOCK , &rx_params);
	if (rc != GENAVB_SUCCESS) {
		os_printf("genavb_socket_rx_open failed, rx=%d\r\n", rc);
		goto err;
	}
	rc = genavb_socket_rx_set_callback(port->sock_rx_ptp.sock, net_callback, &port->sock_rx_ptp);
	if (rc != GENAVB_SUCCESS) {
		os_printf("genavb_socket_rx_set_callback failed, rx=%d\r\n", rc);
		goto err;
	}
	port->sock_rx_ptp.port = port;
	port->sock_rx_ptp.is_ptp_sock = true;
	port->sock_rx_ptp_opened = true;

	return 0;

err:
	/* Close all opend socket */
	if (port->sock_tx_opened) {
		genavb_socket_tx_close(port->sock_tx);
		port->sock_tx_opened = false;
	}
	if (port->sock_rx_opened) {
		genavb_socket_rx_close(port->sock_rx.sock);
		port->sock_rx_opened = false;
	}
	if (port->sock_tx_ptp_opened) {
		genavb_socket_tx_close(port->sock_tx_ptp);
		port->sock_tx_ptp_opened = false;
	}
	if (port->sock_rx_ptp_opened) {
		genavb_socket_rx_close(port->sock_rx_ptp.sock);
		port->sock_rx_ptp_opened = false;
	}

	return -1;
}

void *port_genavb_init(void *switch_dev, struct genavb_port_cfg *cfg)
{
	struct gen_avb_device *genavb_dev;
	struct soft_vport_dev *vport_dev;
	struct soft_vport_cfg vport_cfg;
	bool enable_ptp;
	int rc;
	int i;

	genavb_dev = os_malloc(sizeof(struct gen_avb_device));
	os_assert(genavb_dev, "no memory for gen_avb socket device");
	memset(genavb_dev, 0, sizeof(struct gen_avb_device));

	rc = os_mq_open(&genavb_dev->mqueue, "port_mqueue", 100, sizeof(struct genavb_port_event));
	if (rc) {
		os_printf("port mqueue initialization failed, rx=%d\r\n", rc);
		goto err;
	}

	genavb_dev->is_hostport = cfg->is_hostport;
	if (cfg->is_hostport) {
		if (!((cfg->ep_port_num == 1 && cfg->bridge_port_num == 0) ||
			(cfg->ep_port_num == 0 && cfg->bridge_port_num == 1)))
			os_printf("host port must provides single ep port or switch port\r\n");
	}

	genavb_dev->ep_port_num = cfg->ep_port_num;
	for (i = 0; i < cfg->ep_port_num; i++) {
		genavb_dev->endpoint_port[i].logical_port_id = cfg->ep_port_id[i];
		genavb_dev->endpoint_port[i].is_bridge_port = false;
		genavb_dev->endpoint_port[i].genavb_dev = genavb_dev;
	}
	genavb_dev->flags = cfg->flags;
	genavb_dev->link_up = false;
	enable_ptp = cfg->flags & GENAVB_PORT_FLAGS_PTP_SOCKET;
	if (enable_ptp) {
		rc = os_mq_open(&genavb_dev->ptp_ts_mqueue, "port ptp_mqueue", 100, sizeof(struct genavb_port_event));
		if (rc) {
			os_printf("port ptp mqueue initialization failed, rx=%d\r\n", rc);
			goto err;
		}
	}

	genavb_dev->gptp_clock_id = cfg->gptp_clock_id;

	if (!(cfg->flags & GENAVB_PORT_FLAGS_LOCAL_PORT))
		memcpy(genavb_dev->host_mac, cfg->host_mac, MAC_ADDR_LEN);

	for (i = 0; i < cfg->ep_port_num; i++) {
		rc = port_genavb_setup_sockets(&genavb_dev->endpoint_port[i], enable_ptp);
		if (rc)
			goto err;
	}

	os_assert(cfg->bridge_port_num <= CFG_BRIDGE_NUM, "invalid genavb bridge port number");
	genavb_dev->bridge_port_num = cfg->bridge_port_num;
	for (i = 0; i < cfg->bridge_port_num; i++) {
		genavb_dev->bridge_port[i].bridge_port_index = i;
		genavb_dev->bridge_port[i].is_bridge_port = true;
		genavb_dev->bridge_port[i].logical_port_id = cfg->bridge_port_id[i].logical_id;
		genavb_dev->bridge_port[i].hardware_id = cfg->bridge_port_id[i].hardware_id;
		genavb_dev->bridge_port[i].genavb_dev = genavb_dev;

		rc = port_genavb_setup_sockets(&genavb_dev->bridge_port[i], enable_ptp);
		if (rc)
			goto err;
	}

	memset(&vport_cfg, 0, sizeof(struct soft_vport_cfg));
	if (cfg->flags & GENAVB_PORT_FLAGS_LOCAL_PORT) {
		vport_cfg.flags |= SW_VPORT_FLAGS_LOCAL_PORT;
		vport_cfg.port_out_cb = genavb_socket_output;
	} else
		vport_cfg.port_out_cb = genavb_host_to_sw_stack;
	vport_cfg.print_priv_stats = print_priv_stats;
	vport_cfg.link_up = &genavb_dev->link_up;
	/* frame bulk count to be handled by switch */
	vport_cfg.in_threshhold = 1;
	memcpy(vport_cfg.mac_addr, cfg->mac_addr, MAC_ADDR_LEN);
	strncpy(vport_cfg.name, cfg->name, sizeof(vport_cfg.name) - 1);
	vport_cfg.name[sizeof(vport_cfg.name) - 1] = '\0';

	vport_dev = soft_vport_init(switch_dev, &vport_cfg);

	vport_dev->priv = genavb_dev;
	genavb_dev->vport_dev = vport_dev;

	create_genavb_vport_thread(genavb_dev, enable_ptp);

	return (void *)genavb_dev;

err:
	/* Close all opend socket */
	for (i = 0; i < cfg->ep_port_num; i++) {
		if (genavb_dev->endpoint_port[i].sock_tx_opened)
			genavb_socket_tx_close(genavb_dev->endpoint_port[i].sock_tx);
		if (genavb_dev->endpoint_port[i].sock_rx_opened)
			genavb_socket_rx_close(genavb_dev->endpoint_port[i].sock_rx.sock);
	}
	for (i = 0; i < cfg->bridge_port_num; i++) {
			if (genavb_dev->bridge_port[i].sock_tx_opened)
				genavb_socket_tx_close(genavb_dev->bridge_port[i].sock_tx);
			if (genavb_dev->bridge_port[i].sock_rx_opened)
				genavb_socket_rx_close(genavb_dev->bridge_port[i].sock_rx.sock);
			if (genavb_dev->bridge_port[i].sock_tx_ptp_opened)
				genavb_socket_tx_close(genavb_dev->bridge_port[i].sock_tx_ptp);
			if (genavb_dev->bridge_port[i].sock_rx_ptp_opened)
				genavb_socket_rx_close(genavb_dev->bridge_port[i].sock_rx_ptp.sock);
	}

	os_free(genavb_dev);

	return NULL;
}
