/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "os/stdio.h"
#include "os/assert.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app_board.h"
#include "app_rpmsg.h"

#ifndef RTOSID
#define RTOSID				(0)
#endif

/* Task priority and stack size */
#define rpmsg_init_task_PRIORITY	(configMAX_PRIORITIES - 1)
#define rpmsg_ept_task_PRIORITY		(configMAX_PRIORITIES - 1)
#define RPMSG_EPT_TASK_STACK_SIZE	(1024)

/* Each RPMSG buffer can carry less than 512 payload */
#define	RPMSG_BUF_LEN			(512)

static TaskHandle_t rpmsg_ept_task_handle[RPMSG_EPT_NUM];

static void rpmsg_ept_task(void *param)
{
	struct rpmsg_ept *ept = (struct rpmsg_ept *)param;
	char rpmsg_buf[RPMSG_BUF_LEN];
	uint32_t len;
	int result;

	for (;;) {
		/* Receive RPMsg data */
		len = RPMSG_BUF_LEN;
		result = rpmsg_recv(ept, rpmsg_buf, &len, RL_BLOCK);
		os_assert(!result, "ept%d: rpmsg_recv() failed! (ret = %d)", ept->rl_ept->addr, result);

		/* End string by '\0' */
		rpmsg_buf[len] = 0;

#ifdef RPMSG_PRINT_RECV
		if ((len == 2) && (rpmsg_buf[0] == 0xd) && (rpmsg_buf[1] == 0xa))
			os_printf("ept%d: Get New Line From Master Side\r\n", ept->rl_ept->addr);
		else
			os_printf("ept%d: Get Message From Master Side : \"%s\" [len : %d]\r\n", ept->rl_ept->addr, rpmsg_buf, len);
#endif
#ifdef RPMSG_ECHO_BACK
		/* Echo back the received message */
		result = rpmsg_send(ept, rpmsg_buf, len, RL_BLOCK);
		os_assert(!result, "ept%d: rpmsg_send() failed! (ret = %d)", ept->rl_ept->addr, result);
#endif
	}
}

static void rpmsg_init_task(void *param)
{
	struct rpmsg_instance *ri;
	struct rpmsg_ept *ept[RPMSG_EPT_NUM];
	uint32_t ept_addr[RPMSG_EPT_NUM] = RPMSG_EPT_ADDR;
	struct rpmsg_config cfg = {
		.mode = RL_REMOTE,
		.link_id = RPMSG_LITE_LINK_ID,
		.flags = RL_NO_FLAGS,
		.is_coherent = RPMSG_IS_COHERENT,
		.rpmsg_shmem_pa = VDEV0_VRING_BASE,
		.rpmsg_shmem_sz = RL_SHMEM_SIZE,
		.rpmsg_buf_pa = RPMSG_BUF_BASE,
		.rpmsg_buf_sz = RPMSG_BUF_SIZE,
	};
	int i;

	os_printf("\r\n%s: RTOS%d: ", CPU_CORE_NAME, RTOSID);
	/* Print the initial banner */
	os_printf("Multiple Endpoints RPMsg String Echo FreeRTOS Demo...\r\n");

#if (RTOSID == 0) && defined(RAM_CONSOLE_ADDR)
	for (i = 1; i < RTOS_CNT; i++)
		os_printf("RTOS%d: RAM console@0x%x\r\n", i, RAM_CONSOLE_ADDR + RTOS_MEM_LEN * i);
#endif

	ri = rpmsg_init(&cfg);
	if (!ri) {
		os_printf("Failed to initialize RPMsg!\r\n");
		goto err_rpmsg_init;
	}

	for (i = 0; i < RPMSG_EPT_NUM; i++) {
		ept[i] = rpmsg_create_ept(ri, ept_addr[i], RPMSG_LITE_NS_ANNOUNCE_STRING);
		if (!ept[i]) {
			os_printf("Failed to create RPMsg EPT(%d)!\r\n", ept_addr[i]);
			goto err_rpmsg_create_ept;
		}

		if (xTaskCreate(rpmsg_ept_task, "RPMsg_ept_task", RPMSG_EPT_TASK_STACK_SIZE, ept[i],
				rpmsg_ept_task_PRIORITY, &rpmsg_ept_task_handle[i]) != pdPASS) {
			os_printf("\r\nFailed to create task for RPMsg EPT(%d)!\r\n", ept_addr[i]);
			goto err_rpmsg_create_ept_task;
		}
	}

	vTaskSuspend(NULL);

err_rpmsg_create_ept_task:
	rpmsg_destroy_ept(ept[i]);

err_rpmsg_create_ept:
	for (i--; i >= 0; i--) {
		vTaskDelete(rpmsg_ept_task_handle[i]);
		rpmsg_ept_task_handle[i] = NULL;
		rpmsg_destroy_ept(ept[i]);
	}
	rpmsg_deinit(ri);

err_rpmsg_init:
	os_assert(0, "\r\nrpmsg_init_task() failed!\r\n");
}

static void rpmsg_create_task(void)
{
	if (xTaskCreate(rpmsg_init_task, "RPMsg_init_task", configMINIMAL_STACK_SIZE + 100, NULL,
			rpmsg_init_task_PRIORITY, NULL) != pdPASS)
		os_assert(0, "RPMsg init Task creation failed!\r\n");
}

/*!
 * @brief Main function
 */
int main(void)
{
	board_init();

	rpmsg_create_task();

	vTaskStartScheduler();

	for (;;)
		;
}
