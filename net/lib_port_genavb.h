/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _LIB_PORT_GENAVB_H_
#define _LIB_PORT_GENAVB_H_

#include <stdint.h>
#include <stdbool.h>

#define GENAVB_PORT_FLAGS_LOCAL_PORT       (1U << 0)
#define GENAVB_PORT_FLAGS_RX_INSERT_VLAN   (1U << 1)
#define GENAVB_PORT_FLAGS_PTP_SOCKET       (1U << 2)
#define GENAVB_PORT_FLAGS_RX_INSERT_DSA    (1U << 3)   /* Insert DSA tag with timestamp */
#define GENAVB_PORT_FLAGS_TX_REMOVE_VLAN   (1U << 4)
#define GENAVB_PORT_FLAGS_TX_REMOVE_DSA    (1U << 5)

#define MAX_NAME_LEN 20
#define MAC_ADDR_LEN 6
#define CFG_EP_NUM 2
#define CFG_BRIDGE_NUM 4

struct genavb_port_cfg {
    char name[MAX_NAME_LEN];
    bool is_hostport;

    uint8_t mac_addr[MAC_ADDR_LEN];
    /* Only used for remote port */
    uint8_t host_mac[MAC_ADDR_LEN];

    uint8_t ep_port_num;
    uint16_t ep_port_id[CFG_EP_NUM]; /* logical port id for endpoint port from 0 to CFG_EP_NUM*/
    /* bridge port only valid with pseudo ep port */
    uint8_t bridge_port_num;
    struct bridge_port_id {
        uint16_t logical_id;
        uint16_t hardware_id;
    } bridge_port_id[CFG_BRIDGE_NUM]; /* logical port id for bridge port from 0 to CFG_BRIDGE_NUM*/

    uint16_t gptp_clock_id;

    uint32_t flags;
};

void *port_genavb_init(void *switch_dev, struct genavb_port_cfg *cfg);
void port_genavb_link_up(void *port, bool up);
void port_genavb_setup_host_mac(void *dev, void *host_mac);
void port_genavb_update_addr(void *dev, void *mac_addr);
int port_genavb_add_fdb_entry(void *dev, void *mac_addr);
int port_genavb_remove_fdb_entry(void *dev, void *mac_addr);

#endif /* _LIB_PORT_GENAVB_H_ */
