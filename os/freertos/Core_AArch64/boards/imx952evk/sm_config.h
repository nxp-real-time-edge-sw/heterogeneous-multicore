/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SM_CONFIG_H_
#define _SM_CONFIG_H_

#if RTOSID == 0
#define SM_PLATFORM_MU_INST	4 /* MU3 */
#elif RTOSID == 1
#define SM_PLATFORM_MU_INST	6 /* MU4 */
#elif RTOSID == 2
#define SM_PLATFORM_MU_INST	2 /* MU2 */
#else
#error "Unsupported RTOSID!"
#endif

#endif /* _SM_CONFIG_H_ */
