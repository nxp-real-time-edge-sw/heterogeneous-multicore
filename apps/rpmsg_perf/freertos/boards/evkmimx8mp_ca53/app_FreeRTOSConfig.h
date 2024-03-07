/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APP_FREERTOS_CONFIG_H
#define APP_FREERTOS_CONFIG_H

#ifdef configGENERATE_RUN_TIME_STATS
#undef configGENERATE_RUN_TIME_STATS
#endif
#ifdef configUSE_STATS_FORMATTING_FUNCTIONS
#undef configUSE_STATS_FORMATTING_FUNCTIONS
#endif
#ifdef portGET_RUN_TIME_COUNTER_VALUE
#undef portGET_RUN_TIME_COUNTER_VALUE
#endif

#define configGENERATE_RUN_TIME_STATS           1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE()        xTaskGetTickCount()

#endif /* APP_FREERTOS_CONFIG_H */
