/*
 * Copyright 2022-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_VIRTIO_H_
#define _APP_VIRTIO_H_

#include "FreeRTOS.h"

#define ENET_IRQ_PRIO			(portLOWEST_USABLE_INTERRUPT_PRIORITY - 1)

#define VIRTIO_NET_MEM_BASE		0xb8400000

#define ENET_PORT_BASE			ENET1
#define ENET_PORT_IRQ			ENET1_IRQn

extern void ENET1_DriverIRQHandler(void);
static inline void enet_hal_irq_handler(void)
{
	ENET1_DriverIRQHandler();
}

#endif /* _APP_VIRTIO_H_ */
