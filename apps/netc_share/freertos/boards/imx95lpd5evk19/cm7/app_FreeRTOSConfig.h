/*
 * Copyright 2025-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APP_FREERTOS_CONFIG_H
#define APP_FREERTOS_CONFIG_H

#undef configMAX_PRIORITIES
#define configMAX_PRIORITIES                    18

/* Memory allocation related definitions. */
#undef configSUPPORT_STATIC_ALLOCATION
#define configSUPPORT_STATIC_ALLOCATION         0

#endif /* APP_FREERTOS_CONFIG_H */
