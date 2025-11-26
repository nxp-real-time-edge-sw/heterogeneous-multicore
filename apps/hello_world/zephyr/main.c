/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <os/stdio.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#if __has_include("rtos_memory.h")
#include "rtos_memory.h"
#endif
#include "app_board.h"

#ifndef RTOSID
#define RTOSID		(0)
#endif

#define NUM_THREADS	CONFIG_MP_MAX_NUM_CPUS
#define STACK_SIZE	4096

K_SEM_DEFINE(hello_sem, 0, NUM_THREADS);
K_THREAD_STACK_ARRAY_DEFINE(hello_stack, NUM_THREADS, STACK_SIZE);

static struct k_thread hello_thread[NUM_THREADS];

uint64_t times[NUM_THREADS] = {0};

static uint64_t get_core_mpid(void)
{
#if defined(MPIDR_TO_CORE) && defined(GET_MPIDR)
	return MPIDR_TO_CORE(GET_MPIDR());
#else
	return 0;
#endif
}

#ifdef CONFIG_CPU_CORTEX_A55
static uint64_t mpid_list[] = {0x0, 0x100, 0x200, 0x300, 0x400, 0x500};
#elif defined(CONFIG_CPU_CORTEX_A53)
static uint64_t mpid_list[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5};
#else
#error "CPU Core is not supported"
#endif

#ifdef CONFIG_CPU_CORTEX_A55
static char cpu_name[] = "Cortex-A55";
#elif defined(CONFIG_CPU_CORTEX_A53)
static char cpu_name[] = "Cortex-A53";
#else
static char cpu_name[] = "Unknown";
#endif

static int get_core_id(uint64_t mpid)
{
	int i;

	for(i = 0; i < sizeof(mpid_list) / sizeof(mpid_list[0]); i++) {
		if (mpid_list[i] == mpid)
			return i;
	}

	return -1;
}

static void hello_func(void *p1, void *p2, void *p3)
{
	const char *name = k_thread_name_get(k_current_get());
	long my_id = (long)p1;
	uint64_t mpid = get_core_mpid();
	int id = get_core_id(mpid);

	do {
		k_sem_take(&hello_sem, K_FOREVER);
		os_printf("%s: hello %ld times from %s core%d (MPID: 0x%llx)\r\n", name, times[my_id]++, cpu_name, id, mpid);
	} while(1);
}

int main(void)
{
	int i;

	os_printf("%s: RTOS%d: ", CPU_CORE_NAME, RTOSID);
	os_printf("Hello World! Real-time Edge on %s\n", CONFIG_BOARD);

	for (i = 0; i < NUM_THREADS; i++) {
		/* Create hello threads */
		k_thread_create(&hello_thread[i], hello_stack[i], STACK_SIZE,
				hello_func, INT_TO_POINTER(i), NULL, NULL,
				K_HIGHEST_THREAD_PRIO, 0, K_FOREVER);

		char name[16];
		snprintf(name, sizeof(name), "Zephyr_thread_%d", i);
		k_thread_name_set(&hello_thread[i], name);

#ifdef CONFIG_SCHED_CPU_MASK
		k_thread_cpu_pin(&hello_thread[i], i);
#endif
		/* Start threads */
		k_thread_start(&hello_thread[i]);
	}

	do {
		/* Wakeup all hello threads */
		for (i = 0; i < NUM_THREADS; i++) {
			k_sem_give(&hello_sem);
		}
		k_sleep(K_SECONDS(1));

	} while(1);

	return 0;
}
