/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_clock.h"
//#include "clock_init.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

static void clock_config_enet(void)
{
	/* enetClk 250MHz */
	const clock_root_config_t enetClkCfg = {
		.clockOff = false,
		.mux = kCLOCK_WAKEUPAXI_ClockRoot_MuxSysPll1Pfd0, // 1000MHz
		.div = 4
	};

	/* enetRefClk 250MHz (For 125MHz TX_CLK ) */
	const clock_root_config_t enetRefClkCfg = {
		.clockOff = false,
		.mux = kCLOCK_ENETREF_ClockRoot_MuxSysPll1Pfd0Div2, // 500MHz
		.div = 2
	};

	CLOCK_SetRootClock(kCLOCK_Root_WakeupAxi, &enetClkCfg);
	CLOCK_SetRootClock(kCLOCK_Root_EnetRef, &enetRefClkCfg);
}

static void BOARD_TpmClockSetup(void)
{
    const clock_root_config_t tpmClkCfg = {
        .clockOff = false,
        .mux = 0, /* 24MHz oscillator source */
        .div = 1
    };

    CLOCK_SetRootClock(kCLOCK_Root_Tpm2, &tpmClkCfg);
    CLOCK_SetRootClock(kCLOCK_Root_Tpm4, &tpmClkCfg);
}

void board_clock_setup(void)
{
	BOARD_InitClock();
	BOARD_TpmClockSetup();
	clock_config_enet();
}
