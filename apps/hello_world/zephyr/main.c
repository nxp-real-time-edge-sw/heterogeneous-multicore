/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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

#define STACK_SIZE	4096

K_THREAD_STACK_DEFINE(hello_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(tictac_stack, STACK_SIZE);

static void print_ram_console_addr(void)
{
#if (RTOSID == 0 && RTOS_CNT > 1)
	int i;

	for (i = 1; i < RTOS_CNT; i++)
		os_printf("RTOS%d: RAM console@0x%x\r\n", i,
			  RAM_CONSOLE_ADDR + RTOS_INST_STRIDE * i);
#endif
}

static void hello_func(void *p1, void *p2, void *p3)
{
	os_printf("%s: RTOS%d: ", CPU_CORE_NAME, RTOSID);
	os_printf("Hello World! Real-time Edge on %s\n", CONFIG_BOARD);
	print_ram_console_addr();

	do {
		k_msleep(500);
		k_thread_suspend(k_current_get());

	} while(1);
}

static void tictac_func(void *p1, void *p2, void *p3)
{
	unsigned long long count = 0;

	do {
		k_msleep(1000);

		if (++count % 2)
			os_printf("tic ");
		else
			os_printf("tac ");

		if (!(count % 20))
			os_printf("\n");
	} while(1);
}

int main(void)
{
	struct k_thread hello_thread;
	struct k_thread tictac_thread;

	/* tic tac thread */
	k_thread_create(&tictac_thread, tictac_stack, STACK_SIZE,
			tictac_func, NULL, NULL, NULL,
			K_LOWEST_APPLICATION_THREAD_PRIO, 0, K_FOREVER);

	/* Hello thread */
	k_thread_create(&hello_thread, hello_stack, STACK_SIZE,
			hello_func, NULL, NULL, NULL,
			K_HIGHEST_THREAD_PRIO, 0, K_FOREVER);

	/* Start threads */
	k_thread_start(&tictac_thread);
	k_thread_start(&hello_thread);

	do {
		k_msleep(100);

	} while(1);

	return 0;
}
