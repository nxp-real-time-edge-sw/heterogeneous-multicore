/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_clock.h"
#include "board.h"

static void BOARD_TpmClockSetup(void)
{
	/* clang-format off */
	clk_t clk = {
		.clkId = kCLOCK_Tpm2,
		.pclkId = kCLOCK_Osc24m,
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 24000000UL,
	};
	/* clang-format on */

	CLOCK_SetParent(&clk);
	CLOCK_SetRate(&clk);
	CLOCK_EnableClock(clk.clkId);

	clk.clkId = kCLOCK_Tpm4;
	CLOCK_SetRate(&clk);
	CLOCK_EnableClock(clk.clkId);
}

void board_clock_setup(void)
{
	BOARD_InitClock();
	BOARD_TpmClockSetup();
}
