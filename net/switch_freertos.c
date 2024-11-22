/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "task.h"

#include <os/assert.h>

#include "switch.h"

#define port_task_PRIORITY   (configMAX_PRIORITIES - 1)
#define stats_task_PRIORITY   (configMAX_PRIORITIES - 3)

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
	os_assert(xResult == pdPASS, "virtual switch port data task creation failed");
}

static void stats_task(void *pvParameters)
{
	switch_print_stats(pvParameters);
	vTaskDelete(NULL);
}

void create_stats_thread(void *switch_dev)
{
	BaseType_t xResult;

	xResult = xTaskCreate(stats_task, "sw_stats_task",
			configMINIMAL_STACK_SIZE + 100, switch_dev,
			stats_task_PRIORITY, NULL);
	os_assert(xResult == pdPASS, "virtual switch stats task creation failed");
}

