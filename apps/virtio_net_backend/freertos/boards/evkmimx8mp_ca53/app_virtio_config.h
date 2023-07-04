/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_VIRTIO_H_
#define _APP_VIRTIO_H_

#include "FreeRTOS.h"

#define CONFIG_CPU_STATS	1

#define VIRTIO_GEN_SW_MBOX_BASE		(0xfe000000)
#define VIRTIO_GEN_SW_MBOX_IRQ		(183)
#define VIRTIO_GEN_SW_MBOX_REMOTE_IRQ	(182)
#define VIRTIO_GEN_SW_MBOX_IRQ_PRIO	(portLOWEST_USABLE_INTERRUPT_PRIORITY - 1)

#define ENET_IRQ_PRIO			(portLOWEST_USABLE_INTERRUPT_PRIORITY - 1)

#define VIRTIO_NET_MEM_BASE		0xfc700000

#define ENET_PORT_BASE			ENET1
#define ENET_PORT_IRQ			ENET1_IRQn

extern void ENET1_DriverIRQHandler(void);
static inline void enet_hal_irq_handler(void)
{
	ENET1_DriverIRQHandler();
}

#endif /* _APP_VIRTIO_H_ */
