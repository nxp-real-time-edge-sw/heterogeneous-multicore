/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_SRTM_H_
#define _APP_SRTM_H_

#include "rpmsg_lite.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define APP_MS2TICK(ms) ((ms + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS)

#define APP_SRTM_LPUART5_CLK_FREQ CLOCK_GetIpFreq(kCLOCK_Root_Lpuart5)
#define APP_SRTM_LPUART5_BAUDRATE (115200U)
#define APP_SRTM_LPUART5_INSTANCE (5U)
#define APP_SRTM_FIRST_UART_INSTANCE (1U)
#define APP_SRTM_UART_SERIAL_MANAGER_RING_BUFFER_SIZE (1024U)
#define APP_SRTM_UART_SERIAL_MANAGER_BLOCK_TYPE (kSerialManager_NonBlocking)
#define APP_SRTM_UART_TYPE (kSerialPort_Uart)
#define APP_SRTM_UART_RECEIVER_TASK_PRIO (3U)

/* IRQ handler priority definition, bigger number stands for lower priority */

/* Task priority definition, bigger number stands for higher priority */
#define APP_SRTM_MONITOR_TASK_PRIO    (4U)
#define APP_SRTM_DISPATCHER_TASK_PRIO (3U)

/* Define the timeout ms to polling the A Core link up status */
#define APP_LINKUP_TIMER_PERIOD_MS (10U)

#define RPMSG_LITE_SRTM_SHMEM_BASE (VDEV0_VRING_BASE)
#define RPMSG_LITE_SRTM_LINK_ID    (0U)

/* for srtm uart service */
#define APP_SRTM_UART_ENDPOINT_MAX_NUM (0x0BU) /* 11 endpoints are binded to srtm uart channel */
#define APP_SRTM_UART_CHANNEL_NAME "srtm-uart-channel"

#define PEER_CORE_ID (1U)

typedef void (*app_rpmsg_monitor_t)(struct rpmsg_lite_instance *rpmsgHandle, bool ready, void *param);
typedef void (*app_irq_handler_t)(IRQn_Type irq, void *param);

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum
{
    APP_SRTM_StateRun = 0x0U,
    APP_SRTM_StateLinkedUp,
    APP_SRTM_StateReboot,
    APP_SRTM_StateShutdown,
} app_srtm_state_t;

/* Initialize SRTM contexts */
void APP_SRTM_Init(void);

/* Create RPMsg channel and start SRTM communication */
void APP_SRTM_StartCommunication(void);

/* Set RPMsg channel init/deinit monitor */
void APP_SRTM_SetRpmsgMonitor(app_rpmsg_monitor_t monitor, void *param);

#if defined(__cplusplus)
}
#endif

#endif /* _APP_SRTM_H_ */
