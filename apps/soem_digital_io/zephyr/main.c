/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_config.h>
#include "ethercattype.h"
#include "nicdrv.h"
#include "ethercatbase.h"
#include "ethercatmain.h"
#include "ethercatdc.h"
#include "ethercatcoe.h"
#include "ethercatfoe.h"
#include "ethercatconfig.h"
#include "ethercatprint.h"
#include "os/stdio.h"
#include "os/counter.h"
#include "os/semaphore.h"
#include "log/log.h"

#define SOEM_PERIOD_US		(125)
#define CTRL_TASK_STACK_SIZE	(1024)

static os_sem_t loop_sem;

OSAL_THREAD_FUNC_RT control_task(void *ifname)
{
	const char *dev_name = ((struct net_if *)ifname)->if_dev->dev->name;
	static char IOmap[100];
	volatile int wkc;
	int oloop, iloop;
	int expectedWKC;
	char chk = 40;

	/* initialise SOEM, and if_port */
	if (!ec_init(ifname)) {
		log_err("ec_init(ifname: %s) failed!\n", dev_name);
		return;
	}

	log_info("ec_init(ifname: %s) succeeded.\n", dev_name);

	if (ec_config_init(FALSE) <= 0) {
		log_err("ec_config_init failed!\n");
		return;
	}

	log_info("%d slaves found and configured.\n", ec_slavecount);

	ec_config_map(&IOmap);
	ec_configdc();
	log_info("Slaves mapped, state to SAFE_OP.\n");

	/* wait for all slaves to reach SAFE_OP state */
	ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE * 4);
	oloop = ec_slave[2].Obytes;
	iloop = ec_slave[3].Ibytes;
	log_info("oloop = %d, iloop = %d\n", oloop, iloop);
	log_info("segments : %d : %d %d %d %d\n",
		ec_group[0].nsegments,
		ec_group[0].IOsegment[0],
		ec_group[0].IOsegment[1],
		ec_group[0].IOsegment[2],
		ec_group[0].IOsegment[3]);

	log_info("Request operational state for all slaves\n");
	expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
	log_info("Calculated workcounter %d\n", expectedWKC);
	ec_slave[0].state = EC_STATE_OPERATIONAL;

	/* send one valid process data to make outputs in slaves happy*/
	ec_send_processdata();
	ec_receive_processdata(EC_TIMEOUTRET);
	/* request OP state for all slaves */
	ec_writestate(0);

	/* wait for all slaves to reach OP state */
	do {
		ec_send_processdata();
		ec_receive_processdata(EC_TIMEOUTRET);
		ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
	} while(chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));
	log_info("Operational state reached for all slaves.\n");

	/* cyclic loop */
	for(;;) {
		ec_send_processdata();
		wkc = ec_receive_processdata(EC_TIMEOUTRET);
		if(wkc >= expectedWKC) {
		}
		os_sem_take(&loop_sem, 0, OS_SEM_TIMEOUT_MAX);
	}
}

static void control_task_wakeup_alarm_handler(os_counter_t *dev, uint8_t chan_id,
					      uint32_t irq_counter, void *user_data)
{
	struct os_counter_alarm_cfg *alarm_cfg = user_data;

	os_sem_give(&loop_sem, OS_SEM_FLAGS_ISR_CONTEXT);
	os_counter_set_channel_alarm(dev, 0, alarm_cfg);
}

static int counter_init(uint32_t period_us)
{
	static struct os_counter_alarm_cfg alarm_cfg;
	os_counter_t *counter_dev;
	int ret;

	counter_dev = (os_counter_t *)DEVICE_DT_GET(DT_ALIAS(counter0));
	if (!counter_dev) {
		log_err("Unable to get counter device\n");
		return -1;
	}

	ret = os_counter_start(counter_dev);
	if (ret) {
		log_err("Counter start failed (ret: %d)\n", ret);
		return ret;
	}

	alarm_cfg.ticks = os_counter_us_to_ticks(counter_dev, period_us);
	alarm_cfg.flags = 0;
	alarm_cfg.user_data = &alarm_cfg;
	alarm_cfg.callback = control_task_wakeup_alarm_handler;

	ret = os_counter_set_channel_alarm(counter_dev, 0, &alarm_cfg);
	if (ret) {
		log_err("Counter set alarm failed (ret: %d)\n", ret);
		return ret;
	}

	return 0;
}

/*!
 * @brief Main function
 */
int main(void)
{
	pthread_t control_task_handle;
	struct net_if *iface;
	int ret;

	ret = os_sem_init(&loop_sem, 0);
	if (ret) {
		log_err("Loop semaphore creation failed!\n");
		return ret;
	}

	ret = counter_init(SOEM_PERIOD_US);
	if (ret) {
		log_err("Counter init failed!\n");
		return ret;
	}

	iface = net_if_get_default();

	/* create control task */
	ret = osal_thread_create_rt(&control_task_handle, CTRL_TASK_STACK_SIZE, &control_task, iface);
	if (!ret) {
		log_err("Create control_task thread failed (ret: %d)!\n", ret);
		return 1;
	}

	if (pthread_join(control_task_handle, NULL)) {
		log_err("pthread_join failed!\n");
		return 1;
	}

	return 0;
}
