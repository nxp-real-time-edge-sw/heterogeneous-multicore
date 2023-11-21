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

/* Task priority and stack size */
#define rpmsg_init_task_PRIORITY	(configMAX_PRIORITIES - 1)
#define rpmsg_ept_task_PRIORITY		(configMAX_PRIORITIES - 1)
#define RPMSG_EPT_TASK_STACK_SIZE	(1024)

static TaskHandle_t rpmsg_ept_task_handle[RPMSG_EPT_NUM];

typedef struct the_message {
	uint32_t DATA;
} THE_MESSAGE, *THE_MESSAGE_PTR;

static volatile THE_MESSAGE msg = {0};

static void rpmsg_ept_task(void *param)
{
	struct rpmsg_ept *ept = (struct rpmsg_ept *)param;
	uint32_t len;
	int ret;

	os_printf("Waiting for ping ...\r\n");

	while (msg.DATA <= 100U) {
		len = sizeof(THE_MESSAGE);
		ret = rpmsg_recv(ept, (void *)&msg, &len, RL_BLOCK);
		os_assert(!ret, "ept%d: rpmsg_recv() failed! (ret = %d)", ept->rl_ept->addr, ret);

		os_printf("Received ping (%d) ... ", msg.DATA);

		msg.DATA++;
		os_printf("sending pong (%d)\r\n", msg.DATA);
		ret = rpmsg_send(ept, (void *)&msg, len, RL_BLOCK);
		os_assert(!ret, "ept%d: rpmsg_send() failed! (ret = %d)", ept->rl_ept->addr, ret);
	}

	msg.DATA = 0U;

	os_printf("\r\nRPMsg demo ends\r\n");

	/* End of the example */
	for (;;)
		vTaskDelay(1);
}

static void rpmsg_init_task(void *param)
{
	struct rpmsg_instance *ri;
	struct rpmsg_ept *ept;
	uint32_t ept_addr = RPMSG_EPT_ADDR;
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


	/* Print the initial banner */
	os_printf("RPMsg Ping-Pong FreeRTOS Demo: remote: running at 0x%x\r\n", M_INTERRUPTS_BASE);

	os_printf("master RAM_CONSOLE at 0x%x\r\n", RAM_CONSOLE_ADDR + 0x1000000);

	ri = rpmsg_init(&cfg);
	if (!ri) {
		os_printf("Failed to initialize RPMsg!\r\n");
		goto err_rpmsg_init;
	}

	ept = rpmsg_create_ept(ri, ept_addr, RPMSG_LITE_NS_ANNOUNCE_STRING);
	if (!ept) {
		os_printf("Failed to create RPMsg EPT(%d)!\r\n", ept_addr);
		goto err_rpmsg_create_ept;
	}

	if (xTaskCreate(rpmsg_ept_task, "RPMsg_ept_task", RPMSG_EPT_TASK_STACK_SIZE, ept,
			rpmsg_ept_task_PRIORITY, &rpmsg_ept_task_handle[0]) != pdPASS) {
		os_printf("\r\nFailed to create task for RPMsg EPT(%d)!\r\n", ept_addr);
		goto err_rpmsg_create_ept_task;
	}

	vTaskSuspend(NULL);

err_rpmsg_create_ept_task:
	rpmsg_destroy_ept(ept);

err_rpmsg_create_ept:
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
