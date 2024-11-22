/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "os/stdio.h"
#include "os/irq.h"
#include "FreeRTOS.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "rtos_memory.h"
#include "sm_platform.h"

void hello_world_board_init(void)
{
	IRQn_Type const s_muIrqs[] = MU_IRQS;
	IRQn_Type irq = s_muIrqs[SM_PLATFORM_MU_INST];

	/* Init board cpu and hardware. */
	BOARD_InitMemory();
	/* Enable GIC before register any interrupt handler*/
	GIC_Enable();
	SM_Platform_Init();
	os_irq_register(irq, SM_platform_MU_IRQHandler, NULL, portLOWEST_USABLE_INTERRUPT_PRIORITY - 1);
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
		os_printf("RTOS%d: RAM console@0x%x\r\n", i, RAM_CONSOLE_ADDR + RTOS_MEM_LEN * i);
#endif
}
