/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SRTM_CONFIG_H
#define _SRTM_CONFIG_H

#ifdef SDK_OS_FREE_RTOS
#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SRTM_STATIC_API 0

#endif /* SDK_OS_FREE_RTOS */

#ifdef __cplusplus
}
#endif
#endif /* _SRTM_CONFIG_H */
