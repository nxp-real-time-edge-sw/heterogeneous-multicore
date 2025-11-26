/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include <os/assert.h>

#include "srtm_dispatcher.h"
#include "srtm_peercore.h"
#include "srtm_message.h"
#include "srtm_rpmsg_endpoint.h"
#include "srtm_uart_adapter.h"
#include "srtm_uart_service.h"

#include "app_srtm.h"
#include "board.h"
#include "rsc_table.h"
#include "fsl_mu.h"
#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static srtm_dispatcher_t disp;
static srtm_peercore_t core;
static srtm_service_t uartService = NULL;
static srtm_uart_adapter_t uartAdapter = NULL;
static SemaphoreHandle_t monSig;
volatile app_srtm_state_t srtmState;
static struct rpmsg_lite_instance *rpmsgHandle;
static app_rpmsg_monitor_t rpmsgMonitor;
static void *rpmsgMonitorParam;
static TimerHandle_t linkupTimer;

static SERIAL_MANAGER_HANDLE_DEFINE(serialHandle5);
static SERIAL_MANAGER_WRITE_HANDLE_DEFINE(serialWriteHandle5);
static SERIAL_MANAGER_READ_HANDLE_DEFINE(serialReadHandle5);
/* don't have LPUART0, so +1 to skip LPUART0(seq is start from 1, such as: LPUART1, LPUART2...) */
static serial_handle_t serialHandles[FSL_FEATURE_SOC_LPUART_COUNT + 1] = { NULL, NULL, NULL, NULL, NULL, (serial_handle_t)serialHandle5, NULL, NULL, NULL}; /* lpuart5 is used */
static serial_write_handle_t serialWriteHandles[FSL_FEATURE_SOC_LPUART_COUNT + 1] = { NULL, NULL, NULL, NULL, NULL, (serial_write_handle_t)serialWriteHandle5, NULL, NULL, NULL}; /* lpuart5 is used */
static serial_read_handle_t serialReadHandles[FSL_FEATURE_SOC_LPUART_COUNT + 1] = { NULL, NULL, NULL, NULL, NULL, (serial_read_handle_t)serialReadHandle5, NULL, NULL, NULL}; /* lpuart5 is used */
static uint8_t s_ringBuffer[APP_SRTM_UART_SERIAL_MANAGER_RING_BUFFER_SIZE];

/*******************************************************************************
 * Code
 ******************************************************************************/

void APP_SRTM_SetRpmsgMonitor(app_rpmsg_monitor_t monitor, void *param)
{
    rpmsgMonitor      = monitor;
    rpmsgMonitorParam = param;
}

static srtm_status_t APP_SRTM_InitUartDevice(void)
{
    srtm_status_t error = SRTM_Status_Error;
    serial_port_uart_config_t uartConfig[ARRAY_SIZE(serialHandles)] = {0};
    serial_manager_config_t serialManagerConfig[ARRAY_SIZE(serialHandles)] = {0};
    int i = 0;

    uartConfig[APP_SRTM_LPUART5_INSTANCE].clockRate = APP_SRTM_LPUART5_CLK_FREQ;
    uartConfig[APP_SRTM_LPUART5_INSTANCE].baudRate = APP_SRTM_LPUART5_BAUDRATE;
    uartConfig[APP_SRTM_LPUART5_INSTANCE].parityMode = kSerialManager_UartParityDisabled;
    uartConfig[APP_SRTM_LPUART5_INSTANCE].stopBitCount = kSerialManager_UartOneStopBit;
    uartConfig[APP_SRTM_LPUART5_INSTANCE].enableRx = 1;
    uartConfig[APP_SRTM_LPUART5_INSTANCE].enableTx = 1;
    uartConfig[APP_SRTM_LPUART5_INSTANCE].enableRxRTS = 0;
    uartConfig[APP_SRTM_LPUART5_INSTANCE].enableTxCTS = 0;
    uartConfig[APP_SRTM_LPUART5_INSTANCE].instance = APP_SRTM_LPUART5_INSTANCE;

    serialManagerConfig[APP_SRTM_LPUART5_INSTANCE].type = APP_SRTM_UART_TYPE;
    serialManagerConfig[APP_SRTM_LPUART5_INSTANCE].ringBuffer = &s_ringBuffer[0];
    serialManagerConfig[APP_SRTM_LPUART5_INSTANCE].ringBufferSize = sizeof(s_ringBuffer);
    serialManagerConfig[APP_SRTM_LPUART5_INSTANCE].blockType = APP_SRTM_UART_SERIAL_MANAGER_BLOCK_TYPE;
    serialManagerConfig[APP_SRTM_LPUART5_INSTANCE].portConfig = (serial_port_uart_config_t *)&uartConfig[APP_SRTM_LPUART5_INSTANCE];

    for (i = APP_SRTM_FIRST_UART_INSTANCE; i < ARRAY_SIZE(serialHandles); i++)
    {
        if (serialHandles[i] != NULL)
        {
            do {
                if (SerialManager_Init((serial_handle_t)serialHandles[i], &serialManagerConfig[i]) != kStatus_SerialManager_Success)
                    break;
                if (SerialManager_OpenWriteHandle((serial_handle_t)serialHandles[i], (serial_write_handle_t)serialWriteHandles[i]) != kStatus_SerialManager_Success)
                    break;
                if (SerialManager_OpenReadHandle((serial_handle_t)serialHandles[i], (serial_read_handle_t)serialReadHandles[i]) != kStatus_SerialManager_Success)
                    break;
                if (SerialManager_InstallRxCallback((serial_read_handle_t)serialReadHandles[i], SRTM_Uart_RxCallBack, serialReadHandles[i]) != kStatus_SerialManager_Success)
                    break;
                if (SerialManager_InstallTxCallback((serial_write_handle_t)serialWriteHandles[i], SRTM_Uart_TxCallBack, serialWriteHandles[i]) != kStatus_SerialManager_Success)
                    break;
                error = SRTM_Status_Success;
            } while(0);
        }
    }
    return error;
}

