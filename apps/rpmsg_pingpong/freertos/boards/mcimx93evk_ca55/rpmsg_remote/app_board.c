/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "app_board.h"

#include "fsl_debug_console.h"

void board_init(void)
{
	BOARD_InitMemory();
	BOARD_RdcInit();
	GIC_Enable();
	board_pins_setup();
	board_clock_setup();
	BOARD_InitDebugConsole();
}
