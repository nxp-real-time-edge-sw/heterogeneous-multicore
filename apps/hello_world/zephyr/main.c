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

static uint64_t get_core_mpid(void)
{
#if defined(MPIDR_TO_CORE) && defined(GET_MPIDR)
	return MPIDR_TO_CORE(GET_MPIDR());
#else
	return 0;
#endif
}

static void hello_func(void *p1, void *p2, void *p3)
{
	const char *name = k_thread_name_get(k_current_get());

	do {
		k_sem_take(&hello_sem, K_FOREVER);
		os_printf("%s: hello from core (MPID: 0x%llx)\r\n", name, get_core_mpid());
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
		snprintf(name, sizeof(name), "hello_thread_%d", i);
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
