/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_BOARD_H_
#define _APP_BOARD_H_

#include "os/stdio.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_clock.h"
#include "rtos_memory.h"

#define CPU_CORE_NAME		"Cortex-A53"
#define RTOS_CNT	(2)

#ifdef APP_DEFINE_UART
/* The UART to use for debug messages. */
#define BOARD_DEBUG_UART_TYPE     kSerialPort_Uart
#define BOARD_DEBUG_UART_BAUDRATE (115200U)
#define BOARD_DEBUG_UART_BASEADDR UART3_BASE
#define BOARD_DEBUG_UART_INSTANCE (3U)
#define BOARD_DEBUG_UART_CLK_FREQ (CLOCK_GetClockRootFreq(kCLOCK_Uart3ClkRoot))
#define BOARD_UART_IRQ         UART3_IRQn
#define BOARD_UART_IRQ_HANDLER UART3_IRQHandler
#endif

void board_init(void);

#endif /* _APP_BOARD_H_ */
