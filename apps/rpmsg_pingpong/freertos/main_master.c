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

typedef struct the_message {
	uint32_t DATA;
} THE_MESSAGE, *THE_MESSAGE_PTR;

static volatile THE_MESSAGE msg = {0};

static void rpmsg_init_task(void *param)
{
	struct rpmsg_instance *ri;
	struct rpmsg_ept *ept;
	uint32_t ept_dst_addr = RPMSG_EPT_DST_ADDR;
	struct rpmsg_config cfg = {
		.mode = RL_MASTER,
		.link_id = RPMSG_LITE_LINK_ID,
		.flags = RL_NO_FLAGS,
		.is_coherent = RPMSG_IS_COHERENT,
		.rpmsg_shmem_pa = VDEV0_VRING_BASE,
		.rpmsg_shmem_sz = RL_SHMEM_SIZE,
		.rpmsg_buf_pa = RPMSG_BUF_BASE,
		.rpmsg_buf_sz = RPMSG_BUF_SIZE,
	};
	uint32_t len;
	int ret;

	/* Print the initial banner */
	os_printf("RPMsg Ping-Pong FreeRTOS Demo: master: running at 0x%x\r\n", M_INTERRUPTS_BASE);

	ri = rpmsg_init(&cfg);
	if (!ri) {
		os_printf("Failed to initialize RPMsg!\r\n");
		goto err_rpmsg_init;
	}

	os_printf("Waiting for NS announce ...\r\n");
	/* Wait until the secondary core application issues the nameservice. */
	do {
		ept = rpmsg_get_endpoint_by_dst(ri, ept_dst_addr);
		vTaskDelay(1);
	} while (ept == NULL);

	msg.DATA = 0U;
	while (msg.DATA <= 100U) {
		os_printf("Sending ping (%d) ... ", msg.DATA);
		len = sizeof(THE_MESSAGE);
		ret = rpmsg_send(ept, (void *)&msg, len, msg.DATA ? RL_BLOCK : RL_DONT_BLOCK);
		os_assert(!ret, "ept%d: rpmsg_send() failed! (ret = %d)", ept->rl_ept->addr, ret);

		ret = rpmsg_recv(ept, (void *)&msg, &len, RL_BLOCK);
		os_assert(!ret, "ept%d: rpmsg_recv() failed! (ret = %d)", ept->rl_ept->addr, ret);
		os_printf("received pong (%d)\r\n", msg.DATA);
		msg.DATA++;
	}

	/* Print the ending banner */
	os_printf("\r\nRPMsg demo ends\r\n");

	/* End of the example */
	for (;;)
		vTaskDelay(1);

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
