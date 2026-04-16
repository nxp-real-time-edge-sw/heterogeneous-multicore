/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_clock.h"
#include "fsl_power.h"
#include "board.h"

static void clock_config_netc(void)
{
	/* busNetcMixClk 133MHz */
	clk_t busmixClk = {
		.clkId = kCLOCK_Busnetcmix,
		.pclkId = kCLOCK_Syspll1dfs1div2, /* 400 MHz */
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 133333333UL,
	};
	/* enetClk 666 MHz */
	clk_t enetClk = {
		.clkId = kCLOCK_Enet,
		.pclkId = kCLOCK_Syspll1dfs2, /* 666 MHz */
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 666000000UL,
	};
	/* enetRefClk 250MHz */
	clk_t enetrefClk = {
		.clkId = kCLOCK_Enetref,
		.pclkId = kCLOCK_Syspll1dfs0, /* 1 GHz */
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 250000000UL,
	};
	/* enetTimer1Clk 100MHz */
	clk_t enettimer1Clk = {
		.clkId = kCLOCK_Enettimer1,
		.pclkId = kCLOCK_Syspll1dfs0div2, /* 500 MHz */
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 100000000UL,
	};

	/* mac3Clk(enetc0) 250MHz */
	clk_t mac3Clk = {
		.clkId = kCLOCK_Mac3,
		.pclkId = kCLOCK_Syspll1dfs0, /* syspll1 dfs0 = 1 GHz */
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 250000000UL,
	};

	/* mac4Clk(enetc1) 250MHz */
	clk_t mac4Clk = {
		.clkId = kCLOCK_Mac4,
		.pclkId = kCLOCK_Syspll1dfs0, /* syspll1 dfs0 = 1 GHz */
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 250000000UL,
	};

	/* mac5Clk(enetc2) 250MHz */
	clk_t mac5Clk = {
		.clkId = kCLOCK_Mac5,
		.pclkId = kCLOCK_Syspll1dfs0, /* syspll1 dfs0 = 1 GHz */
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 250000000UL,
	};

	pwr_s_t pwrst = {
		.did =  POWER_MIX_SLICE_IDX_NETC,
		.st = SCMI_POWER_DOMAIN_STATE_ON,
	};

	POWER_SetState(&pwrst);
	assert(POWER_GetState(&pwrst) == SCMI_POWER_DOMAIN_STATE_ON);

	CLOCK_SetParent(&busmixClk);
	CLOCK_SetRate(&busmixClk);
	CLOCK_EnableClock(busmixClk.clkId);

	CLOCK_SetParent(&enetClk);
	CLOCK_SetRate(&enetClk);
	CLOCK_EnableClock(enetClk.clkId);

	CLOCK_SetParent(&enetrefClk);
	CLOCK_SetRate(&enetrefClk);
	CLOCK_EnableClock(enetrefClk.clkId);

	CLOCK_SetParent(&enettimer1Clk);
	CLOCK_SetRate(&enettimer1Clk);
	CLOCK_EnableClock(enettimer1Clk.clkId);

	CLOCK_SetParent(&mac3Clk);
	CLOCK_SetRate(&mac3Clk);
	CLOCK_EnableClock(mac3Clk.clkId);

	CLOCK_SetParent(&mac4Clk);
	CLOCK_SetRate(&mac4Clk);
	CLOCK_EnableClock(mac4Clk.clkId);

	CLOCK_SetParent(&mac5Clk);
	CLOCK_SetRate(&mac5Clk);
	CLOCK_EnableClock(mac5Clk.clkId);
}

void board_clock_setup(void)
{
	BOARD_InitClock();
	clock_config_netc();
}
