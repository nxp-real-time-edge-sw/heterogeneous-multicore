/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "hal_clock_platform.h"

static void BOARD_TpmClockSetup(void)
{
	/* clang-format off */
	hal_clk_t hal_clk = {
		.clk_id = hal_clock_tpm2,
		.pclk_id = hal_clock_osc24m,
		.div = 1,
		.enable_clk = true,
		.clk_round_opt = hal_clk_round_auto,
	};
	/* clang-format on */

	HAL_ClockSetRootClk(&hal_clk);

	hal_clk.clk_id = hal_clock_tpm4;
	HAL_ClockSetRootClk(&hal_clk);
}


void board_clock_setup(void)
{
	BOARD_InitClock();
	BOARD_TpmClockSetup();
}
