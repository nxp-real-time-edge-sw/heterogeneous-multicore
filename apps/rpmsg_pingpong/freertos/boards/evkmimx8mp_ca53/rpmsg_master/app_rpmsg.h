/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_RPMSG_H_
#define _APP_RPMSG_H_

#include "rpmsg.h"

/* RPMsg device is DMA coherent */
#define RPMSG_IS_COHERENT		(true)

/* RPMsg Link ID and service device driver */
#define RPMSG_LITE_LINK_ID		(RL_PLATFORM_USER_LINK_ID)

#define RPMSG_EPT_DST_ADDR		(30U)

#endif /* _APP_RPMSG_H_ */
