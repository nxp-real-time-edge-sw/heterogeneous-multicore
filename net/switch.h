/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SWITCH_OS_H_
#define _SWITCH_OS_H_

void switch_process_pkts(void *switch_dev);
void port_process_pkts(void *port_dev);
void switch_print_stats(void *switch_dev);

#endif /* _SWITCH_OS_H_ */
