/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/list.h>
#include <net/data_pkt.h>
#include <net/net_switch.h>
#ifdef CONFIG_CPU_STATS
#include <os/cpu_load.h>
#endif
#include <os/stdbool.h>
#include <os/stdlib.h>
#include <os/stdio.h>
#include <os/string.h>
#include "os/unistd.h"

#include "switch_os.h"

#define STATS_PRINT_DELAY	1000	/* 1000ms */
#define STATS_PRINT_PERIOD	10	/* print cycle is 10*1000ms */


static bool is_broadcast_pkt(uint8_t *mac_addr)
{
	int i;

	/* Broadcast packet */
	for (i = 0; i < MAC_ADDR_LEN; i++)
		if (*mac_addr++ != 0xff)
			return false;

	return true;
}

static struct switch_port *find_dst_port(struct switch_device *dev, struct switch_port *src_port, uint8_t *mac_addr)
{
	struct switch_port *port;
	int i;

	list_for_each_entry(port, &dev->local_port_list, port_node) {
		if (port->power_on) {
			for (i = 0; i < MAC_ADDR_LEN; i++)
				if (*mac_addr++ != port->mac_addr[i])
					break;
			if (i == MAC_ADDR_LEN)
				return port;
		}
	}

	/* If there is no matched local port, the packet from local port will be sent to remote port */
	if (src_port->is_local)
		return dev->remote_port;

	return NULL;
}

void notify_ports(struct switch_device *dev)
{
	struct port_event p_event;
	struct switch_port *port;

	p_event.type = PORT_SEND;

	if (dev->remote_port && dev->remote_port->need_notify) {
		dev->remote_port->need_notify = false;
		os_mq_send(&dev->remote_port->mqueue, &p_event, 0, 0);
	}

	list_for_each_entry(port, &dev->local_port_list, port_node) {
		if (port->need_notify) {
			port->need_notify = false;
			os_mq_send(&port->mqueue, &p_event, 0, 0);
		}
	}
}

static void port_out_pkt(struct switch_port *dst_port, struct data_pkt *pkt)
{
	struct port_event p_event;

	p_event.type = PORT_SEND;
	os_sem_take(&dst_port->data_out_sem, 0, OS_SEM_TIMEOUT_MAX);
	list_add_tail(&dst_port->data_out, &pkt->pkt_entry);
	os_sem_give(&dst_port->data_out_sem, 0);
	if (dst_port->data_out.count >= 10)
		os_mq_send(&dst_port->mqueue, &p_event, 0, 0);
	else
		dst_port->need_notify = true;
}

static void broadcast_pkt(struct switch_device *dev, struct switch_port *src_port, struct data_pkt *pkt)
{
	struct switch_port *port;
	struct data_pkt *cloned_pkt;
	bool need_free = true;

	if (src_port->is_local) {
		list_for_each_entry(port, &dev->local_port_list, port_node) {
			if ( port != src_port && cable_link_is_on(&port->cable)) {
				cloned_pkt = data_pkt_clone(pkt);
				/* Add to out-port data_out list */
				if (cloned_pkt)
					port_out_pkt(port, cloned_pkt);
			}
		}
		if (cable_link_is_on(&dev->remote_port->cable)) {
			port_out_pkt(dev->remote_port, pkt);
			need_free = false;
		}
	} else {
		list_for_each_entry(port, &dev->local_port_list, port_node) {
			/* Bypass the port which link is off */
			if (!cable_link_is_on(&port->cable))
				continue;

			if (list_is_last(&dev->local_port_list, &port->port_node)) {
				port_out_pkt(port, pkt);
				need_free = false;
			} else {
				cloned_pkt = data_pkt_clone(pkt);
				/* Add to out-port data_out list */
				if (cloned_pkt)
					port_out_pkt(port, cloned_pkt);
			}
		}
	}

	if (need_free)
		data_pkt_free(pkt);
}

static void switch_pkt(struct switch_port *port, struct data_pkt *data_entry)
{
	struct switch_port *out_port;

	if (is_broadcast_pkt(data_entry->data)) {
		broadcast_pkt(port->switch_dev, port, data_entry);
	} else {
		out_port = find_dst_port(port->switch_dev, port, data_entry->data);
		if (out_port && cable_link_is_on(&out_port->cable))
			port_out_pkt(out_port, data_entry);
		else {
			/* Don't find any match out-port, discard it */
			data_pkt_free(data_entry);
			port->stats.in_dropped++;
		}
	}
}

