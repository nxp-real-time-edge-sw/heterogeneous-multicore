/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_BOARD_H_
#define _APP_BOARD_H_

#include "fsl_enet.h"
#include "fsl_phyrtl8211f.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define BOARD_PHY0_ADDRESS      (0x02U)           /* Phy address of enet port 0. */
#define BOARD_PHY0_OPS          &phyrtl8211f_ops    /* PHY operations. */

#define BOARD_NET_PORT0_MII_MODE    kENET_RgmiiMode
#define BOARD_ENET_CLOCK_FREQ       (CLOCK_GetIpFreq(kCLOCK_Root_WakeupAxi))
#define BOARD_DEBUG_UART_INSTANCE 2
#define BOARD_OUTPUT_COMPARE_CHANNEL    0

#define BOARD_COUNTER_0_BASE    TPM2
#define BOARD_COUNTER_0_IRQ     TPM2_IRQn
#define BOARD_COUNTER_0_IRQ_PRIO 10

#endif /* _APP_BOARD_H_ */
