/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "app.h"
#include "board.h"

#include "fsl_clock.h"
#include "fsl_power.h"

#ifdef LPSPI_TEST
#include "lpspi.h"
#endif

#ifdef FLEXCAN_TEST
#include "flexcan.h"
#endif

#ifdef LPI2C_TEST
#include "lpi2c.h"
#endif

#ifdef LPSPI_TEST
static void BOARD_LpSpiClockSetup(void)
{
	clk_t lpspiMstClkCfg = {
		.clkId = LPSPI_MASTER_CLK_ID,
		.pclkId = kCLOCK_Osc24m,
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 24000000UL,
	};

	clk_t lpspiSlvClkCfg = {
		.clkId = LPSPI_SLAVE_CLK_ID,
		.pclkId = kCLOCK_Osc24m,
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 24000000UL,
	};

	CLOCK_SetRate(&lpspiMstClkCfg);
	CLOCK_EnableClock(lpspiMstClkCfg.clkId);

	CLOCK_SetRate(&lpspiSlvClkCfg);
	CLOCK_EnableClock(lpspiSlvClkCfg.clkId);
}
#endif

#ifdef FLEXCAN_TEST
static void BOARD_FlexCanClockSetup(void)
{
	clk_t flexcanClkCfg1 = {
		.clkId = FLEXCAN_CLOCK_ROOT1,
		.pclkId = kCLOCK_Osc24m,
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 24000000UL,
	};

	CLOCK_SetParent(&flexcanClkCfg1);
	CLOCK_SetRate(&flexcanClkCfg1);
	CLOCK_EnableClock(flexcanClkCfg1.clkId);

	clk_t flexcanClkCfg2 = {
		.clkId = FLEXCAN_CLOCK_ROOT2,
		.pclkId = kCLOCK_Osc24m,
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 24000000UL,
	};

	CLOCK_SetParent(&flexcanClkCfg2);
	CLOCK_SetRate(&flexcanClkCfg2);
	CLOCK_EnableClock(flexcanClkCfg2.clkId);
}
#endif

#ifdef LPI2C_TEST
static void BOARD_LpI2cClockSetup(void)
{
	clk_t hal_lpi2cMstclk = {
		.clkId = LPI2C_MASTER_CLOCK_ROOT,
		.pclkId = kCLOCK_Osc24m,
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 24000000UL,
	};

	clk_t hal_lpi2cSlvclk = {
		.clkId = LPI2C_SLAVE_CLOCK_ROOT,
		.pclkId = kCLOCK_Osc24m,
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 24000000UL,
	};

	CLOCK_SetRate(&hal_lpi2cMstclk);
	CLOCK_EnableClock(hal_lpi2cMstclk.clkId);

	CLOCK_SetRate(&hal_lpi2cSlvclk);
	CLOCK_EnableClock(hal_lpi2cSlvclk.clkId);
}
#endif

static void BOARD_TpmClockSetup(void)
{
	/* clang-format off */
	clk_t lptpmclk = {
		.clkId = kCLOCK_Tpm2,
		.pclkId = kCLOCK_Osc24m,
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 24000000UL,
	};

	CLOCK_SetParent(&lptpmclk);
	CLOCK_SetRate(&lptpmclk);
	CLOCK_EnableClock(lptpmclk.clkId);
}

void board_clock_setup(void)
{
	BOARD_InitClock();
	BOARD_TpmClockSetup();

#ifdef LPSPI_TEST
	BOARD_LpSpiClockSetup();
#endif

#ifdef FLEXCAN_TEST
	BOARD_FlexCanClockSetup();
#endif

#ifdef LPI2C_TEST
	BOARD_LpI2cClockSetup();
#endif
}