void port_process_pkts(void *port_dev)
{
	struct switch_port *port = port_dev;
	struct port_event event;
	struct data_pkt *data_entry, *tmp;

	do {
		if (!os_mq_receive(&port->mqueue, &event, 0, OS_QUEUE_EVENT_TIMEOUT_MAX)) {
			if (event.type == PORT_STOP)
				break;
			list_for_each_entry_safe(data_entry, tmp, &port->data_out, pkt_entry) {
				port->port_out_cb(port, data_entry);
				port->stats.out++;
				if (data_entry->flags & DATA_PKT_FLAG_CAN_FREE) {
					os_sem_take(&port->data_out_sem, 0, OS_SEM_TIMEOUT_MAX);
					list_del(&data_entry->pkt_entry);
					os_sem_give(&port->data_out_sem, 0);
					data_pkt_free(data_entry);
				}
			}
		}
	} while (1);
}

void port_in_pkt_nocpy(void *port_dev, void *data_pkt, bool notify)
{
	struct switch_port *port = port_dev;
	struct data_pkt *pkt = data_pkt;

	port->stats.in++;
	switch_pkt(port, pkt);

	if (notify)
		notify_ports(port->switch_dev);
}

void port_in_pkt(void *port_dev, void *data, uint32_t data_len, bool notify)
{
	struct switch_port *port = port_dev;
	struct data_pkt *pkt;

	if (port->power_on == false)
		return;

	pkt = data_pkt_alloc(data_len, DATA_PKT_FLAG_CAN_FREE);
	if (!pkt) {
		os_printf("%s: failed to alloc data pkt\r\n", __func__);
		return;
	}

	memcpy(pkt->data, data, data_len);

	data_pkt_set_len(pkt, data_len);

	port_in_pkt_nocpy(port_dev, pkt, notify);
}

void *switch_init_port(void *switch_dev)
{
	struct switch_port *port;

	port = os_malloc(sizeof(struct switch_port));
	os_assert(port, "no memory for switch port");
	memset(port, 0, sizeof(struct switch_port));

	port->switch_dev = switch_dev;

	return port;
}

void switch_add_port(void *port_dev, struct port_config *config)
{
	struct switch_port *port = port_dev;
	struct switch_port *local_port;
	struct switch_device *dev = port->switch_dev;
	int err;

	strlcpy(port->name, config->name, sizeof(port->name));
	memcpy(port->mac_addr, config->mac_addr, sizeof(port->mac_addr));
	if (config->is_local && dev->remote_port)
		dev->remote_port->setup_address_cb(OPS_ADDR_ADD, config->mac_addr, NULL);

	port->port_out_cb = config->port_out_cb;
	port->is_local = config->is_local;

	port->print_priv_stats = config->print_priv_stats;

	list_init(&port->data_out);

	err = os_sem_init(&port->data_out_sem, 1);
	os_assert(!err, "port data_out semaphore initialization failed");

	err = os_mq_open(&port->mqueue, "port_mqueue", 100, sizeof(struct port_event));
	os_assert(!err, "port mqueue initialization failed");

	os_assert(config->dev_end_on, "dev_end_on is not initialized");
	port->cable.dev_end_on = config->dev_end_on;
	port->cable.sw_end_on = &port->power_on;
	port->power_on = false;

	create_port_thread(port);

	port->power_on = true;

	if (port->is_local) {
		os_sem_take(&dev->sem, 0, OS_SEM_TIMEOUT_MAX);
		list_add_tail(&dev->local_port_list, &port->port_node);
		os_sem_give(&dev->sem, 0);
	} else {
		dev->remote_port = port;
		os_assert(config->setup_address_cb, "Need to provide setup address callback for remote port");
		dev->remote_port->setup_address_cb = config->setup_address_cb;

		/* Add address for already added local ports */
		list_for_each_entry(local_port, &dev->local_port_list, port_node) {
			dev->remote_port->setup_address_cb(OPS_ADDR_ADD, local_port->mac_addr, NULL);
		}
	}
}

void port_update_addr(void *port_dev, void *mac_addr)
{
	struct switch_port *port = port_dev;
	struct switch_device *dev = port->switch_dev;

	if (dev->remote_port)
		dev->remote_port->setup_address_cb(OPS_ADDR_UPDATE, mac_addr, port->mac_addr);

	port->power_on = false;
	memcpy(port->mac_addr, mac_addr, sizeof(port->mac_addr));
	port->power_on = true;
}

