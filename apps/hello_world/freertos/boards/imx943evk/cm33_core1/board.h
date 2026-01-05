/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "fsl_clock.h"
#include "clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "MIMX943-EVK"

#define BOARD_ENABLE_ELE 1

/*
 * The UART instance to be used for debugging.
 * On iMX943 EVK
 * +------------------------------------------------------------------------------------------------------+
 * |   ca55(core0 ~ core3)   | cm33 core0  | cm33 core1            | cm7 core0         | cm7 core1        |
 * --------------------------------------------------------------------------------------------------------
 * |            1            |      2      |  8                    |  11               | 12               |
 * +------------------------------------------------------------------------------------------------------+
 */
#define BOARD_LPUART8_USE_JTAG_PADS 1
#define BOARD_LPUART8_USE_GPIO_IO12_GPIO_IO13_PADS 0

#define BOARD_DEBUG_UART_TYPE       kSerialPort_Uart
#define BOARD_DEBUG_SWO_TYPE        kSerialPort_Swo

#define BOARD_DEBUG_UART_BAUDRATE   115200U
#define BOARD_DEBUG_SWO_BAUDRATE    4000000U

#if defined(CPU_MIMX94398AVKM_cm33_core1)
/*
 * 0x20800000-0x2089FFFF Shared between AP(ns) and Cortex-M33 Core1(M33S) ← RPMSG area
 *  640 KiB(4 x 32 KB vring + 2 x 512 x 512 bytes buffer) M33S OCRAM(from Cortex-M33 Core1 View)
 * 0x82000000-0x83FFFFFF Shared between AP(ns) and Cortex-M7 Core0 <- RPMSG area - 32M
 * 0x84000000-0x85FFFFFF Shared between AP(ns) and Cortex-M7 Core1 <- RPMSG area - 32M
 */
#define VDEV0_VRING_BASE (0x20800000U)
#define VDEV1_VRING_BASE (0x20810000U)

#define BOARD_DEBUG_CONSOLE_TYPE BOARD_DEBUG_UART_TYPE
#define BOARD_DEBUG_UART_INSTANCE 8
#define BOARD_DEBUG_CONSOLE_PORT BOARD_DEBUG_UART_INSTANCE
#define BOARD_DEBUG_CONSOLE_BAUDRATE BOARD_DEBUG_UART_BAUDRATE

#define SOC_CORE_ICACHE_CTRL M33S_CACHE_CTRLPC
#define SOC_CORE_DCACHE_CTRL M33S_CACHE_CTRLPS
#define SOC_ELE_MU_INST_BASE (ELE_MU7_MUA)

#else
#error "Pls define CPU macro!"
#endif

/*
 * IOMUXC_GPR VALUE
 * core0_did - iomuxc_gpr0[3:0]  - 0xd(13) - cortex-m33 core1
 * core1_did - iomuxc_gpr0[7:4]  - 0x2     - cortex-m33 core0
 * core2_did - iomuxc_gpr0[11:8] - 0x4     - cortex-m7 core0
 * core3_did - iomuxc_gpr0[15:12]- 0xe(14) - cortex-m7 core1
 */
#define BOARD_IOMUXC_GPR_VAL (0xE42D)

/*
 * @brief The Ethernet PHY addresses.
 * ethernet port    phy addr   endpoint/switch port   phy name              enetc/switch
 * ETH0             0x0        switch port 0          DP83849I(PORT A)      switch(enetc3)
 * ETH1             0x1        switch port 1          DP83849I(PORT B)      switch(enetc3)
 * ETH2             0x5        switch port 2          RTL8211FDI-VD-CG      switch(enetc3)
 * ETH2             0x5        endpoint 2             RTL8211FDI-VD-CG      enetc0
 * ETH3             0x6        endpoint 1             RTL8211FDI-VD-CG      enetc1
 * ETH4             0x7        endpoint 0             RTL8211FDI-VD-CG      enetc2
 *
 * Note: ETH2 and ETH3 cannot be used at the same time due to SoC constraint
 */
#define BOARD_EP0_PHY_ADDR       (0x07U)
#define BOARD_EP1_PHY_ADDR       (0x06U)
#define BOARD_EP2_PHY_ADDR       (0x05U)
#define BOARD_SWT_PORT0_PHY_ADDR (0x00U)
#define BOARD_SWT_PORT1_PHY_ADDR (0x01U)
#define BOARD_SWT_PORT2_PHY_ADDR (0x05U)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/
void BOARD_InitDebugConsole(void);

void BOARD_ConfigMPU(void);

void BOARD_InitDebugConsolePins(void);

clock_ip_name_t BOARD_GetUartClkId(uint32_t uartInstIdx);

void BOARD_InitLpuart8Pins(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _BOARD_H_ */
