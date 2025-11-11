/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <net/data_pkt.h>
#include <net/net_debug.h>
#include <net/net_switch.h>
#include <net/net_types.h>
#include <os/stdlib.h>
#include <os/string.h>

#include "vport_software.h"
#include "lib_virtual_switch.h"


/* Transmit frames from Virtual Port to Virtual Switch*/
void soft_vport_in_nocpy(void *dev, struct data_pkt *pkt)
{
	struct soft_vport_dev *vport_dev = dev;
	bool notify = false;

	if (vport_dev->in_count++ > vport_dev->in_threshhold) {
		notify = true;
		vport_dev->in_count = 0;
	}
	port_in_pkt_nocpy(vport_dev->port_dev, pkt, notify);
}

/* Forcely notify Virtual Switch to wakeup to switch frames*/
void soft_vport_in_kick(void *dev)
{
	struct soft_vport_dev *sw_vport_dev = dev;

	if (sw_vport_dev->in_count > 0) {
		sw_vport_dev->in_count = 0;
		notify_ports(sw_vport_dev->port_dev->switch_dev);
	}
}

void soft_vport_update_addr(void *dev, void *mac_addr)
{
	struct soft_vport_dev *sw_vport_dev = dev;

	port_update_addr(sw_vport_dev->port_dev, mac_addr);
}

int soft_vport_add_fdb_entry(void *dev, void *mac_addr)
{
	struct soft_vport_dev *sw_vport_dev = dev;

	return vs_port_add_fdb_entry(sw_vport_dev->port_dev, mac_addr);
}

int soft_vport_remove_fdb_entry(void *dev, void *mac_addr)
{
	struct soft_vport_dev *sw_vport_dev = dev;

	return vs_port_remove_fdb_entry(sw_vport_dev->port_dev, mac_addr);
}

/*
 * TODO: update hardware MAC address by though soft virtual port 
 */
static int vs_port_setup_macaddr(uint8_t addr_ops, uint8_t *new_addr, uint8_t *old_addr)
{
	return 0;
}

static int soft_vport_socket_output(struct switch_port *port, void *data_packet)
{
	struct soft_vport_dev *sw_vport_dev = port->priv;

	return sw_vport_dev->port_out_cb(sw_vport_dev, data_packet);
}

static void soft_vport_priv_stats(struct switch_port *port)
{
	struct soft_vport_dev *sw_vport_dev = port->priv;

	sw_vport_dev->print_priv_stats(sw_vport_dev);
}

void *soft_vport_init(void *switch_dev, struct soft_vport_cfg *cfg)
{
	struct soft_vport_dev *sw_vport_dev;
	struct port_config config;
	struct switch_port *port_dev;

	sw_vport_dev = os_malloc(sizeof(struct soft_vport_dev));
	os_assert(sw_vport_dev, "no memory for software vport device");
	memset(sw_vport_dev, 0, sizeof(struct soft_vport_dev));

	sw_vport_dev->in_threshhold = cfg->in_threshhold;
	sw_vport_dev->in_count = 0;

	port_dev = switch_init_port(switch_dev);
	sw_vport_dev->port_dev = (void *)port_dev;
	port_dev->priv = (void *)sw_vport_dev;



	os_assert(cfg->port_out_cb, "No out callback provided");
	os_assert(cfg->print_priv_stats, "No printing private stats callback provided");
	sw_vport_dev->port_out_cb = cfg->port_out_cb;
	sw_vport_dev->print_priv_stats = cfg->print_priv_stats;

	memset(&config, 0, sizeof(struct port_config));
	config.dev_end_on = cfg->link_up;
	config.is_local = (cfg->flags & SW_VPORT_FLAGS_LOCAL_PORT);
	config.port_out_cb = soft_vport_socket_output;
	config.print_priv_stats = soft_vport_priv_stats;

	memcpy(config.mac_addr, cfg->mac_addr, MAC_ADDR_LEN);
	config.setup_address_cb = vs_port_setup_macaddr;
	strncpy(config.name, cfg->name, sizeof(config.name) - 1);
	config.name[sizeof(config.name) - 1] = '\0';
	switch_add_port(port_dev, &config);

	return (void *)sw_vport_dev;
}
