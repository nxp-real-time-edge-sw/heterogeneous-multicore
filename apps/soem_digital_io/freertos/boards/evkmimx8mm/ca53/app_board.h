/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_BOARD_H_
#define _APP_BOARD_H_

#include "fsl_enet.h"
#include "fsl_phyar8031.h"
#include "fsl_gpt.h"

#include "app_enet.h"

#include "ethercattype.h"
#include "nicdrv.h"
#include "ethercatbase.h"
#include "ethercatmain.h"
#include "ethercatdc.h"
#include "ethercatcoe.h"
#include "ethercatfoe.h"
#include "ethercatconfig.h"
#include "ethercatprint.h"
#include "enet/soem_enet.h"
#include "enet/enet.h"
#include "soem_port.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SOEM_PORT_NAME "enet0"

#define BOARD_PHY0_ADDRESS      (0x00U)           /* Phy address of enet port 0. */
#define BOARD_PHY0_OPS          phyar8031_ops    /* PHY operations. */

#define BOARD_NET_PORT0_MII_MODE    kENET_RgmiiMode
#define BOARD_ENET_CLOCK_FREQ       (CLOCK_GetIpFreq(kCLOCK_EnetIpgClk))
#define BOARD_DEBUG_UART_INSTANCE 4
#define BOARD_OUTPUT_COMPARE_CHANNEL    kGPT_OutputCompare_Channel1

#define BOARD_COUNTER_0_BASE    GPT1
#define BOARD_COUNTER_0_IRQ     GPT1_IRQn
#define BOARD_COUNTER_0_IRQ_PRIO 10

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
void BOARD_InitHardware(void);
int if_port_init(void);
/*${prototype:end}*/

#endif /* _APP_BOARD_H_ */
