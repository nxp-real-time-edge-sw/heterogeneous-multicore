/*
 * Copyright 2025-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include "fsl_clock.h"
#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "clock_config.h"
#include "fsl_iomuxc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "IMX952LPD5-EVK"

/*
 * M7's domain id, please find the domain id that assigned to M7
 * from system manager configuration.
 * From imx-sm/configs/mx952alt.cfg,
 * LM1                 name="Boot", rpc=scmi, boot=2, did=3, default
 *
 * "did=3" means that the domain id is 3.
 */
#ifndef SOC_M7_DOMAIN_ID
#define SOC_M7_DOMAIN_ID 3
#endif

/* for System Manager */

/*! Number of device pins */
#define DEV_SM_NUM_PIN    108U
/*! Number of device daisy */
#define DEV_SM_NUM_DAISY  68U
#define SCMI_IOMUXC_BASE AON__IOMUXC0__IOMUXC_BASE
#define SCMI_IOMUXC_CFGREG_OFF (SCMI_IOMUXC_BASE + (DEV_SM_NUM_PIN * 4U))
#define SCMI_IOMUXC_DAISYREG_OFF (SCMI_IOMUXC_BASE + (DEV_SM_NUM_PIN * 8U))

#define SCMI_LMID_M7 1
#define SCMI_LMID_A55 2
#define SCMI_MU_SEL     8

/* Doorbell interrupt request number */
#define SCMI_DBIR_A2P    0    /* 0: A2P channel */
#define SCMI_DBIR_NOTIFY 1    /* 1: P2A notification channel */
#define SCMI_DBIR_P2A    3    /* 2: FuSa reverse channel 3: P2A channel */
#define SCMI_SMA_ADDR    0    /* share memory address. 0: Use of internal MU SRAM */

#if (SCMI_DBIR_NOTIFY == 1)
#define MU_NOTIFY_GENINT_EN(base)  \
    MU_EnableInterrupts(base, kMU_GenInt1InterruptEnable)
#define MU_NOTIFY_GENINT_FLAG kMU_GenInt1Flag
#endif

#if (SCMI_DBIR_P2A == 3)
#define MU_P2A_GENINT_EN(base)  \
    MU_EnableInterrupts(base, kMU_GenInt3InterruptEnable)
#define MU_P2A_GENINT_FLAG kMU_GenInt1Flag
#endif

/*
 * The UART instance to be used for debugging.
 * On iMX952 Emulator
 * +---------------------------------+
 * |   CM33    |  CM7        | CA55  |
 * -----------------------------------
 * |   1      |   2          | 3     |
 * +---------------------------------+
 *
 * On iMX952 LPDDR5 19x19 EVK
 * +---------------------------------+
 * |   CM33    |  CM7        | CA55  |
 * -----------------------------------
 * |   2      |   3          | 1     |
 * +---------------------------------+
 */

/* boad type: imx952 emulator or imx952 lpddr5 19x19 evk */
#define BOARD_DEBUG_UART_INSTANCE 3

#define BOARD_DEBUG_UART_INSTANCE_FOR_CM7_WITH_SM 3U
#define BOARD_DEBUG_UART_CLOCK_ROOT_FOR_CM7_WITH_SM kCLOCK_lpuart3
#define BOARD_DEBUG_UART_CLK_FREQ_FOR_CM7_WITH_SM   CLOCK_GetRate(BOARD_DEBUG_UART_CLOCK_ROOT_FOR_CM7_WITH_SM)

#define BOARD_DEBUG_UART_INSTANCE_FOR_CM7_WITHOUT_SM 3U
#define BOARD_DEBUG_UART_CLOCK_ROOT_FOR_CM7_WITHOUT_SM kCLOCK_lpuart3
#define BOARD_DEBUG_UART_CLK_FREQ_FOR_CM7_WITHOUT_SM   CLOCK_GetRate(BOARD_DEBUG_UART_CLOCK_ROOT_FOR_CM7_WITHOUT_SM)

#define BOARD_DEBUG_UART_CLOCK_ROOT kCLOCK_lpuart3

#define BOARD_DEBUG_UART_CLK_FREQ CLOCK_GetRate(BOARD_DEBUG_UART_CLOCK_ROOT)
#define BOARD_DEBUG_UART_BAUDRATE   115200U
#define BOARD_DEBUG_UART_TYPE       kSerialPort_Uart

void BOARD_InitDebugConsole(void);

void BOARD_ConfigMPU(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _BOARD_H_ */
