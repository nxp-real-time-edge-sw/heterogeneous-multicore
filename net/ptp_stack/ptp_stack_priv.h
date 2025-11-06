/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PORT_GENAVB_PRIV_H_
#define _PORT_GENAVB_PRIV_H_

#include <os/mqueue.h>

#include "lib_port_genavb.h"

struct genavb_priv_stats {
	/* Received pkts by hardware port without any error */
	uint64_t rx_pkts;
	/* Dropped pkts with errors */
	uint64_t rx_err_pkts;
	/* Transmitted pkts by hardware port */
	uint64_t tx_pkts;
	/* Transmitted pkts with errors */
	uint64_t tx_err_pkts;
	/* only used by SW port, meta frames sent from host to SW */
	uint64_t meta_rx_pkts;
	/* only used by SW port, invalid frame sent from host to SW */
	uint64_t invalid_host_pkts;
	/* only used by SW port, retrieved tx timestamp frames sent from sw to host */
	uint64_t tx_ts_pkts;
};

struct socket_rx_dev {
	struct genavb_socket_rx *sock;
	struct gen_avb_port_dev *port;
	bool is_ptp_sock;
};

struct gen_avb_port_dev {
	uint16_t logical_port_id;
	uint16_t hardware_id;

	bool is_bridge_port;
	/* only used for bridge port*/
	uint16_t bridge_port_index;

	struct genavb_socket_tx *sock_tx;
	struct socket_rx_dev sock_rx;
	bool sock_tx_opened;
	bool sock_rx_opened;

	struct genavb_socket_tx *sock_tx_ptp;
	struct socket_rx_dev sock_rx_ptp;
	bool sock_tx_ptp_opened;
	bool sock_rx_ptp_opened;

	struct gen_avb_device *genavb_dev;
};

struct gen_avb_device {
	bool link_up;
	bool is_hostport;

	uint32_t flags;

	uint8_t ep_port_num;
	struct gen_avb_port_dev endpoint_port[CFG_EP_NUM];

	/* bridge port only used with pseudo endpoint port */
	uint8_t bridge_port_num;
	struct gen_avb_port_dev bridge_port[CFG_BRIDGE_NUM];

	uint16_t gptp_clock_id;

	/* host MAC address used only by remote port */
	uint8_t host_mac[MAC_ADDR_LEN];

	void *handle;
	void *vport_dev;

	os_mqd_t mqueue;
	os_mqd_t ptp_ts_mqueue;

	struct genavb_priv_stats priv_stats;
};

enum net_flags {
	NET_OK,
	NET_NO_FRAME,
	NET_ERR,
};

typedef enum {
	GENAVB_RCV,
	GENAVB_SEND,
} genavb_event_type_t;

struct genavb_port_event {
	genavb_event_type_t type;
	void *data;
};

void genavb_receive_pkts(void *param);
void genavb_retrieve_ptp_ts(void *param);

#endif /* _PORT_GENAVB_PRIV_H_ */
