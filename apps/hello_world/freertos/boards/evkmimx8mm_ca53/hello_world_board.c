/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "rtos_memory.h"

void hello_world_board_init(void)
{
	/* Init board cpu and hardware. */
	BOARD_InitMemory();
	BOARD_RdcInit();
	/* Enable GIC before register any interrupt handler*/
	GIC_Enable();
	board_pins_setup();
	board_clock_setup();
#ifdef CONFIG_RAM_CONSOLE
	RamConsole_Init(RAM_CONSOLE_ADDR, RAM_CONSOLE_SIZE);
#else
	BOARD_InitDebugConsole();
#endif
}

void print_ram_console_addr(void)
{
#if (RTOSID == 0)
		int i;

		for (i = 1; i < RTOS_CNT; i++)
			PRINTF("RTOS%d: RAM console@0x%x\r\n", i, RAM_CONSOLE_ADDR + RTOS_MEM_LEN * i);
#endif
}
