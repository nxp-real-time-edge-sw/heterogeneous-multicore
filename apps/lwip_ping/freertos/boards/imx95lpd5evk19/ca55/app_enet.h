/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_ENET_H_
#define _APP_ENET_H_

#include "hal_clock.h"
#include "fsl_phyaqr113c.h"
#include "fsl_phyrtl8211f.h"
#include "lwip_netc_port.h"

#define ENET_PHY_ADDRESS  ((NETC_PSI == kNETC_ENETC0PSI0) ? 0x1U : 0x8U)
#define ENET_PHY_OPS      ((NETC_PSI == kNETC_ENETC0PSI0) ? \
			   &g_app_phy_rtl8211f_ops : &g_app_phy_aqr113c_ops)
#define ENET_PHY_RESOURCE ((NETC_PSI == kNETC_ENETC0PSI0) ? \
			   (void *)&g_phy_rtl8211f_resource : (void *)&g_phy_aqr113c_resource)
#define ENET_CLOCK_FREQ   HAL_ClockGetIpFreq(hal_clock_enet)

extern phy_aqr113c_resource_t g_phy_aqr113c_resource;
extern const phy_operations_t g_app_phy_aqr113c_ops;
extern phy_rtl8211f_resource_t g_phy_rtl8211f_resource;
extern const phy_operations_t g_app_phy_rtl8211f_ops;

/* MAC address configuration. */
#ifndef configMAC_ADDR
#define configMAC_ADDR				\
{						\
	0x02, 0x12, 0x13, 0x10, 0x15, 0x11	\
}
#endif

#endif /* _APP_ENET_H_ */
