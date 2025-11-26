/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SM_CONFIG_H_
#define _SM_CONFIG_H_

#if RTOSID == 0
#define SM_PLATFORM_MU_INST	4 /* MUA3 */
#elif RTOSID == 1
#define SM_PLATFORM_MU_INST	6 /* MUA4 */
#elif RTOSID == 2
#define SM_PLATFORM_MU_INST	2 /* MUA2 */
#elif
#error "Unsupported RTOSID!"
#endif

#endif /* _SM_CONFIG_H_ */
