/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _LWIP_NETC_PORT_H_
#define _LWIP_NETC_PORT_H_

#include "fsl_netc.h"
#include "fsl_netc_soc.h"

#define NETC_PSI                kNETC_ENETC1PSI0
#define NETC_VSI_NUM_USED       (0U)
#define NETC_USE_SWT       	(0U)
#define NETC_MII_MODE           kNETC_RgmiiMode
#define NETC_MII_SPEED          kNETC_MiiSpeed1000M
#define NETC_MSGINTR            MSGINTR2
#define NETC_MSGINTR_IRQ        MSGINTR2_IRQn
#define NETC_FRAME_MAX_FRAMELEN (2000U)
#endif
