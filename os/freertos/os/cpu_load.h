/*
 * Copyright 2022,2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FREERTOS_CPU_LOAD_H_
#define _FREERTOS_CPU_LOAD_H_

#include <cpu_load.h>

#ifdef CONFIG_CPU_LOAD_STATS
#define os_cpu_load_stats() cpu_load_stats()
#else
#define os_cpu_load_stats()
#endif

#endif /* #ifndef _FREERTOS_CPU_LOAD_H_ */
