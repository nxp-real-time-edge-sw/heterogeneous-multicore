/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

static void BOARD_InitClock(void)
{
	g_clockSourceFreq[kCLOCK_Osc24M]          = 24000000U;
	g_clockSourceFreq[kCLOCK_SysPll1]         = 4000000000U;
	g_clockSourceFreq[kCLOCK_SysPll1Pfd0]     = 1000000000U;
	g_clockSourceFreq[kCLOCK_SysPll1Pfd0Div2] = 500000000U;
	g_clockSourceFreq[kCLOCK_SysPll1Pfd1]     = 800000000U;
	g_clockSourceFreq[kCLOCK_SysPll1Pfd1Div2] = 400000000U;
	g_clockSourceFreq[kCLOCK_SysPll1Pfd2]     = 625000000U;
	g_clockSourceFreq[kCLOCK_SysPll1Pfd2Div2] = 312500000U;
	g_clockSourceFreq[kCLOCK_AudioPll1Out]    = 393216000U;
	g_clockSourceFreq[kCLOCK_AudioPll1]       = 393216000U;
}

static void clock_config_lpuart2(void)
{
	const clock_root_config_t uartClkCfg = {
		.clockOff = false,
		.mux = 0, // 24MHz oscillator source
		.div = 1
	};

	CLOCK_SetRootClock(kCLOCK_Root_Lpuart2, &uartClkCfg);
}

void board_clock_setup(void)
{
	BOARD_InitClock();

	clock_config_lpuart2();
}
