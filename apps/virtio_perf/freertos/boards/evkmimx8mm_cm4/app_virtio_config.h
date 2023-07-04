/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_VIRTIO_H_
#define _APP_VIRTIO_H_

//#define CONFIG_NOTIFY_SGI	1
//#define CONFIG_CPU_STATS	1

#define VIRTIO_TRANS_MEM_BASE	0xb8400000

#define MU_IRQ_NUM			MU_M4_IRQn
#define MU_CM_IF			MUB

extern int32_t mu_irq_handler(void);

int32_t MU_M4_IRQHandler(void)
{
	return mu_irq_handler();
}

#endif /* _APP_VIRTIO_H_ */
