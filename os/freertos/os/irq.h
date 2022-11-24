/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FREERTOS_IRQ_H_
#define _FREERTOS_IRQ_H_

#include "fsl_common.h"
#include "FreeRTOS.h"

#if defined(__GIC_PRIO_BITS) /* For GIC */
#include <irq.h>

static inline int os_irq_register(unsigned int irq, void (*func)(void *data),
		void *data, unsigned int prio)
{
	return irq_register(irq, func, data, prio);
}

static inline int os_irq_unregister(unsigned int irq)
{
	return irq_unregister(irq);
}

static inline void os_irq_enable(unsigned int irq)
{
	EnableIRQ(irq);
}

static inline void os_irq_disable(unsigned int irq)
{
	DisableIRQ(irq);
}

static inline void os_irq_set_priority(unsigned int irq, unsigned int prio)
{
	if (prio)
		GIC_SetPriority(irq, prio << portPRIORITY_SHIFT);
}

#else /* For NVIC */

static inline int os_irq_register(unsigned int irq, void (*func)(void *data),
		void *data, unsigned int prio)
{
	if (prio)
		NVIC_SetPriority(irq, prio);

	return 0;
}

static inline int os_irq_unregister(unsigned int irq)
{
	return 0;
}

static inline void os_irq_enable(unsigned int irq)
{
	EnableIRQ(irq);
}

static inline void os_irq_disable(unsigned int irq)
{
	DisableIRQ(irq);
}

static inline void os_irq_set_priority(unsigned int irq, unsigned int prio)
{
	NVIC_SetPriority(irq, prio);
}

#endif

#endif /* #ifndef _FREERTOS_IRQ_H_ */
