/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_VIRTIO_H_
#define _APP_VIRTIO_H_

#include <os/stdlib.h>

#define VIRTIO_NET_MEM_BASE	0xa8400000

#define ENET_IRQ_PRIO		(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1)

#define MU_IRQ_NUM		MU1_A_IRQn
#define MU_CM_IF		MU1_MUA

#define ENET_PORT_BASE		ENET
#define ENET_PORT_IRQ		ENET_IRQn

extern void ENET_DriverIRQHandler(void);
static inline void enet_hal_irq_handler(void)
{
	ENET_DriverIRQHandler();
}

extern int32_t mu_irq_handler(void);

int32_t MU1_A_IRQHandler(void)
{
	return mu_irq_handler();
}

#endif /* _APP_VIRTIO_H_ */
