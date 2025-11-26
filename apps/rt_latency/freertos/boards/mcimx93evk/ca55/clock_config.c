/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"

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
}
