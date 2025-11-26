/*
 * Copyright 2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "board.h"
#include "clock_config.h"
#include "os/stdio.h"
#include "os/unistd.h"
#include "pin_mux.h"

#include "app.h"

#include "fsl_debug_console.h"

#ifndef RTOSID
#define RTOSID		(0)
#endif


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Task priorities. */
#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static uint64_t times = 0;

__WEAK uint64_t get_core_mpid(void)
{
	return 0;
}

#ifdef CPU_CORTEX_A55
static uint64_t mpid_list[] = {0x0, 0x100, 0x200, 0x300, 0x400, 0x500};
#elif defined(CPU_CORTEX_A53)
static uint64_t mpid_list[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5};
#else
static uint64_t mpid_list[] = {0x0};
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

#define TIME_DELAY_SLEEP      (1 * configTICK_RATE_HZ)
/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void hello_task(void *pvParameters)
{
	uint64_t mpid = get_core_mpid();
	int id = get_core_id(mpid);

	PRINTF("\r\n");
	PRINTF("%s: RTOS%d: ", CPU_CORE_NAME, RTOSID);
	PRINTF("Hello world! Real-time Edge on %s\r\n", BOARD_NAME);

	for (;;)
	{
		os_printf("FreeRTOS_thread_0: hello %lld times from %s core%d (MPID: 0x%llx)\r\n", times++, CPU_CORE_NAME, id, mpid);
		vTaskDelay(TIME_DELAY_SLEEP);
	}
}

/*!
 * @brief Application entry point.
 */
int main(void)
{
	BOARD_InitHardware();

	if (xTaskCreate(hello_task, "Hello_task", configMINIMAL_STACK_SIZE + 100, NULL, hello_task_PRIORITY, NULL) !=
			pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1)
			;
	}

	vTaskStartScheduler();
	for (;;)
		;
}
