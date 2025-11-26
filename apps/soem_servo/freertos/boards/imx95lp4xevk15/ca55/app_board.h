/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_BOARD_H_
#define _APP_BOARD_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "fsl_lpi2c.h"
#include "fsl_netc_endpoint.h"
#include "fsl_netc_mdio.h"
#include "fsl_netc_phy_wrapper.h"
#include "fsl_phyrtl8211f.h"
#include "fsl_msgintr.h"
#include "fsl_tpm.h"
#include "fsl_pcal6524.h"

#include "sm_platform.h"

#include "hal_clock.h"
#include "hal_power.h"
#include "hal_pinctrl.h"

#include "ethercattype.h"
#include "nicdrv.h"
#include "ethercatbase.h"
#include "ethercatmain.h"
#include "ethercatdc.h"
#include "ethercatcoe.h"
#include "ethercatfoe.h"
#include "ethercatconfig.h"
#include "ethercatprint.h"
#include "netc_ep/soem_netc_ep.h"
#include "netc_ep/netc_ep.h"
#include "soem_port.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SOEM_PORT_NAME "netc0"

#define BOARD_OUTPUT_COMPARE_CHANNEL    0

#define BOARD_COUNTER_0_BASE    TPM4
#define BOARD_COUNTER_0_IRQ     TPM4_IRQn
#define BOARD_COUNTER_0_IRQ_PRIO 10

#ifndef PHY_AUTONEGO_TIMEOUT_COUNT
#define PHY_AUTONEGO_TIMEOUT_COUNT (100000)
#endif

#ifndef PHY_STABILITY_DELAY_US
#define PHY_STABILITY_DELAY_US (0U)
#endif

#define EP0_PHY_ADDR       0x01U

#define EP_RING_NUM       3U
#define EP_RXBD_NUM       8U
#define EP_TXBD_NUM       8U
#define EP_BD_ALIGN       128U
#define EP_BUFF_SIZE_ALIGN   64U
#define EP_RXBUFF_SIZE     1518U
#define EP_TXBUFF_SIZE     1518U
#define EP_RXBUFF_SIZE_ALIGN SDK_SIZEALIGN(EP_RXBUFF_SIZE, EP_BUFF_SIZE_ALIGN)

#define EP_TXFRAME_NUM 20U
#define TX_INTR_MSG_DATA  1U
#define RX_INTR_MSG_DATA  2U
#define TX_MSIX_ENTRY_IDX 0U
#define RX_MSIX_ENTRY_IDX 1U

#define NONCACHEABLE(var, alignbytes)   AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
void BOARD_InitHardware(void);
int if_port_init(void);
/*${prototype:end}*/

#endif /* _APP_BOARD_H_ */
