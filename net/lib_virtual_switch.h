/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _LIB_VIRTUAL_SWITCH_H_
#define _LIB_VIRTUAL_SWITCH_H_

#include <stdint.h>

void *switch_init(void);
void create_stats_thread(void *switch_dev);
void port_update_addr(void *port_dev, void *mac_addr);
int vs_port_add_fdb_entry(void *port_dev, void *mac_addr);
int vs_port_remove_fdb_entry(void *port_dev, void *mac_addr);

#endif /* _LIB_VIRTUAL_SWITCH_H_ */
