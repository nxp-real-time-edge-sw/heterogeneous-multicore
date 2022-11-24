/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "task.h"

#include <os/assert.h>

#include "switch.h"

#define port_task_PRIORITY   (configMAX_PRIORITIES - 1)
#define switch_task_PRIORITY   (configMAX_PRIORITIES - 1)

static void port_task(void *pvParameters)
{
	port_process_pkts(pvParameters);
	vTaskDelete(NULL);
}

void create_port_thread(void *port_dev)
{
	BaseType_t xResult;

	xResult = xTaskCreate(port_task, "port_task",
			configMINIMAL_STACK_SIZE + 100, port_dev,
			port_task_PRIORITY, NULL);
	os_assert(xResult == pdPASS, "data task creation failed");
}

static void switch_task(void *pvParameters)
{
	switch_process_pkts(pvParameters);
}

void create_switch_thread(void *switch_dev)
{
	BaseType_t xResult;

	xResult = xTaskCreate(switch_task, "switch_task",
			configMINIMAL_STACK_SIZE + 100, switch_dev,
			switch_task_PRIORITY, NULL);
	os_assert(xResult == pdPASS, "switch task creation failed");
}
