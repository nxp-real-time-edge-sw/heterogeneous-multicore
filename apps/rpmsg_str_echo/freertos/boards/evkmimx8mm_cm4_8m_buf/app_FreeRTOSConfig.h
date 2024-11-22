/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APP_FREERTOS_CONFIG_H
#define APP_FREERTOS_CONFIG_H

#ifdef configTOTAL_HEAP_SIZE
#undef configTOTAL_HEAP_SIZE
#endif

#define configTOTAL_HEAP_SIZE                   ((size_t)(80 * 1024))

#endif /* APP_FREERTOS_CONFIG_H */
