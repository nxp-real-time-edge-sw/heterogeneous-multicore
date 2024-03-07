/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020,2022-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "fsl_debug_console.h"
#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "app_enet.h"

#include "ethercattype.h"
#include "nicdrv.h"
#include "ethercatbase.h"
#include "ethercatmain.h"
#include "ethercatdc.h"
#include "ethercatcoe.h"
#include "ethercatfoe.h"
#include "ethercatconfig.h"
#include "ethercatprint.h"
#include "enet/soem_enet.h"
#include "soem_port.h"

#include "os/stdio.h"
#include "os/unistd.h"
#include "os/assert.h"
#include "os/irq.h"
#include "os/counter.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Stack size of the temporary soem initialization thread. */
#define RT_TASK_STACK_SIZE 1024

#ifndef PHY_AUTONEGO_TIMEOUT_COUNT
#define PHY_AUTONEGO_TIMEOUT_COUNT (100000)
#endif

#ifndef PHY_STABILITY_DELAY_US
#define PHY_STABILITY_DELAY_US (0U)
#endif

#define NUM_1M      (1000000UL)
#define SOEM_PERIOD 40 /* 1 millisecond */

#define OSEM_PORT_NAME "enet0"

#define ENET_RXBD_NUM (4)
#define ENET_TXBD_NUM (4)

#define ENET_RXBUFF_SIZE (ENET_FRAME_MAX_FRAMELEN)
#define ENET_TXBUFF_SIZE (ENET_FRAME_MAX_FRAMELEN)

#define ENET_PRIORITY (portLOWEST_USABLE_INTERRUPT_PRIORITY - 1)

#ifndef APP_ENET_BUFF_ALIGNMENT
#define APP_ENET_BUFF_ALIGNMENT		ENET_BUFF_ALIGNMENT
#endif

#define STATIC_BUFF_COUNT	(ENET_RXBD_NUM * 3)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static phy_rtl8211f_resource_t phy_resource;

static struct enet_if_port if_port;

static os_counter_t *main_counter_dev;

/* The sum of ticks from the timer started */
static volatile uint64_t system_ticks;

/* The last counter ticks when alarm_handler happen */
static volatile uint32_t ticks_last;

/* The ticks per each period of rt_task */
static volatile uint32_t ticks_per_period;

static char IOmap[100];

static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t IdleTaskTCB;

static StackType_t TimerTaskStacj[configMINIMAL_STACK_SIZE];
static StaticTask_t TimerTaskTCB;

static StaticTask_t xTaskBuffer;
static TaskHandle_t rt_task = NULL;
static StackType_t rt_task_stack[RT_TASK_STACK_SIZE];


/*! @brief Buffer descriptors should be in non-cacheable region and should be align to "ENET_BUFF_ALIGNMENT". */
AT_NONCACHEABLE_SECTION_ALIGN(static enet_rx_bd_struct_t g_rxBuffDescrip[ENET_RXBD_NUM], ENET_BUFF_ALIGNMENT);
AT_NONCACHEABLE_SECTION_ALIGN(static enet_tx_bd_struct_t g_txBuffDescrip[ENET_TXBD_NUM], ENET_BUFF_ALIGNMENT);

/*
 * The data buffers can be in cacheable region or in non-cacheable region.
 * If use cacheable region, the alignment size should be the maximum size of "CACHE LINE SIZE" and "ENET_BUFF_ALIGNMENT"
 * If use non-cache region, the alignment size is the "ENET_BUFF_ALIGNMENT".
 */
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && (FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL)
SDK_ALIGN(static uint8_t g_enet_rx_buffer[STATIC_BUFF_COUNT][SDK_SIZEALIGN(ENET_RXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)], ENET_BUFF_ALIGNMENT);
SDK_ALIGN(static uint8_t g_enet_tx_buffer[ENET_TXBD_NUM][SDK_SIZEALIGN(ENET_TXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)], ENET_BUFF_ALIGNMENT);
#else
NONCACHEABLE(static uint8_t g_enet_rx_buffer[STATIC_BUFF_COUNT][SDK_SIZEALIGN(ENET_RXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)], ENET_BUFF_ALIGNMENT);
NONCACHEABLE(static uint8_t g_enet_tx_buffer[ENET_TXBD_NUM][SDK_SIZEALIGN(ENET_TXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)], ENET_BUFF_ALIGNMENT);
#endif

