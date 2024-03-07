/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "os/stdio.h"
#include "FreeRTOS.h"
#include "task.h"

#include "app_board.h"
#include "app_srtm.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern volatile app_srtm_state_t srtmState;

/*******************************************************************************
 * Code
 ******************************************************************************/
__WEAK void gpc_config(void)
{
}

void MainTask(void *pvParameters)
{
	/*
	 * Wait For A core Side Become Ready
	 */
	os_printf("********************************\r\n");
	os_printf(" Wait the Linux kernel boot up to create the link between M core and A core.\r\n");
	os_printf("\r\n");
	os_printf("********************************\r\n");
	while (srtmState != APP_SRTM_StateLinkedUp)
		;
	os_printf("The rpmsg channel between M core and A core created!\r\n");
	os_printf("********************************\r\n");
	os_printf("\r\n");

	gpc_config();

	while (true) {
		/* Use App task logic to replace vTaskDelay */
		os_printf("\r\nTask %s is working now.\r\n", (char *)pvParameters);
		vTaskDelay(portMAX_DELAY);
	}
}
/*!
 * @brief Main function
 */
int main(void)
{
	char *taskID = "A";

	board_init();

	os_printf("\r\n####################  RPMSG UART SHARING DEMO  ####################\n\r\n");
	os_printf("    Build Time: %s--%s \r\n", __DATE__, __TIME__);

	srtm_init();

	if (xTaskCreate(MainTask, "Main Task", 256U, (void *)taskID, tskIDLE_PRIORITY + 1U, NULL) != pdPASS) {
		os_printf("Task creation failed!.\r\n");
		while (1)
			;
	}

	/* Start FreeRTOS scheduler. */
	vTaskStartScheduler();

	/* Application should never reach this point. */
	for (;;) {
	}
}
void vApplicationMallocFailedHook(void)
{
	os_printf("Malloc Failed!!!\r\n");
}
