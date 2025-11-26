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

#include "cia402.h"
#include "servo.h"

#define SOEM_PERIOD_US		(2000)
#define CYCLE_PERIOD_NS		(SOEM_PERIOD_US * 1000)
#define CTRL_TASK_STACK_SIZE	(1024)

#define nxp_VendorId 0x00000CC2
#define nxp_ProductID 0x00000002

#define MAX_SERVO 1
#define MAX_AXIS 1

static os_sem_t loop_sem;

static char *tp[MAX_SERVO] = {
// NXP
"Cyclic=1; Scale=364; Bias=0; Accel=8; Decel=8; Max_speed=3600; TpArrays=[(0:2000),(270:1000),(270:2000),(180:1000),(180:2000),(0:1000),(0:2000),(0:1000)];",
};

static struct servo_t servo[MAX_SERVO];
static struct axis_t axis[MAX_AXIS];

static void EtherCAT_servo_init(struct servo_t *svo, struct axis_t *ax)
{
	memset(svo, 0, sizeof(struct servo_t) * MAX_SERVO);
	memset(ax, 0, sizeof(struct axis_t) * MAX_AXIS);

	svo[0].slave_id = 0;
	svo[0].axis_num = 1;
	svo[0].VendorId = nxp_VendorId;
	svo[0].ProductID = nxp_ProductID;

	ax[0].servo = svo;
	ax[0].axis_offset = 0;
}

static int EtherCAT_servo_setup(uint16 slave)
{
	int i;
	int ret = 0;
	int chk = 10;
	struct servo_t *svo = NULL;

	for (i = 0; i < MAX_SERVO; i++) {
		if (servo[i].slave_id + 1 == slave) {
			svo = &servo[i];
			break;
		}
	}
	if (svo) {
		while (chk--) {
			ret = servo_pdo_activate_map(svo);
			if (ret) {
				break;
			}
		}
	}

	if (!ret)
		return 0;
	else
		return -1;
}

static int servo_setup(struct servo_t *servo)
{
	int si = servo->slave_id + 1;

	if((ec_slave[si].eep_man == servo->VendorId) && (ec_slave[si].eep_id == servo->ProductID)) {
		ec_slave[si].PO2SOconfig = EtherCAT_servo_setup;
	}

	return 0;
}

OSAL_THREAD_FUNC_RT control_task(void *ifname)
{
	const char *dev_name = ((struct net_if *)ifname)->if_dev->dev->name;
	static char IOmap[1500];
	volatile int wkc;
	int expectedWKC, i;
	int chk = 40;
	int wkc_lost = 0;

	log_info("Starting mition task\n");
	EtherCAT_servo_init(servo, axis);

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

	if (ec_slavecount < MAX_SERVO) {
		log_err("The number of Servo scanned is not consistent with configed, please reconfirm\n");
		return;
	}

	i = servo_slave_check(servo, MAX_SERVO);
	if (i < 0) {
		log_err("The infomation of Servo:%d is not consistent with scanned, please reconfirm\n", -i);
		return;
	}

	ec_configdc();
	chk = 100;
	while (chk--) {
		if (servo_synced_check(servo, MAX_SERVO) == 1) {
			break;
		}
	}

	for (i = 0; i < MAX_SERVO; i++) {
		if(servo[i].slave->hasdc > 0) {
			ec_dcsync0(servo[i].slave_id + 1, TRUE, CYCLE_PERIOD_NS, CYCLE_PERIOD_NS * 3);
		}
	}

	for (i = 0; i < MAX_SERVO; i++) {
		servo_setup(&servo[i]);
	}

	ec_config_map(&IOmap);
	for (i = 0; i < MAX_AXIS; i++) {
		axis_nc_init(&axis[i], tp[i], CYCLE_PERIOD_NS);
	}

	log_info("Slaves mapped, state to SAFE_OP.\n");

	/* wait for all slaves to reach SAFE_OP state */
	ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE * 4);

	for (i = 0; i < MAX_AXIS; i++) {
		PDO_write_targe_position(&axis[i], axis[i].current_position);
		axis_nc_start(&axis[i]);
	}

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
	chk = 200;
	/* wait for all slaves to reach OP state */
	do {
		ec_send_processdata();
		ec_receive_processdata(EC_TIMEOUTRET);
		ec_statecheck(1, EC_STATE_OPERATIONAL, 50000);
	} while(chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));
	log_info("Operational state reached for all slaves.\n");

	if (ec_slave[0].state != EC_STATE_OPERATIONAL) {
		log_info("Not all slaves reached operational state.\r\n");
	} else {
		ec_send_processdata();
		int unop_num;
		for(;;) {
			wkc = ec_receive_processdata(EC_TIMEOUTRET);
			unop_num = 0;
			for(i = 0; i < MAX_AXIS; i++) {
				if (axis_start(&axis[i], op_mode_csp) != 1) {
					unop_num++;
				}
			}

			if (unop_num == 0) {
				if(wkc >= expectedWKC) {
					for(i = 0; i < MAX_AXIS; i++) {
						axis[i].current_velocity = PDO_read_actual_velocity(&axis[i]);
						axis[i].current_position = PDO_read_actual_position(&axis[i]);
						if (axis[i].axis_status.csp_status == csp_status_running || axis[i].axis_status.csp_status == csp_status_pre_stop) {
							int pos = axis_nc_get_next_pos(&axis[i]);
							PDO_write_targe_position(&axis[i], pos);
						}
					}
					if (axis[MAX_AXIS-1].axis_status.csp_status == csp_status_stop) {
						break;
					}
				} else {
					wkc_lost++;
					ec_slave[0].state = EC_STATE_OPERATIONAL;
					ec_writestate(0);
				}
			}
			ec_send_processdata();
			os_sem_take(&loop_sem, 0, OS_SEM_TIMEOUT_MAX);
		}

		/* Dump status codes*/
		for(i = 1; i<=ec_slavecount ; i++) {
			if(ec_slave[i].state != EC_STATE_OPERATIONAL) {
				log_info("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\r\n", i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
			}
		}
		ec_slave[0].state = EC_STATE_INIT;
		/* request INIT state for all slaves */
		ec_writestate(0);
	}
	
	/* stop SOEM, close socket */
	ec_writestate(0);
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