static enet_buffer_config_t buffConfig[] = {{
    ENET_RXBD_NUM,
    ENET_TXBD_NUM,
    SDK_SIZEALIGN(ENET_RXBUFF_SIZE, ENET_BUFF_ALIGNMENT),
    SDK_SIZEALIGN(ENET_TXBUFF_SIZE, ENET_BUFF_ALIGNMENT),
    &g_rxBuffDescrip[0],
    &g_txBuffDescrip[0],
    &g_enet_rx_buffer[0][0],
    &g_enet_tx_buffer[0][0],
    true,
    true,
    NULL,
}};

/*******************************************************************************
 * Code
 ******************************************************************************/

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer   = &IdleTaskTCB;
    *ppxIdleTaskStackBuffer = &IdleTaskStack[0];
    *pulIdleTaskStackSize   = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer   = &TimerTaskTCB;
    *ppxTimerTaskStackBuffer = &TimerTaskStacj[0];
    *pulTimerTaskStackSize   = configMINIMAL_STACK_SIZE;
}

static void rt_task_wakeup_alarm_handler(os_counter_t *dev, uint8_t chan_id,
			  uint32_t irq_counter,
			  void *user_data)
{
	uint32_t ticks;
    BaseType_t xHigherPriorityTaskWoken;
	
    os_counter_get_value(dev, &ticks);
    ticks_last = ticks;
    system_ticks += ticks_per_period;
    if (rt_task)
    {
        xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(rt_task, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static void osal_timer_init( uint32_t usec)
{
	uint32_t current_tick;
	struct os_counter_alarm_cfg alarm_cfg;

	main_counter_dev = GET_COUNTER_DEVICE_INSTANCE(0);
	ticks_per_period = os_counter_us_to_ticks(main_counter_dev, usec);
	os_counter_start(main_counter_dev);
	alarm_cfg.flags = OS_COUNTER_ALARM_CFG_LOOP;
	alarm_cfg.user_data = NULL;
	alarm_cfg.callback = rt_task_wakeup_alarm_handler;	
	os_counter_get_value(main_counter_dev, &current_tick);
	alarm_cfg.ticks = ticks_per_period;
	system_ticks = current_tick;
	os_counter_set_channel_alarm(main_counter_dev, BOARD_OUTPUT_COMPARE_CHANNEL, &alarm_cfg);
}

void osal_gettime(struct timeval *current_time)
{
    uint64_t sys_time_ns;
    uint32_t ticks;
    uint64_t sys_ticks;
    uint32_t ticks_last_temp;

	/*if alarm_handler happen during os_counter_get_value, ticks must be read again */
    ticks_last_temp = ticks_last;
    sys_ticks = system_ticks;
    os_counter_get_value(main_counter_dev, &ticks);

    if (sys_ticks != system_ticks || ticks_last_temp != ticks_last)
    {
        sys_ticks = system_ticks;
        ticks_last_temp = ticks_last;
    	os_counter_get_value(main_counter_dev, &ticks);
    }

	sys_ticks += ticks > ticks_last_temp ? ticks - ticks_last_temp : (0xFFFFFFFF - ticks_last_temp + ticks);
	sys_time_ns = os_counter_ticks_to_ns(main_counter_dev, sys_ticks);

    current_time->tv_sec  = sys_time_ns / 1000000000;
    current_time->tv_usec = (sys_time_ns % 1000000000) / 1000;

    return;
}

static status_t MDIO_Write(uint8_t phyAddr, uint8_t regAddr, uint16_t data)
{
	return ENET_MDIOWrite(ENET, phyAddr, regAddr, data);
}

static status_t MDIO_Read(uint8_t phyAddr, uint8_t regAddr, uint16_t *pData)
{
	return ENET_MDIORead(ENET, phyAddr, regAddr, pData);
}

/*!
 * @brief Initializes soem interface.
 */

static int if_port_init(void)
{
	ENET_SetSMI(ENET, ENET_CLOCK_FREQ, false);
    phy_resource.read  = MDIO_Read;
    phy_resource.write = MDIO_Write;
	memset(&if_port, 0, sizeof(if_port));
    if_port.bufferConfig = buffConfig;
    if_port.base         = ENET;

    /* The miiMode should be set according to the different PHY interfaces. */
    if_port.mii_mode                   = kENET_RgmiiMode;
    if_port.phy_config.autoNeg         = true;
    if_port.phy_config.phyAddr         = ENET_PHY_ADDRESS;
    if_port.phy_config.resource        = &phy_resource;
    if_port.phy_config.ops             = &ENET_PHY_OPS;
    if_port.srcClock_Hz                = ENET_CLOCK_FREQ;
    if_port.phy_autonego_timeout_count = PHY_AUTONEGO_TIMEOUT_COUNT;
    if_port.phy_stability_delay_us     = PHY_STABILITY_DELAY_US;

	return register_soem_port(OSEM_PORT_NAME, "enet", &if_port);
}

void control_task(void *ifname)
{
    int oloop, iloop;
    int expectedWKC;
    volatile int wkc;
    const TickType_t xBlockTime = pdMS_TO_TICKS(500);
    char chk                    = 40;

    /* initialise SOEM, and if_port */
    if (ec_init(ifname))
    {
        os_printf("ec_init on %s succeeded.\r\n", ifname);
        if (ec_config_init(FALSE) > 0)
        {
            os_printf("%d slaves found and configured.\r\n", ec_slavecount);
            ec_config_map(&IOmap);
            ec_configdc();
            os_printf("Slaves mapped, state to SAFE_OP.\r\n");
            /* wait for all slaves to reach SAFE_OP state */
            ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE * 4);
            oloop = ec_slave[2].Obytes;
            iloop = ec_slave[3].Ibytes;
            os_printf("oloop = %d, iloop = %d\r\n", oloop, iloop);
            os_printf("segments : %d : %d %d %d %d\r\n", ec_group[0].nsegments, ec_group[0].IOsegment[0],
                   ec_group[0].IOsegment[1], ec_group[0].IOsegment[2], ec_group[0].IOsegment[3]);

            os_printf("Request operational state for all slaves\r\n");
            expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
            os_printf("Calculated workcounter %d\r\n", expectedWKC);
            ec_slave[0].state = EC_STATE_OPERATIONAL;
            /* send one valid process data to make outputs in slaves happy*/
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);
            /* request OP state for all slaves */
            ec_writestate(0);
            /* wait for all slaves to reach OP state */
            do
            {
                ec_send_processdata();
                ec_receive_processdata(EC_TIMEOUTRET);
                ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
            } while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));
            os_printf("Operational state reached for all slaves.\r\n");
            /* cyclic loop */
            ulTaskNotifyTake(pdTRUE, xBlockTime);
            ec_send_processdata();
            for (;;)
            {
                wkc = ec_receive_processdata(EC_TIMEOUTRET);
                if (wkc >= expectedWKC)
                {
                }
            	ec_send_processdata();
                ulTaskNotifyTake(pdFALSE, xBlockTime);
            }
        }
    } else {
		os_printf("ec_init on %s failed.\r\n", ifname);
	}

	vTaskSuspend(NULL);
    while(1);
}

int main(void)
{

	BOARD_InitMemory();
	BOARD_RdcInit();
	/* Enable GIC before register any interrupt handler*/
	GIC_Enable(1);
	board_pins_setup();
	board_clock_setup();
	BOARD_InitDebugConsole();
	osal_timer_init(SOEM_PERIOD);
	if_port_init();

    rt_task = xTaskCreateStatic(/* The function that implements the task. */
                                control_task, "RT_task", RT_TASK_STACK_SIZE, OSEM_PORT_NAME, configMAX_PRIORITIES - 1,
                                rt_task_stack, &xTaskBuffer);
    vTaskStartScheduler();
    for (;;)
        ;

	/* Will not get here unless a task calls vTaskEndScheduler ()*/
	return 0;
}
