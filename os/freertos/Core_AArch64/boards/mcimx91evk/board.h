/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#if __has_include("app_board.h")
#include "app_board.h"
#endif
#include "fsl_clock.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME        "MIMX91-EVK"
#define MANUFACTURER_NAME "NXP"
#define BOARD_DOMAIN_ID   (0U)

/* The UART to use for debug messages. */
#define BOARD_DEBUG_UART_TYPE     kSerialPort_Uart
#define BOARD_DEBUG_UART_BAUDRATE (115200U)

#if !defined(CONFIG_RAM_CONSOLE) && !defined(BOARD_DEBUG_UART_INSTANCE)
#define BOARD_DEBUG_UART_INSTANCE (1U) /* Use LPUART1 if not specified */
#endif

#if (BOARD_DEBUG_UART_INSTANCE == 1) /* LPUART1 which is used for uboot and Linux by default */
#define BOARD_DEBUG_UART_BASEADDR LPUART1_BASE
#define BOARD_DEBUG_UART_CLK_FREQ (CLOCK_GetIpFreq(kCLOCK_Root_Lpuart1))
#define BOARD_UART_IRQ            LPUART1_IRQn
#define BOARD_UART_IRQ_HANDLER    LPUART1_IRQHandler
#elif (BOARD_DEBUG_UART_INSTANCE == 2) /* LPUART2 which is used for M-Core by default*/
#define BOARD_DEBUG_UART_BASEADDR LPUART2_BASE
#define BOARD_DEBUG_UART_CLK_FREQ (CLOCK_GetIpFreq(kCLOCK_Root_Lpuart2))
#define BOARD_UART_IRQ            LPUART2_IRQn
#define BOARD_UART_IRQ_HANDLER    LPUART2_IRQHandler
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/

void BOARD_InitClock(void);
void BOARD_InitDebugConsole(void);
void BOARD_InitMemory(void);
void BOARD_RdcInit(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
