/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "task.h"

#include <os/assert.h>

#include "port_enet_priv.h"

#define enet_task_PRIORITY   (configMAX_PRIORITIES - 3)

static void enet_task(void *pvParameters)
{
	/* dead lock */
	enet_process_pkts(pvParameters);
	vTaskDelete(NULL);
}

void create_enet_thread(void *enet_dev)
{
	BaseType_t xResult;

	xResult = xTaskCreate(enet_task, "enet_task",
			configMINIMAL_STACK_SIZE + 100, enet_dev,
			enet_task_PRIORITY, NULL);
	os_assert(xResult == pdPASS, "enet task creation failed");
}
