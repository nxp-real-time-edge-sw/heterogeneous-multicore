/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "task.h"

#include <os/assert.h>

#include "ptp_stack_priv.h"

#define genavb_rx_task_PRIORITY   (configMAX_PRIORITIES - 3)

static void genavb_rx_task(void *pvParameters)
{
	/* dead lock */
	genavb_receive_pkts(pvParameters);
	vTaskDelete(NULL);
}

static void genavb_ptp_ts_task(void *pvParameters)
{
	/* dead lock */
	genavb_retrieve_ptp_ts(pvParameters);
	vTaskDelete(NULL);
}

void create_genavb_vport_thread(void *genavb_dev, bool enable_ptp)
{
	BaseType_t xResult;

	xResult = xTaskCreate(genavb_rx_task, "genavb_vport_task",
			configMINIMAL_STACK_SIZE + 100, genavb_dev,
			genavb_rx_task_PRIORITY, NULL);
	os_assert(xResult == pdPASS, "genavb vport rx task creation failed");

	if (enable_ptp) {
		xResult = xTaskCreate(genavb_ptp_ts_task, "genavb_vport_ptp_task",
				configMINIMAL_STACK_SIZE + 100, genavb_dev,
				genavb_rx_task_PRIORITY, NULL);
		os_assert(xResult == pdPASS, "genavb vport retrieve ptp timestamp task creation failed");
	}
}
