/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "clock_config.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* clang-format off */
/*
 * AUDIOPLL1/AUDIOPLL1OUT
 *
 * VCO = (24MHz / rdiv) * (mfi + (mfn / mfd))  = 3,932,160,000 Hz
 * Output = VCO / odiv = 393.216 MHz
 */
const fracn_pll_init_t g_audioPllCfg = {
    .rdiv = 1,
    .mfi = 163,
    .mfn = 84,
    .mfd = 100,
    .odiv = 10
};
/* clang-format on */

/*******************************************************************************
 * Code
 ******************************************************************************/

void BOARD_BootClockRUN(void)
{
    /* ROM has already initialized PLL */
    CLOCK_PllInit(AUDIOPLL, &g_audioPllCfg);
}
