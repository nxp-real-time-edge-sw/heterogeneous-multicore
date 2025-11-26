/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "os/irq.h"
#include "app_board.h"
#include "fsl_debug_console.h"
#include "sm_platform.h"

void board_init(void)
{
	IRQn_Type const s_muIrqs[] = MU_IRQS;
	IRQn_Type irq = s_muIrqs[SM_PLATFORM_MU_INST];

	/* Init board cpu and hardware. */
	BOARD_InitMemory();
	/* Enable GIC before register any interrupt handler*/
	GIC_Enable();
	os_irq_register(irq, SM_platform_MU_IRQHandler, NULL, portLOWEST_USABLE_INTERRUPT_PRIORITY - 1);
	SM_Platform_Init();
	board_pins_setup();
	board_clock_setup();
	BOARD_InitDebugConsole();
}
