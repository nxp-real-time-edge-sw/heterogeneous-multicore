/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "app_board.h"

void srtm_init(void)
{
	APP_SRTM_Init();
	APP_SRTM_StartCommunication();
}

void board_init(void)
{
	const clock_root_config_t uartClkCfg = {
		.clockOff = false,
		.mux = 0, // 24MHz oscillator source
		.div = 1
	};

	BOARD_InitBootPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();

	/* Setup clock for lpuart5 */
	CLOCK_SetRootClock(kCLOCK_Root_Lpuart5, &uartClkCfg);
	CLOCK_EnableClock(kCLOCK_Root_Lpuart5);
}
