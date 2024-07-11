/*
 * Copyright 2022,2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"

void vApplicationIdleHook(void)
{
#ifdef CONFIG_CPU_LOAD_STATS
extern void cpu_load_stats_hook(void);

	cpu_load_stats_hook();
#endif

	/* No more activity */
	__WFI();
}