static void APP_SRTM_InitUartService(void)
{
    if (SRTM_Status_Success == APP_SRTM_InitUartDevice())
    {
        uartAdapter = SRTM_UartAdapter_Create(serialHandles, serialWriteHandles, serialReadHandles, ARRAY_SIZE(serialHandles));
        assert(uartAdapter);

        /* Create and register serial service */
        uartService = SRTM_UartService_Create(uartAdapter);
        SRTM_Dispatcher_RegisterService(disp, uartService);
    }
    else
    {
        PRINTF("%s: %d Failed to Do Init SRTM Serial Service\r\n", __func__, __LINE__);
    }
}

static void APP_SRTM_PollLinkup(srtm_dispatcher_t dispatcher, void *param1, void *param2)
{
    if (srtmState == APP_SRTM_StateRun)
    {
        if (rpmsg_lite_is_link_up(rpmsgHandle))
        {
            srtmState = APP_SRTM_StateLinkedUp;
            xSemaphoreGive(monSig);
        }
        else
        {
            /* Start timer to poll linkup status. */
            xTimerStart(linkupTimer, portMAX_DELAY);
        }
    }
}

static void APP_LinkupTimerCallback(TimerHandle_t xTimer)
{
    srtm_procedure_t proc = SRTM_Procedure_Create(APP_SRTM_PollLinkup, NULL, NULL);

    if (proc)
    {
        SRTM_Dispatcher_PostProc(disp, proc);
    }
}

static void APP_SRTM_Linkup(void)
{
    uint8_t uart_id = 0;
    srtm_channel_t chan;
    srtm_rpmsg_endpoint_config_t rpmsgConfig;

    /* Create SRTM peer core */
    core = SRTM_PeerCore_Create(PEER_CORE_ID);
    /* Set peer core state to activated */
    SRTM_PeerCore_SetState(core, SRTM_PeerCore_State_Activated);

    /* Common RPMsg channel config */
    rpmsgConfig.localAddr   = RL_ADDR_ANY;
    rpmsgConfig.peerAddr    = RL_ADDR_ANY;
    rpmsgConfig.rpmsgHandle = rpmsgHandle;

    /* Create and add SRTM UART channel to peer core*/
    rpmsgConfig.epName = APP_SRTM_UART_CHANNEL_NAME;
    for (uart_id = 0; uart_id < APP_SRTM_UART_ENDPOINT_MAX_NUM; uart_id++)
    {
        chan = SRTM_RPMsgEndpoint_Create(&rpmsgConfig);
        uartAdapter->bindChanByUartId(chan, SRTM_UART_INVALID_BUS_ID, 0U, uart_id);
        SRTM_PeerCore_AddChannel(core, chan);
    }

    SRTM_Dispatcher_AddPeerCore(disp, core);
}

static void APP_SRTM_NotifyPeerCoreReady(struct rpmsg_lite_instance *rpmsgHandle, bool ready)
{
    /* deinit and init app task(str_echo/pingpong rpmsg) in APP_SRTM_StateReboot only */
    if (rpmsgMonitor && (srtmState == APP_SRTM_StateReboot))
    {
        rpmsgMonitor(rpmsgHandle, ready, rpmsgMonitorParam);
    }
}

static void APP_SRTM_InitPeerCore(void)
{
    copyResourceTable();

    rpmsgHandle = rpmsg_lite_remote_init((void *)RPMSG_LITE_SRTM_SHMEM_BASE, RPMSG_LITE_SRTM_LINK_ID, RL_NO_FLAGS);
    assert(rpmsgHandle);

    APP_SRTM_NotifyPeerCoreReady(rpmsgHandle, true);

    if (rpmsg_lite_is_link_up(rpmsgHandle))
    {
        APP_SRTM_Linkup();
    }
    else
    {
        /* Start timer to poll linkup status. */
        xTimerStart(linkupTimer, portMAX_DELAY);
    }
}

