/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_ENET_H_
#define _APP_ENET_H_

#include "hal_clock.h"
#include "fsl_phyrtl8211f.h"
#include "lwip_netc_port.h"
#include "fsl_netc_mdio.h"

extern phy_rtl8211f_resource_t *get_netc_phy_res(void);
extern phy_operations_t *get_netc_phy_ops(void);

#define ENET_PHY_ADDRESS	((NETC_PSI == kNETC_ENETC0PSI0) ? 0x5U : \
				((NETC_PSI == kNETC_ENETC1PSI0) ? 0x6U : 0x7U))

#define ENET_PHY_OPS			 get_netc_phy_ops()
#define ENET_PHY_RESOURCE		 get_netc_phy_res()
#define ENET_CLOCK_FREQ			 HAL_ClockGetIpFreq(hal_clock_enet)

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
