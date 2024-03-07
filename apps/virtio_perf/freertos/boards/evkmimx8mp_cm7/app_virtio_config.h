/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_VIRTIO_H_
#define _APP_VIRTIO_H_

#define VIRTIO_TRANS_MEM_BASE	0xb8400000

#define MU_IRQ_NUM		MU1_M7_IRQn
#define MU_CM_IF		MUB

extern int32_t mu_irq_handler(void);

int32_t MU1_M7_IRQHandler(void)
{
	return mu_irq_handler();
}

#endif /* _APP_VIRTIO_H_ */
