/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_ENET_H_
#define _APP_ENET_H_

#include "fsl_enet.h"
#include "fsl_phyar8031.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef phy_ar8031_resource_t       phy_resource_t;

#ifndef PHY_AUTONEGO_TIMEOUT_COUNT
#define PHY_AUTONEGO_TIMEOUT_COUNT (100000)
#endif

#ifndef PHY_STABILITY_DELAY_US
#define PHY_STABILITY_DELAY_US (0U)
#endif

#define BOARD_ENET              ENET1
#define ENET_IRQ                ENET1_IRQn
#define ENET_PHY_ADDRESS        0x0U
#define ENET_PHY_OPS            phyar8031_ops
#define ENET_CLOCK_FREQ         CLOCK_GetFreq(kCLOCK_EnetIpgClk)

#define ENET_RXBD_NUM (4)
#define ENET_TXBD_NUM (4)

#define ENET_RXBUFF_SIZE (ENET_FRAME_MAX_FRAMELEN)
#define ENET_TXBUFF_SIZE (ENET_FRAME_MAX_FRAMELEN)

#define ENET_PRIORITY (portLOWEST_USABLE_INTERRUPT_PRIORITY - 1)

#ifndef APP_ENET_BUFF_ALIGNMENT
#define APP_ENET_BUFF_ALIGNMENT     ENET_BUFF_ALIGNMENT
#endif

#define STATIC_BUFF_COUNT   (ENET_RXBD_NUM * 3)

#define NONCACHEABLE(var, alignbytes)   AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes)

/* MAC address configuration. */
#ifndef configMAC_ADDR
#define configMAC_ADDR              \
{                       \
    0x02, 0x12, 0x13, 0x10, 0x15, 0x11  \
}
#endif

extern void ENET_DriverIRQHandler(void);
static inline void enet_irq_handler(void *param)
{
    ENET_DriverIRQHandler();
}
#endif /* _APP_ENET_H_ */
