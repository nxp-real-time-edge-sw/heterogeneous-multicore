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

#include "hello_world_board.h"

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
__WEAK uint64_t get_core_mpid(void)
{
	return 0;
}

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void hello_task(void *pvParameters)
{
#define TIME_DELAY_SLEEP      (1 * configTICK_RATE_HZ)
	PRINTF("\r\n");
	PRINTF("%s: RTOS%d: ", CPU_CORE_NAME, RTOSID);
	PRINTF("Hello world! Real-time Edge on %s\r\n", BOARD_NAME);

	for (;;)
	{
		os_printf("hello_thread_0: hello from core (MPID: 0x%llx)\r\n", get_core_mpid());
		vTaskDelay(TIME_DELAY_SLEEP);
	}
}

/*!
 * @brief Application entry point.
 */
int main(void)
{
	hello_world_board_init();

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
