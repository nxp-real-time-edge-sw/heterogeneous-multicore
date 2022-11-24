/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SWITCH_OS_H_
#define _SWITCH_OS_H_

void switch_process_pkts(void *switch_dev);
void port_process_pkts(void *port_dev);

#endif /* _SWITCH_OS_H_ */
