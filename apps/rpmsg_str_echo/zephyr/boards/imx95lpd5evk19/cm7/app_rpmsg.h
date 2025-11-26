/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_RPMSG_H_
#define _APP_RPMSG_H_

/* RPMsg device is DMA coherent */
#define RPMSG_IS_COHERENT		(false)

#define RPMSG_NS_ANNOUNCE_STRING	"rpmsg-virtual-tty-channel-1"

/* RPMsg Endpoint number and addresses */
#define RPMSG_EPT_NUM			(3)

#define RPMSG_EPT_ADDR			{0, 1, 2}

/* Print out the received data from master */
#define RPMSG_PRINT_RECV		(1)
/* Echo back the received data to master */
#define RPMSG_ECHO_BACK			(1)

#endif /* _APP_RPMSG_H_ */
