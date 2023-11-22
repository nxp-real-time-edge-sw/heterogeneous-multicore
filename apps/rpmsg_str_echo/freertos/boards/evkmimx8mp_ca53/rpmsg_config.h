/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * Copyright (c) 2015 Xilinx, Inc.
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RPMSG_CONFIG_H_
#define RPMSG_CONFIG_H_

#ifndef RTOSID
#define RTOSID 0
#endif

/*!
 * @addtogroup config
 * @{
 * @file
 */

//! @name Configuration options
//@{

//! @def RL_MS_PER_INTERVAL
//!
//! Delay in milliseconds used in non-blocking API functions for polling.
//! The default value is 1.
#define RL_MS_PER_INTERVAL (1)

//! @def RL_BUFFER_PAYLOAD_SIZE
//!
//! Size of the buffer payload, it must be equal to (240, 496, 1008, ...)
//! [2^n - 16]. Ensure the same value is defined on both sides of rpmsg
//! communication. The default value is 496U.
#define RL_BUFFER_PAYLOAD_SIZE (496U)

//! @def RL_BUFFER_COUNT
//!
//! Number of the buffers, it must be power of two (2, 4, ...).
//! The default value is 2U.
//! Note this value defines the buffer count for one direction of the rpmsg
//! communication only, i.e. if the default value of 2 is used
//! in rpmsg_config.h files for the master and the remote side, 4 buffers
//! in total are created in the shared memory.
#define RL_BUFFER_COUNT (256U)

//! @def RL_API_HAS_ZEROCOPY
//!
//! Zero-copy API functions enabled/disabled.
//! The default value is 1 (enabled).
#define RL_API_HAS_ZEROCOPY (1)

//! @def RL_USE_STATIC_API
//!
//! Static API functions (no dynamic allocation) enabled/disabled.
//! The default value is 0 (static API disabled).
#define RL_USE_STATIC_API (0)

//! @def RL_CLEAR_USED_BUFFERS
//!
//! Clearing used buffers before returning back to the pool of free buffers
//! enabled/disabled.
//! The default value is 0 (disabled).
#define RL_CLEAR_USED_BUFFERS (0)

//! @def RL_USE_MCMGR_IPC_ISR_HANDLER
//!
//! When enabled IPC interrupts are managed by the Multicore Manager (IPC
//! interrupts router), when disabled RPMsg-Lite manages IPC interrupts
//! by itself.
//! The default value is 0 (no MCMGR IPC ISR handler used).
#define RL_USE_MCMGR_IPC_ISR_HANDLER (0)

//! @def RL_USE_ENVIRONMENT_CONTEXT
//!
//! When enabled the environment layer uses its own context.
//! Added for QNX port mainly, but can be used if required.
//! The default value is 0 (no context, saves some RAM).
#define RL_USE_ENVIRONMENT_CONTEXT (0)

//! @def RL_DEBUG_CHECK_BUFFERS
//!
//! Do not use in RPMsg-Lite to Linux configuration
#define RL_DEBUG_CHECK_BUFFERS (0)
//@}

//! @def RL_USE_GEN_SW_MBOX
//!
//! Use generic software mailbox.
#define RL_USE_GEN_SW_MBOX (1)

//! @def VDEV0_VRING_BASE
//!
//! Specify the sharing memory base address used for RPMSG.
#define VDEV0_VRING_BASE (0xfe100000U + 0x10000 * RTOSID)
//@}

//! @def RL_SHMEM_SIZE
//!
//! Specify the sharing memory size used for RPMSG.
#define RL_SHMEM_SIZE (0x10000U)
//@}

//! @def RPMSG_BUF_BASE
//!
//! Specify the sharing memory base address used for RPMSG buffer.
#define RPMSG_BUF_BASE (0xfe200000U + 0x100000 * RTOSID)
//@}

//! @def RPMSG_BUF_SIZE
//!
//! Specify the sharing memory size used for RPMSG buffer.
#define RPMSG_BUF_SIZE (0x100000U)
//@}

#endif /* RPMSG_CONFIG_H_ */
