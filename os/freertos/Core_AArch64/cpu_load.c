/*
 * Copyright 2022,2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "os/stdio.h"

#ifdef CONFIG_CPU_LOAD_STATS
static uint64_t cpu_idle = 0, cpu_busy = 0;
uint64_t cpu_load_work_begin;

void cpu_load_stats_hook(void)
{
	static uint64_t cpu_load_work_end;

	/*
	 * CPU load:
	 * cpu time is divided into two pieces, idle and busy
	 * idle starts just before calling WFI below and it ends on entry to the first IRQ
	 * busy starts on entry to first IRQ and ends here
	 * IRQ entry is trapped in vApplicationIRQHandler() and a flag (cpu_load_work_begin = 0) is used to determine the first one
     */
	portDISABLE_INTERRUPTS();
	cpu_idle += cpu_load_work_begin - cpu_load_work_end;
	ARM_TIMER_GetCounterCount(ARM_TIMER_VIRTUAL, &cpu_load_work_end);
	cpu_busy += cpu_load_work_end - cpu_load_work_begin;
	cpu_load_work_begin = 0;
	portENABLE_INTERRUPTS();
}

void cpu_load_stats(void)
{
	float cpu_load;

	if (cpu_idle == 0)
		os_printf("CPU load: 100%%\r\n");

	if (cpu_idle + cpu_busy > 0) {
		cpu_load = (100. * cpu_busy) / (cpu_idle + cpu_busy);
		os_printf("CPU load: %.2f%%\r\n", cpu_load);
	}
	cpu_idle = 0;
	cpu_busy = 0;
}
#endif
