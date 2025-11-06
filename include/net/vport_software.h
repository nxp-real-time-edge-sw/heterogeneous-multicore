/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PORT_ENET_H_
#define _PORT_ENET_H_

#include <net/net_switch.h>
#include <os/stdbool.h>

#define MAC_ADDR_LEN	6

#define SW_VPORT_FLAGS_LOCAL_PORT (1U << 0)

struct soft_vport_dev {
	bool link_up;
	uint8_t in_count;	/* The counter for swiched in frames, will be reset to zero after each switch kick */
	uint8_t in_threshhold; /* the threshhold of count of switched in frames to kick switch to work */

	struct switch_port *port_dev; /* Virtual Port handler */
	void *priv;

	/* Callback function to switch out frames from switch to virtual port */
	int (*port_out_cb)(struct soft_vport_dev *port, void *data_paket);
	/* Callback function to print private stats */
	void (*print_priv_stats)(struct soft_vport_dev *port);
};

struct soft_vport_cfg {
	char name[MAX_NAME_LEN];
	uint8_t mac_addr[MAC_ADDR_LEN];
	bool *link_up;
	uint8_t in_threshhold; /* the threshhold of count of switched in frames to kick switch to work */
	uint32_t flags;

	/* Callback function to switch out frames from switch to virtual port */
	int (*port_out_cb)(struct soft_vport_dev *port, void *data_paket);
	/* Callback function to print private stats */
	void (*print_priv_stats)(struct soft_vport_dev *port);
};

void *soft_vport_init(void *switch_dev, struct soft_vport_cfg *cfg);
int soft_vport_add_fdb_entry(void *dev, void *mac_addr);
int soft_vport_remove_fdb_entry(void *dev, void *mac_addr);
void soft_vport_update_addr(void *dev, void *mac_addr);
void soft_vport_in_nocpy(void *dev, struct data_pkt *pkt);
void soft_vport_in_kick(void *dev);

#endif /* _PORT_ENET_H_ */
