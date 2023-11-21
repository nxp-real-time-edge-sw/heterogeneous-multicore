/*
 * Copyright 2019-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_rdc.h"
#include "board.h"
#include "mmu.h"
#include "uart.h"

/* Enable Cortex-A53 access */
#define RDC_ENABLE_A53_ACCESS	RDC_ACCESS_POLICY(BOARD_DOMAIN_ID, kRDC_ReadWrite)

void clock_config_uart2(void)
{
    CLOCK_DisableClock(kCLOCK_Uart2);
    CLOCK_SetRootMux(kCLOCK_RootUart2, kCLOCK_UartRootmuxOsc24M); /* Set UART source to 24MHZ */
    CLOCK_SetRootDivider(kCLOCK_RootUart2, 1U, 1U); /* Set root clock to 24MHZ/ 1= 24MHZ */
    CLOCK_EnableClock(kCLOCK_Uart2);
}

void clock_config_uart3(void)
{
    CLOCK_DisableClock(kCLOCK_Uart3);
    CLOCK_SetRootMux(kCLOCK_RootUart3, kCLOCK_UartRootmuxOsc24M); /* Set UART source to 24MHZ */
    CLOCK_SetRootDivider(kCLOCK_RootUart3, 1U, 1U); /* Set root clock to 24MHZ/ 1= 24MHZ */
    CLOCK_EnableClock(kCLOCK_Uart3);
}

void clock_config_uart4(void)
{
    CLOCK_DisableClock(kCLOCK_Uart4);
    CLOCK_SetRootMux(kCLOCK_RootUart4, kCLOCK_UartRootmuxOsc24M); /* Set UART source to 24MHZ */
    CLOCK_SetRootDivider(kCLOCK_RootUart4, 1U, 1U); /* Set root clock to 24MHZ/ 1= 24MHZ */
    CLOCK_EnableClock(kCLOCK_Uart4);
}

void pin_mux_uart2(void)
{
    IOMUXC_SetPinMux(IOMUXC_UART2_RXD_UART2_RX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART2_RXD_UART2_RX,
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PE_MASK);
    IOMUXC_SetPinMux(IOMUXC_UART2_TXD_UART2_TX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART2_TXD_UART2_TX,
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PE_MASK);
}

void pin_mux_uart3(void)
{
    IOMUXC_SetPinMux(IOMUXC_ECSPI1_MOSI_UART3_TX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ECSPI1_MOSI_UART3_TX,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(6U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PE_MASK);
    IOMUXC_SetPinMux(IOMUXC_ECSPI1_SCLK_UART3_RX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ECSPI1_SCLK_UART3_RX,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(6U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PE_MASK);
}

void pin_mux_uart4(void)
{
    IOMUXC_SetPinMux(IOMUXC_UART4_RXD_UART4_RX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART4_RXD_UART4_RX, 
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PE_MASK);
    IOMUXC_SetPinMux(IOMUXC_UART4_TXD_UART4_TX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART4_TXD_UART4_TX, 
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PE_MASK);
}

/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
#if (BOARD_DEBUG_UART_INSTANCE == 2)
    clock_config_uart2();
    pin_mux_uart2();
#elif (BOARD_DEBUG_UART_INSTANCE == 3)
    clock_config_uart3();
    pin_mux_uart3();
#elif (BOARD_DEBUG_UART_INSTANCE == 4)
    clock_config_uart4();
    pin_mux_uart4();
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
    rdc_periph_access_config_t periphConfig;

    RDC_GetDefaultPeriphAccessConfig(&periphConfig);
    periphConfig.policy = RDC_ENABLE_A53_ACCESS;

#if (BOARD_DEBUG_UART_INSTANCE == 2)
    periphConfig.periph = kRDC_Periph_UART2;
    RDC_SetPeriphAccessConfig(RDC, &periphConfig);
#elif (BOARD_DEBUG_UART_INSTANCE == 4)
    periphConfig.periph = kRDC_Periph_UART4;
    RDC_SetPeriphAccessConfig(RDC, &periphConfig);
#elif (BOARD_DEBUG_UART_INSTANCE == 3)
    periphConfig.periph = kRDC_Periph_UART3;
    RDC_SetPeriphAccessConfig(RDC, &periphConfig);
#endif
}
