/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_clock.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "pin_mux.h"
#include "board.h"
#include "mmu.h"
#include "uart.h"
#include "hal_pinctrl.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void BOARD_InitClock(void)
{
}

void pin_mux_lpuart1(void)
{
	HAL_PinctrlSetPinMux(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_UART1_RXD__LPUART1_RX, 0U);
	HAL_PinctrlSetPinMux(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_UART1_TXD__LPUART1_TX, 0U);

	HAL_PinctrlSetPinCfg(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_UART1_RXD__LPUART1_RX,
			HAL_PINCTRL_PLATFORM_IOMUXC_PAD_PD_MASK);
	HAL_PinctrlSetPinCfg(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_UART1_TXD__LPUART1_TX,
			HAL_PINCTRL_PLATFORM_IOMUXC_PAD_DSE(15U));
}

void pin_mux_lpuart3(void)
{
	HAL_PinctrlSetPinMux(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_GPIO_IO15__LPUART3_RX, 0U);
	HAL_PinctrlSetPinMux(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_GPIO_IO14__LPUART3_TX, 0U);

	HAL_PinctrlSetPinCfg(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_GPIO_IO15__LPUART3_RX,
			HAL_PINCTRL_PLATFORM_IOMUXC_PAD_PD_MASK);
	HAL_PinctrlSetPinCfg(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_GPIO_IO14__LPUART3_TX,
			HAL_PINCTRL_PLATFORM_IOMUXC_PAD_DSE(15U));
}

void clock_config_lpuart1(void)
{
	/* clang-format off */
	hal_clk_t hal_clk = {
		.clk_id = hal_clock_lpuart1,
		.pclk_id = hal_clock_osc24m,
		.div = 1,
		.enable_clk = true,
		.clk_round_opt = hal_clk_round_auto,
	};
	/* clang-format on */
	HAL_ClockSetRootClk(&hal_clk);
}

void clock_config_lpuart3(void)
{
	/* clang-format off */
	hal_clk_t hal_clk = {
		.clk_id = hal_clock_lpuart3,
		.pclk_id = hal_clock_osc24m,
		.div = 1,
		.enable_clk = true,
		.clk_round_opt = hal_clk_round_auto,
	};
	/* clang-format on */
	HAL_ClockSetRootClk(&hal_clk);
}

/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
#if (BOARD_DEBUG_UART_INSTANCE == 1)
	pin_mux_lpuart1();
	clock_config_lpuart1();
#elif (BOARD_DEBUG_UART_INSTANCE == 3)
	pin_mux_lpuart3();
	clock_config_lpuart3();
#endif
	uart_init();
}

/* Initialize MMU, configure memory attributes for each region */
void BOARD_InitMemory(void)
{
	MMU_init();
}

void BOARD_RdcInit(void)
{
}
