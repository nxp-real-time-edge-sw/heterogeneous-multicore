/*
 * Copyright 2026 NXP
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

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Task priorities. */
#define main_task_PRIORITY	(configMAX_PRIORITIES - 1)
#define TIME_DELAY_SLEEP	(1 * configTICK_RATE_HZ)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

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

int get_test_idx(void)
{
	static int stest_idx = 1;
	return stest_idx++;
}

static void run_test_case(void)
{
#ifdef LPSPI_TEST
	lpspi_test();
#endif

#ifdef FLEXCAN_TEST
	flexcan_test();
#endif

#ifdef LPI2C_TEST
	lpi2c_test();
#endif
}

static void main_task(void *pvParameters)
{
	uint64_t mpid = get_core_mpid();
	int id = get_core_id(mpid);

	run_test_case();

	os_printf("\r\n\r\n%d. ", get_test_idx());
	os_printf("Running %s from %s core%d (MPID: 0x%llx)\r\n", __func__, CPU_CORE_NAME, id, mpid);

	while (1) {
		os_printf(".");
		vTaskDelay(TIME_DELAY_SLEEP);
	}
}

/*!
 * @brief Application entry point.
 */
int main(void)
{
	BOARD_InitHardware();

	PRINTF("\r\n");
	PRINTF("Driver examples for %s on %s\r\n", CPU_CORE_NAME, BOARD_NAME);
	PRINTF("Compiled on %s, at %s\r\n", __DATE__, __TIME__);

	if (xTaskCreate(main_task, "main_task", configMINIMAL_STACK_SIZE + 1000, NULL, main_task_PRIORITY, NULL) == pdPASS)
		vTaskStartScheduler();

	while (1);
}
