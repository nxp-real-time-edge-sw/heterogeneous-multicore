/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APP_FREERTOS_CONFIG_H
#define APP_FREERTOS_CONFIG_H

#ifdef configUSE_TIMERS
#undef configUSE_TIMERS
#endif
#ifdef configTIMER_TASK_PRIORITY
#undef configTIMER_TASK_PRIORITY
#endif
#ifdef configTIMER_TASK_STACK_DEPTH
#undef configTIMER_TASK_STACK_DEPTH
#endif
#ifdef configTIMER_QUEUE_LENGTH
#undef configTIMER_QUEUE_LENGTH
#endif
#ifdef configTOTAL_HEAP_SIZE
#undef configTOTAL_HEAP_SIZE
#endif

#define configTOTAL_HEAP_SIZE                   ((size_t)(115 * 1024))

#define configUSE_TIMERS			1
#define configTIMER_TASK_PRIORITY		(configMAX_PRIORITIES - 3)
#define configTIMER_TASK_STACK_DEPTH		(configMINIMAL_STACK_SIZE + 100)
#define configTIMER_QUEUE_LENGTH		100

#endif /* APP_FREERTOS_CONFIG_H */
