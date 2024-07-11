/*
 * Copyright 2022,2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ZEPHYR_CPU_LOAD_H_
#define _ZEPHYR_CPU_LOAD_H_

static inline void os_cpu_load_stats(void)
{
#ifdef CONFIG_SCHED_THREAD_USAGE
	k_thread_runtime_stats_t stats_all;
	static uint64_t last_execution_cycles = 0, last_total_cycles = 0;
	float cpu_load;

	if (k_thread_runtime_stats_all_get(&stats_all) < 0)
		return;

	cpu_load = (100. * (stats_all.total_cycles - last_total_cycles)) /
		(stats_all.execution_cycles - last_execution_cycles);
	last_total_cycles = stats_all.total_cycles;
	last_execution_cycles = stats_all.execution_cycles;

	os_printf("CPU load: %.2f%%\n", cpu_load);
#endif
}

#endif /* #ifndef _ZEPHYR_CPU_LOAD_H_ */
