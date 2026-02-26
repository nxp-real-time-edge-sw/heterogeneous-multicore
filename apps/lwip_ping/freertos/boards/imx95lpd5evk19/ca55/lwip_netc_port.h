/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _LWIP_NETC_PORT_H_
#define _LWIP_NETC_PORT_H_

#include "fsl_netc.h"
#include "fsl_netc_soc.h"

/*
 * Example supports two ports to select
 *
 * ENETC0
 * - NETC_PSI:       kNETC_ENETC0PSI0
 * - NETC_MII_MODE:  kNETC_RgmiiMode
 * - NETC_MII_SPEED: kNETC_MiiSpeed1000M
 *
 * ENETC2
 * - NETC_PSI:       kNETC_ENETC2PSI0
 * - NETC_MII_MODE:  kNETC_XgmiiMode
 * - NETC_MII_SPEED: kNETC_MiiSpeed1000M
 */

#if !defined(LWIP_PING_USE_ENETC) || (LWIP_PING_USE_ENETC != 2)
/* ENETC0 */
 #define NETC_PSI         kNETC_ENETC0PSI0
 #define NETC_MII_MODE    kNETC_RgmiiMode
#else
/* ENETC2 */
#define NETC_PSI         kNETC_ENETC2PSI0
#define NETC_MII_MODE    kNETC_XgmiiMode
#endif

#define NETC_MII_SPEED   kNETC_MiiSpeed1000M
#define NETC_MSGINTR     MSGINTR2
#define NETC_MSGINTR_IRQ MSGINTR2_IRQn

#endif

