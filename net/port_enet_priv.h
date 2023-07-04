/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PORT_ENET_PRIV_H_
#define _PORT_ENET_PRIV_H_

#include <os/mqueue.h>

#include "fsl_phy.h"

struct enet_device {
	os_mqd_t mqueue;

	bool link_up;
	phy_speed_t speed;
	phy_duplex_t duplex;

	void *handle;
	void *port_dev;
};

typedef enum {
	ENET_RCV,
	ENET_SEND,
} enet_event_type_t;

struct enet_event {
	enet_event_type_t type;
	uintptr_t data;
};


void enet_process_pkts(void *param);
void enet_phy_status(void *dev);

#endif /* _PORT_ENET_PRIV_H_ */
