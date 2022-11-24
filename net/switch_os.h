/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SWITCH_OS_H_
#define _SWITCH_OS_H_

void create_port_thread(void *port_dev);
void create_switch_thread(void *switch_dev);

#endif /* _SWITCH_OS_H_ */