static void APP_SRTM_ResetServices(void)
{
    /* When A Core resets, we need to avoid async event to send to A Core, to do in here */
}

static void APP_SRTM_DeinitPeerCore(void)
{
    /* Stop linkupTimer if it's started. */
    xTimerStop(linkupTimer, portMAX_DELAY);

    /* Notify application for the peer core disconnection. */
    APP_SRTM_NotifyPeerCoreReady(rpmsgHandle, false);

    if (core)
    {
        /* Need to let services know peer core is now down. */
        APP_SRTM_ResetServices();

        SRTM_Dispatcher_RemovePeerCore(disp, core);
        SRTM_PeerCore_Destroy(core);
        core = NULL;
    }

    if (rpmsgHandle)
    {
        rpmsg_lite_deinit(rpmsgHandle);
        rpmsgHandle = NULL;
    }
}

static void APP_SRTM_InitServices(void)
{
    APP_SRTM_InitUartService();
}

static void SRTM_DispatcherTask(void *pvParameters)
{
    SRTM_Dispatcher_Run(disp);
}

static void SRTM_MonitorTask(void *pvParameters)
{
    app_srtm_state_t state = APP_SRTM_StateShutdown;
    int ret;

    /* Initialize services and add to dispatcher */
    APP_SRTM_InitServices();

    /* Start SRTM dispatcher */
    SRTM_Dispatcher_Start(disp);

    /* Monitor peer core state change */
    while (true)
    {
        ret = xSemaphoreTake(monSig, portMAX_DELAY);
        os_assert(ret == pdTRUE, "Failed to take semaphore monSig (ret: %d)", ret);

        if (state == srtmState)
        {
            continue;
        }

        switch (srtmState)
        {
            case APP_SRTM_StateRun:
                assert(state == APP_SRTM_StateShutdown);
                PRINTF("Start SRTM communication\r\n");
                SRTM_Dispatcher_Stop(disp);

                APP_SRTM_InitPeerCore();
                SRTM_Dispatcher_Start(disp);
                state = APP_SRTM_StateRun;
                break;

            case APP_SRTM_StateLinkedUp:
                if (state == APP_SRTM_StateRun)
                {
                    PRINTF("Handle Peer Core Linkup\r\n");
                    SRTM_Dispatcher_Stop(disp);
                    APP_SRTM_Linkup();
                    SRTM_Dispatcher_Start(disp);
                }
                break;

            case APP_SRTM_StateReboot:
                assert(state == APP_SRTM_StateRun);

                PRINTF("Handle Peer Core Reboot\r\n");

                SRTM_Dispatcher_Stop(disp);
                /* Remove peer core from dispatcher */
                APP_SRTM_DeinitPeerCore();

                /* enable clock of MU before accessing registers of MU */
                MU_Init(RPMSG_LITE_M33_A55_MU);

                /* Relase core */
                MU_BootOtherCore(RPMSG_LITE_M33_A55_MU, (mu_core_boot_mode_t)0);

                /* Initialize peer core and add to dispatcher */
                APP_SRTM_InitPeerCore();

                /* Restore srtmState to Run. */
                srtmState = APP_SRTM_StateRun;

                SRTM_Dispatcher_Start(disp);

                /* hold A core for next reboot */
		/* NOTE: MIMX9352 MU does not support hold the other core reset. */
                //MU_HoldOtherCoreReset(RPMSG_LITE_M33_A55_MU);

                /* Do not need to change state. It's still Run. */
                break;

            default:
                assert(false);
                break;
        }
    }
}

void APP_SRTM_Init(void)
{
    BaseType_t xResult;

    monSig = xSemaphoreCreateBinary();
    assert(monSig);

    linkupTimer =
        xTimerCreate("Linkup", pdMS_TO_TICKS(APP_LINKUP_TIMER_PERIOD_MS), pdFALSE, NULL, APP_LinkupTimerCallback);
    assert(linkupTimer);

    /* Create SRTM dispatcher */
    disp = SRTM_Dispatcher_Create();

    xResult = xTaskCreate(SRTM_MonitorTask, "SRTM monitor", 256U, NULL, APP_SRTM_MONITOR_TASK_PRIO, NULL);
    os_assert(xResult == pdPASS, "SRTM_MonitorTask creation failed");
    xResult = xTaskCreate(SRTM_DispatcherTask, "SRTM dispatcher", 512U, NULL, APP_SRTM_DISPATCHER_TASK_PRIO, NULL);
    os_assert(xResult == pdPASS, "SRTM_DispatcherTask creation failed");
}

void APP_SRTM_StartCommunication(void)
{
    srtmState = APP_SRTM_StateRun;
    xSemaphoreGive(monSig);
}
