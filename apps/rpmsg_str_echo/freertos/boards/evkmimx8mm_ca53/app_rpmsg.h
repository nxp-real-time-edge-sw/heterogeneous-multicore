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
#define RPMSG_LITE_NS_ANNOUNCE_STRING	"rpmsg-virtual-tty-channel-1"

/* RPMsg Endpoint number and addresses */
#define RPMSG_EPT_NUM			(3)

/* The first 3 endpoint addresses are assigned to FreeRTOS on M4 */
#if (RTOSID == 0)
#define RPMSG_EPT_ADDR			{3, 4, 5}
#elif (RTOSID == 1)
#define RPMSG_EPT_ADDR			{6, 7, 8}
#elif (RTOSID == 2)
#define RPMSG_EPT_ADDR			{9, 10, 11}
#elif (RTOSID == 3)
#define RPMSG_EPT_ADDR			{12, 13, 14}
#endif

/* Print out the received data from master */
#define RPMSG_PRINT_RECV		(1)
/* Echo back the received data to master */
#define RPMSG_ECHO_BACK			(1)

#endif /* _APP_RPMSG_H_ */
