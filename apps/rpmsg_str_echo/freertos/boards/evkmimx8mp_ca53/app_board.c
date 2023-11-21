/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "app_board.h"

#if (RTOSID == 0)
#define INIT_GICD			1
#else
#define INIT_GICD			0
#endif

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
