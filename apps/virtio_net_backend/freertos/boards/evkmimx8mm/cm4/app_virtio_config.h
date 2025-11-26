/*
 * Copyright 2022-2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_VIRTIO_H_
#define _APP_VIRTIO_H_

#include <os/stdlib.h>

#define VIRTIO_NET_MEM_BASE	0xb8400000

#define ENET_IRQ_PRIO	(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1)

#define MU_IRQ_NUM			MU_M4_IRQn
#define MU_CM_IF			MUB

#define ENET_PORT_BASE			ENET1
#define ENET_PORT_IRQ			ENET1_IRQn

extern void ENET_DriverIRQHandler(void);
static inline void enet_hal_irq_handler(void)
{
	ENET_DriverIRQHandler();
}

#endif /* _APP_VIRTIO_H_ */
