/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020, 2022-2025 NXP
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
#include "app_board.h"

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

#define NUM_1M      (1000000UL)
#define SOEM_PERIOD 40 /* 1 millisecond */
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static os_counter_t *main_counter_dev;

/* The sum of ticks from the timer started */
static volatile uint64_t system_ticks;

/* The last counter ticks when alarm_handler happen */
static volatile uint32_t ticks_last;

/* The ticks per each period of rt_task */
static volatile uint32_t ticks_per_period;

static char IOmap[100];

static StaticTask_t xTaskBuffer;
static TaskHandle_t rt_task = NULL;
static StackType_t rt_task_stack[RT_TASK_STACK_SIZE];
/*******************************************************************************
 * Code
 ******************************************************************************/

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

	BOARD_InitHardware();
	osal_timer_init(SOEM_PERIOD);
	if_port_init();

    rt_task = xTaskCreateStatic(/* The function that implements the task. */
                                control_task, "RT_task", RT_TASK_STACK_SIZE, SOEM_PORT_NAME, configMAX_PRIORITIES - 1,
                                rt_task_stack, &xTaskBuffer);
    vTaskStartScheduler();
    for (;;)
        ;

	/* Will not get here unless a task calls vTaskEndScheduler ()*/
	return 0;
}
