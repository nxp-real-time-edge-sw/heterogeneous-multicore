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

/* lowest priority */
#define phy_task_PRIORITY    (1)

static void enet_task(void *pvParameters)
{
	/* dead lock */
	enet_process_pkts(pvParameters);
	vTaskDelete(NULL);
}

static void phy_task(void *pvParameters)
{
	/* dead lock */
	enet_phy_status(pvParameters);
	vTaskDelete(NULL);
}

void create_enet_thread(void *enet_dev)
{
	BaseType_t xResult;

	xResult = xTaskCreate(enet_task, "enet_task",
			configMINIMAL_STACK_SIZE + 100, enet_dev,
			enet_task_PRIORITY, NULL);
	os_assert(xResult == pdPASS, "enet task creation failed");

	xResult = xTaskCreate(phy_task, "phy_task",
			configMINIMAL_STACK_SIZE + 100, enet_dev,
			phy_task_PRIORITY, NULL);
	os_assert(xResult == pdPASS, "enet phy task creation failed");
}
