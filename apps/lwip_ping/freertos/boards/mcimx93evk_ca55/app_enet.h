/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_ENET_H_
#define _APP_ENET_H_

#include "fsl_enet.h"
#include "fsl_phyrtl8211f.h"

typedef phy_rtl8211f_resource_t		phy_resource_t;

#define ENET_IRQ			ENET_IRQn
#define ENET_PHY_ADDRESS		0x2U
#define ENET_PHY_OPS			phyrtl8211f_ops
#define ENET_CLOCK_FREQ			(CLOCK_GetIpFreq(kCLOCK_Root_WakeupAxi))

/* MAC address configuration. */
#ifndef configMAC_ADDR
#define configMAC_ADDR				\
{						\
	0x02, 0x12, 0x13, 0x10, 0x15, 0x11	\
}
#endif

extern void ENET_DriverIRQHandler(void);
static inline void enet_irq_handler(void *param)
{
	ENET_DriverIRQHandler();
}

#endif /* _APP_ENET_H_ */
