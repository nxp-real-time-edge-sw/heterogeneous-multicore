/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "app_board.h"

#include "fsl_debug_console.h"

/* The remote side boot firstly and initialize the GICD */
#define INIT_GICD			1

void board_init(void)
{
	BOARD_InitMemory();
	BOARD_RdcInit();
	GIC_Enable(INIT_GICD);
	board_pins_setup();
	board_clock_setup();
#ifdef CONFIG_RAM_CONSOLE
	RamConsole_Init(RAM_CONSOLE_ADDR, RAM_CONSOLE_SIZE);
#else
	BOARD_InitDebugConsole();
#endif
}
