/*
 * Copyright 2019-2023 NXP
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
#define BOARD_NAME        "MIMX8MP-EVK"
#define MANUFACTURER_NAME "NXP"
#define BOARD_DOMAIN_ID   (0U)

#ifndef APP_DEFINE_UART
/* The UART to use for debug messages. */
#define BOARD_DEBUG_UART_TYPE     kSerialPort_Uart
#define BOARD_DEBUG_UART_BAUDRATE (115200U)

#if !defined(CONFIG_RAM_CONSOLE) && !defined(BOARD_DEBUG_UART_INSTANCE)
#define BOARD_DEBUG_UART_INSTANCE (4U) /* Use UART4 if not specified */
#endif

#if (BOARD_DEBUG_UART_INSTANCE == 2) /* UART2 which is used for uboot and Linux by default */
#define BOARD_DEBUG_UART_BASEADDR UART2_BASE
#define BOARD_DEBUG_UART_CLK_FREQ (CLOCK_GetClockRootFreq(kCLOCK_Uart2ClkRoot))
#define BOARD_UART_IRQ         UART2_IRQn
#define BOARD_UART_IRQ_HANDLER UART2_IRQHandler
#elif (BOARD_DEBUG_UART_INSTANCE == 3) /* UART3 which is in expansion connector */
#define BOARD_DEBUG_UART_BASEADDR UART3_BASE
#define BOARD_DEBUG_UART_CLK_FREQ (CLOCK_GetClockRootFreq(kCLOCK_Uart3ClkRoot))
#define BOARD_UART_IRQ         UART3_IRQn
#define BOARD_UART_IRQ_HANDLER UART3_IRQHandler
#elif (BOARD_DEBUG_UART_INSTANCE == 4) /* UART4 which is used for M-Core by default*/
#define BOARD_DEBUG_UART_BASEADDR UART4_BASE
#define BOARD_DEBUG_UART_CLK_FREQ (CLOCK_GetClockRootFreq(kCLOCK_Uart4ClkRoot))
#define BOARD_UART_IRQ         UART4_IRQn
#define BOARD_UART_IRQ_HANDLER UART4_IRQHandler
#endif

#endif /* APP_DEFINE_UART */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/

void BOARD_InitDebugConsole(void);
void BOARD_InitMemory(void);
void BOARD_RdcInit(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