void port_on(void *port_dev, bool on)
{
	struct switch_port *port = port_dev;

	port->power_on = on;
}

void switch_remove_port(void *port_dev)
{
	struct switch_port *port = port_dev;
	struct switch_device *dev = port->switch_dev;
	struct port_event event;
	struct data_pkt *data_entry, *tmp;

	event.type = PORT_STOP;
	os_mq_send(&port->mqueue, &event, 0, 0);

	os_sem_take(&port->data_out_sem, 0, OS_SEM_TIMEOUT_MAX);
	port->power_on = false;

	if (port->is_local) {
		os_sem_take(&dev->sem, 0, OS_SEM_TIMEOUT_MAX);
		list_del(&port->port_node);
		os_sem_give(&dev->sem, 0);
		if (dev->remote_port)
			dev->remote_port->setup_address_cb(OPS_ADDR_DEL, NULL, port->mac_addr);
	} else {
		dev->remote_port = NULL;
	}

	/* Discard all packets */
	list_for_each_entry_safe(data_entry, tmp, &port->data_out, pkt_entry) {
		if (data_entry->flags & DATA_PKT_FLAG_CAN_FREE) {
			list_del(&data_entry->pkt_entry);
			data_pkt_free(data_entry);
		}
	}

	os_free(port);
}

void *switch_init(void)
{
	struct switch_device *dev;
	int err;

	os_assert(sizeof(struct data_pkt) < DATA_PKT_HEAD_ROOM,
		"Data packet head room is too small\r\n");

	dev = os_malloc(sizeof(struct switch_device));
	os_assert(dev, "no memory for switch device");
	memset(dev, 0, sizeof(struct switch_device));

	list_init(&dev->local_port_list);

	err = os_sem_init(&dev->sem, 1);
	os_assert(!err, "switch semaphore initialization failed");

	err = os_mq_open(&dev->mqueue, "switch_mqueue", 10, sizeof(struct switch_event));
	os_assert(!err, "switch mqueue initialization failed");

	dev->power_on = true;

	return dev;
}

void switch_destroy(void *switch_dev)
{
	struct switch_device *dev = switch_dev;
	struct switch_port *port, *tmp;
	struct switch_event s_event;

	s_event.type = SWITCH_STOP;
	os_mq_send(&dev->mqueue, &s_event, 0, 0);

	/* Remove remote port */
	switch_remove_port(dev->remote_port);
	/* Remove local port */
	list_for_each_entry_safe(port, tmp, &dev->local_port_list, port_node) {
		switch_remove_port(port);
	}

	dev->power_on = false;

	os_free(switch_dev);
}

void switch_print_stats(void *switch_dev)
{
	struct switch_device *dev = switch_dev;
	struct switch_port *port, *tmp;
	unsigned int period = STATS_PRINT_PERIOD;

	do {
		if (period--) {
			os_msleep(STATS_PRINT_DELAY);
			continue;
		} else
			period = STATS_PRINT_PERIOD;

		if (dev->remote_port) {
			os_printf("[%s]:\r\t\t\tswitch in packets\t%llu\tswitch dropped\t%llu\tswitch out packets\t%llu\r\n",
				dev->remote_port->name, dev->remote_port->stats.in,
				dev->remote_port->stats.in_dropped,
				dev->remote_port->stats.out);
			os_printf("\r\t\t\tpackets in out queue\t%d\r\n",
				dev->remote_port->data_out.count);
			if (dev->remote_port->print_priv_stats)
				dev->remote_port->print_priv_stats();
		}
		list_for_each_entry_safe(port, tmp, &dev->local_port_list, port_node) {
			os_printf("[%s]:\r\t\t\tswitch in packets\t%llu\tswitch dropped\t%llu\tswitch out packets\t%llu\r\n",
				port->name, port->stats.in, port->stats.in_dropped,
				port->stats.out);
			os_printf("\r\t\t\tpackets in out queue\t%d\r\n",
				port->data_out.count);
			if (port->print_priv_stats)
				port->print_priv_stats();
		}

		os_printf("\r\n");
#ifdef CONFIG_CPU_STATS
		os_cpu_load_stats();
		os_printf("\r\n");
#endif
	} while(1);
}
