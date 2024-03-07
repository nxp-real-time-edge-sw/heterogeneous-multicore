/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
#define tictak_task_PRIORITY (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void hello_task(void *pvParameters)
{
	PRINTF("\r\n");
	for (;;)
	{
		PRINTF("%s: RTOS%d: ", CPU_CORE_NAME, RTOSID);
		PRINTF("Hello world! Real-time Edge on %s\r\n", BOARD_NAME);
		print_ram_console_addr();
		vTaskSuspend(NULL);
	}
}

/*!
 * @brief function responsible for printing of "tic tac" messages.
 */
static void tictac_task(void *pvParameters)
{
    unsigned long long count = 0;
#define TIME_DELAY_SLEEP      (1 * configTICK_RATE_HZ)

    for (;;)
    {
        vTaskDelay(TIME_DELAY_SLEEP);

        if (++count % 2)
            PRINTF("tic ");
        else
            PRINTF("tac ");

        if (!(count % 20))
            PRINTF("\r\n");
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
	if (xTaskCreate(tictac_task, "tictak_task", configMINIMAL_STACK_SIZE + 100, NULL, tictak_task_PRIORITY, NULL) !=
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
