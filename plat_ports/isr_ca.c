/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "os/assert.h"

extern uint64_t ullPortInterruptNesting;

void platform_global_isr_disable(void)
{
	__asm volatile ( "MSR DAIFSET, #2" ::: "memory" );
	__asm volatile ( "DSB SY" );
	__asm volatile ( "ISB SY" );
}

void platform_global_isr_enable(void)
{
	__asm volatile ( "MSR DAIFCLR, #2" ::: "memory" );
	__asm volatile ( "DSB SY" );
	__asm volatile ( "ISB SY" );
}

/*
 * platform_in_isr
 * Return whether CPU is processing IRQ
 * @return True for IRQ, false otherwise.
 */
int32_t platform_in_isr(void)
{
	/* This is only working for FreeRTOS */
	return (ullPortInterruptNesting > 0);
}
