/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"

void BOARD_InitHardware(void)
{
	/* Init board cpu and hardware. */
	BOARD_InitMemory();
	BOARD_RdcInit();
	/* Enable GIC before register any interrupt handler*/
	GIC_Enable();
	board_pins_setup();
	board_clock_setup();
	BOARD_InitDebugConsole();
}

uint64_t get_core_mpid(void)
{
	return (__get_MPIDR_EL1() & MPIDR_AFFINITY_MASK);
}
