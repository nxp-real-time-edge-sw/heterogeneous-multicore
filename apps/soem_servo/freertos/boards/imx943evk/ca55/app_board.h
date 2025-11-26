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

#include "fsl_netc_endpoint.h"
#include "fsl_netc_mdio.h"
#include "fsl_netc_phy_wrapper.h"
#include "fsl_phyrtl8211f.h"
#include "fsl_msgintr.h"
#include "fsl_tpm.h"
#include "fsl_power.h"

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
#define BOARD_OUTPUT_COMPARE_CHANNEL    0

/* Define user led gpio instances */
#define BOARD_LED_RGPIO          GPIO2
#define BOARD_LED_RGPIO_PIN      31U

/* Define device counter instances */
#define BOARD_COUNTER_0_BASE    TPM2
#define BOARD_COUNTER_0_IRQ     TPM2_IRQn
/* IRQ priority from 10 to 14,  lower values denote higher priorities */
#define BOARD_COUNTER_0_IRQ_PRIO    10

/* Define netport */
#define SOEM_PORT_NAME "J26"
#define EXAMPLE_EP_NUM        2U
#define EXAMPLE_NETC_HAS_NO_SWITCH 1U
#define EP0_PORT      0U
#define EP1_PORT      1U
#define SOEM_PORT 		EP0_PORT

/* Switch port */
#define EXAMPLE_SWT_MAX_PORT_NUM   3U
#define EXAMPLE_SWT_USED_PORT 0x2U
#define EXAMPLE_SWT_USED_PORT_BITMAP (1U << EXAMPLE_SWT_USED_PORT)
#define EXAMPLE_SWT_PSEUDO_PORT 0x3U
#define EXAMPLE_SWT_PORT0 0x02U
#define EXAMPLE_SWT_PORT1 0x03U
#define EXAMPLE_SWT_PORT2 0x04U

#define EP0_PHY_ADDR       0x6U

#define BOARD_LED_RGPIO          GPIO2
#define BOARD_LED_RGPIO_PIN      31U

#define EP_NUM        3U

#define EP_RING_NUM          3U
#define EP_RXBD_NUM          8U
#define EP_TXBD_NUM          8U
#define EP_BD_ALIGN          128U
#define EP_BUFF_SIZE_ALIGN   64U
#define EP_RXBUFF_SIZE       1518U
#define EP_TXBUFF_SIZE       1518U
#define EP_RXBUFF_SIZE_ALIGN SDK_SIZEALIGN(EP_RXBUFF_SIZE, EP_BUFF_SIZE_ALIGN)
#define EP_TXBUFF_SIZE_ALIGN SDK_SIZEALIGN(EP_TXBUFF_SIZE, EP_BUFF_SIZE_ALIGN)

#define EP_TXFRAME_NUM 20U
#define PHY_PAGE_SELECT_REG 0x1FU /*!< The PHY page select register. */
#define TX_INTR_MSG_DATA  1U
#define RX_INTR_MSG_DATA  2U
#define TX_MSIX_ENTRY_IDX 0U
#define RX_MSIX_ENTRY_IDX 1U

#ifndef PHY_STABILITY_DELAY_US
#define PHY_STABILITY_DELAY_US (500000U)
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
void BOARD_InitHardware(void);
int if_port_init(void);
/*${prototype:end}*/

#endif /* _APP_BOARD_H_ */
