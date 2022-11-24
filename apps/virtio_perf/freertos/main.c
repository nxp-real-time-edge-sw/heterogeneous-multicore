/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "board.h"
#include "virtio_board.h"
#include "virtio-trans.h"
#include "app_virtio_config.h"
#include "os/stdio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Task priorities. */
#define virtio_task_PRIORITY (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void virtio_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */
int main(void)
{
	BaseType_t xResult;

	virtio_board_init();

	xResult = xTaskCreate(virtio_task, "Virtio_task", configMINIMAL_STACK_SIZE + 100, NULL, virtio_task_PRIORITY, NULL);
	assert(xResult == pdPASS);

	vTaskStartScheduler();
	for (;;)
		;

	return xResult;
}

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void virtio_task(void *pvParameters)
{
	int ret;

	ret = virtio_trans_init((void *)VIRTIO_TRANS_MEM_BASE);
	os_printf("virtio transfer device initialization %s!\r\n", ret ? "failed": "succeed");

	vTaskSuspend(NULL);
}
