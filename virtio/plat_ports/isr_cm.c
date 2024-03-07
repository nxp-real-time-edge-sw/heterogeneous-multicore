/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "os/assert.h"

#include "fsl_device_registers.h"

void platform_global_isr_disable(void)
{
	__asm volatile("cpsid i");
}

void platform_global_isr_enable(void)
{
	__asm volatile("cpsie i");
}

/*
 * platform_in_isr
 * Return whether CPU is processing IRQ
 * @return True for IRQ, false otherwise.
 *
 */
int32_t platform_in_isr(void)
{
	return (((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0UL) ? 1 : 0);
}
