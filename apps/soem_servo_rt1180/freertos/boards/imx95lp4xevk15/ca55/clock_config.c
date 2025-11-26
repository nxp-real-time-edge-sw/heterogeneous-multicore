/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "hal_clock_platform.h"

static void BOARD_NetcClockSetup(void)
{
    /* clang-format off */
    /* enetClk 666.66MHz */
    hal_clk_t hal_enetclk = {
        .clk_id = hal_clock_enet,
        .pclk_id = hal_clock_syspll1dfs2,
        .div = 1,
        .enable_clk = true,
        .clk_round_opt = hal_clk_round_auto,
    };
    /* enetRefClk 250MHz */
    hal_clk_t hal_enetrefclk = {
        .clk_id = hal_clock_enetref,
        .pclk_id = hal_clock_syspll1dfs0,
        .div = 4,
        .enable_clk = true,
        .clk_round_opt = hal_clk_round_auto,
    };
    /* enetTimer1Clk 100MHz */
    hal_clk_t hal_enettimer1clk = {
        .clk_id = hal_clock_enettimer1,
        .pclk_id = hal_clock_syspll1dfs0div2,
        .div = 5,
        .enable_clk = true,
        .clk_round_opt = hal_clk_round_auto,
    };
    /* NETCMIX power up */
    hal_pwr_s_t pwrst = {
        .did = HAL_POWER_PLATFORM_MIX_SLICE_IDX_NETC,
        .st = hal_power_state_on,
    };
    /* lpi2c2Clk 24MHz */
    hal_clk_t hal_lpi2c2ClkCfg = {
        .clk_id = hal_clock_lpi2c2,
        .pclk_id = hal_clock_osc24m,
        .div = 1,
        .enable_clk = true,
        .clk_round_opt = hal_clk_round_auto,
    };
    /* clang-format on */

    /* Power up NETCMIX */
    HAL_PowerSetState(&pwrst);
    while(HAL_PowerGetState(&pwrst))
    {
    }

    HAL_ClockSetRootClk(&hal_enetclk);
    HAL_ClockSetRootClk(&hal_enetrefclk);
    HAL_ClockSetRootClk(&hal_enettimer1clk);
    HAL_ClockSetRootClk(&hal_lpi2c2ClkCfg);
}

static void BOARD_TpmClockSetup(void)
{
    /* clang-format off */
    hal_clk_t hal_lptpmclk = {
        .clk_id = hal_clock_tpm4,
        .pclk_id = hal_clock_osc24m,
        .div = 1,
        .enable_clk = true,
        .clk_round_opt = hal_clk_round_auto,
    };


    HAL_ClockSetRootClk(&hal_lptpmclk); 
}
void board_clock_setup(void)
{
    BOARD_InitClock();
    BOARD_NetcClockSetup();
    BOARD_TpmClockSetup();
}
