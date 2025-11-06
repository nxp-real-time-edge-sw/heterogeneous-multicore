/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NET_SWITCH_H_
#define _NET_SWITCH_H_

#include <common/list.h>
#include <os/mqueue.h>
#include <os/semaphore.h>

#define MAC_ADDR_LEN	6

enum {
	OPS_ADDR_ADD,
	OPS_ADDR_DEL,
	OPS_ADDR_UPDATE,
};

struct port_pkts_stat {
	uint64_t in;
	uint64_t in_dropped;
	uint64_t out;
};

struct switch_device {
	bool power_on;

	/* remote port */
	struct switch_port *remote_port;
	/* local port */
	struct list_head local_port_list;

	os_sem_t sem;
	os_mqd_t mqueue;
};

/* VNET Cable between switch and device */
struct vnet_cable {
	/* Switch End Link On */
	bool *sw_end_on;
	/* Device End Link On */
	bool *dev_end_on;
};

/* FDB Entry */
struct fdb_entry {
	struct list_head fdb_node;
	uint8_t mac_addr[MAC_ADDR_LEN];
};

#define MAX_NAME_LEN 20

struct switch_port {
	struct switch_device *switch_dev;

	struct port_pkts_stat stats;

	/* True for local port, false for remote port */
	bool is_local;

	char name[MAX_NAME_LEN];
	uint8_t mac_addr[MAC_ADDR_LEN];

	bool power_on;
	struct vnet_cable cable;

	struct list_head data_out;

	os_sem_t data_out_sem;

	os_mqd_t mqueue;

	/* Callback function to transmit data packets through this port */
	int (*port_out_cb)(struct switch_port *port, void *data_paket);

	/* Optional print function for port's private statitistics */
	void (*print_priv_stats)(struct switch_port *port);

	/* The fields only used by local port */
	struct list_head port_node;
	bool fdb_enabled;
	struct list_head fdb_table; /* Forwarding Data Base table */

	/* The fields only used by remote port */
	/* Callback function to update addresses */
	int (*setup_address_cb)(uint8_t ops, uint8_t *new_addr, uint8_t* old_addr);
	/* port priv */
	void *priv;

	bool need_notify;
};

struct port_config {
	/* True for local port, false for remote port */
	bool is_local;

	char name[MAX_NAME_LEN];
	uint8_t mac_addr[MAC_ADDR_LEN];

	/* vnet cable device end status */
	bool *dev_end_on;

	int (*port_out_cb)(struct switch_port *port, void *data_paket);

	/* Optional print function for port's private statitistics */
	void (*print_priv_stats)(struct switch_port *port);

	/* The fields only used by remote port */
	/* Callback function to update addresses */
	int (*setup_address_cb)(uint8_t ops, uint8_t *new_addr, uint8_t* old_addr);
};

typedef enum {
	PORT_SEND,
	PORT_STOP,
} p_event_type_t;

struct port_event {
	p_event_type_t type;
	uintptr_t data;
};

typedef enum {
	SWITCH_SEND,
	SWITCH_STOP,
} s_event_type_t;

struct switch_event {
	s_event_type_t type;
	uintptr_t data;
};

void *switch_init(void);
void switch_destroy(void *switch_dev);
void *switch_init_port(void *switch_dev);
void switch_add_port(void *switch_dev, struct port_config *config);
void switch_remove_port(void *port_dev);
void port_update_addr(void *port_dev, void *mac_addr);
void port_on(void *port_dev, bool on);
void port_in_pkt(void *port_dev, void *data, uint32_t data_len, bool notify);
void port_in_pkt_nocpy(void *port_dev, void *data_pkt, bool notify);
void switch_print_stats(void *switch_dev);
void notify_ports(struct switch_device *dev);
int vs_port_add_fdb_entry(void *port_dev, void *mac_addr);
int vs_port_remove_fdb_entry(void *port_dev, void *mac_addr);

static inline bool cable_link_is_on(struct vnet_cable *cable)
{
	return (*cable->sw_end_on && *cable->dev_end_on);
};

#endif /* _NET_SWITCH_H_ */
