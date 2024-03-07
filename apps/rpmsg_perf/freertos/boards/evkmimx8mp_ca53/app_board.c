/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "app_board.h"

#define INIT_GICD			1

void board_init(void)
{
	BOARD_InitMemory();
	BOARD_RdcInit();
	GIC_Enable(INIT_GICD);
	board_pins_setup();
	board_clock_setup();
	BOARD_InitDebugConsole();
}
