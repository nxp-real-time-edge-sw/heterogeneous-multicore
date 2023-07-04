/*
 * Copyright 2019-2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_rdc.h"
#include "board.h"
#include "mmu.h"
#include "uart.h"
/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Enable Cortex-A53 access (domain 0) */
#define RDC_DISABLE_A53_ACCESS 0x03

/*******************************************************************************
 * Code
 ******************************************************************************/

void clock_config_uart4(void)
{
    CLOCK_DisableClock(kCLOCK_Uart4);
    CLOCK_SetRootMux(kCLOCK_RootUart4, kCLOCK_UartRootmuxOsc24M); /* Set UART source to 24MHZ */
    CLOCK_SetRootDivider(kCLOCK_RootUart4, 1U, 1U); /* Set root clock to 24MHZ/ 1= 24MHZ */
    CLOCK_EnableClock(kCLOCK_Uart4);
}

void pin_mux_uart4(void)
{
    IOMUXC_SetPinMux(IOMUXC_UART4_RXD_UART4_RX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART4_RXD_UART4_RX,
        IOMUXC_SW_PAD_CTL_PAD_DSE(6U) |
        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U));
    IOMUXC_SetPinMux(IOMUXC_UART4_TXD_UART4_TX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART4_TXD_UART4_TX,
        IOMUXC_SW_PAD_CTL_PAD_DSE(6U) |
        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U));
}

/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
    uart_init();
}

/* Initialize MMU, configure memory attributes for each region */
void BOARD_InitMemory(void)
{
    MMU_init();
}

void BOARD_RdcInit(void)
{
    rdc_periph_access_config_t periphConfig;

    RDC_GetDefaultPeriphAccessConfig(&periphConfig);
    periphConfig.policy = RDC_DISABLE_A53_ACCESS;
    periphConfig.periph = kRDC_Periph_UART4;
    RDC_SetPeriphAccessConfig(RDC, &periphConfig);
}
