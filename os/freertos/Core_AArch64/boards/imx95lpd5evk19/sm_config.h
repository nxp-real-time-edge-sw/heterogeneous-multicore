/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SM_CONFIG_H_
#define _SM_CONFIG_H_

#if RTOSID == 0
#define SM_PLATFORM_MU_INST	7 /* MU3 */
#elif RTOSID == 1
#define SM_PLATFORM_MU_INST	9 /* MU4 */
#elif RTOSID == 2
#define SM_PLATFORM_MU_INST	5 /* MU2 */
#elif
#error "Unsupported RTOSID!"
#endif

#endif /* _SM_CONFIG_H_ */
