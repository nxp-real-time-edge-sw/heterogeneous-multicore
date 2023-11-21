/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_RPMSG_H_
#define _APP_RPMSG_H_

#include "rpmsg.h"

/* RPMsg device is DMA coherent */
#define RPMSG_IS_COHERENT		(false)

/* RPMsg Link ID and service device driver */
#define RPMSG_LITE_LINK_ID		(RL_PLATFORM_IMX8MP_M7_USER_LINK_ID)
#define RPMSG_LITE_NS_ANNOUNCE_STRING	"rpmsg-perf"

#endif /* _APP_RPMSG_H_ */
