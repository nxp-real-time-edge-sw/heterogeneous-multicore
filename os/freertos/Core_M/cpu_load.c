/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "os/stdio.h"
#include "task.h"

#ifdef CONFIG_CPU_LOAD_STATS
static TickType_t get_idle_time(void)
{
	TaskHandle_t xHandle;
	TaskStatus_t xTaskDetails;

	xHandle = xTaskGetIdleTaskHandle();

	vTaskGetInfo(xHandle, &xTaskDetails, pdFALSE, eInvalid);

	return xTaskDetails.ulRunTimeCounter;
}

void cpu_load_stats(void)
{
	static TickType_t last_total, last_idle;
	TickType_t total, idle;
	float cpu_idle;

	idle = get_idle_time();
	total = xTaskGetTickCount();

	cpu_idle = ((idle - last_idle) * 100.) / (float)(total-last_total);

	last_total = total;
	last_idle = idle;

	os_printf("CPU load: %.2f%%\n", 100. - cpu_idle);
}
#endif
