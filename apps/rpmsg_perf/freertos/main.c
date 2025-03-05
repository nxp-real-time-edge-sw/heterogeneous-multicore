/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "os/stdio.h"
#include "os/assert.h"
#include "os/cpu_load.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app_board.h"
#include "app_rpmsg.h"
#include "semphr.h"

/* Task priority and stack size */
#define rpmsg_init_task_PRIORITY	(configMAX_PRIORITIES - 2)
#define rpmsg_recv_task_PRIORITY	(configMAX_PRIORITIES - 2)
#define rpmsg_send_task_PRIORITY	(configMAX_PRIORITIES - 1)

#define RPMSG_RECV_TASK_STACK_SIZE	(4096)
#define RPMSG_SEND_TASK_STACK_SIZE	(4096)

#define RPMSG_EPT_ADDR			(30)

/* RPMSG buffer payload size less than 496 */
#define	APP_BUFFER_PAYLOAD_SIZE		(496)

#define MS_PER_SECOND			(1000U)

enum {
	RPMSG_PERF_PREAMBLE_SENDER_START     = 0xBECAACEA, /* Linux as Sender, start packet */
	RPMSG_PERF_PREAMBLE_SENDER_END       = 0xBECAACEB, /* Linux as Sender, end packet */
	RPMSG_PERF_PREAMBLE_SENDER_END_ACK   = 0xBECAACEC, /* Linux as Sender, end ack packet */
	RPMSG_PERF_PREAMBLE_RECEIVER_START   = 0xBECAACED, /* Linux as Receiver */
	RPMSG_PERF_PREAMBLE_RECEIVER_END     = 0xBECAACEE, /* Linux as Receiver */
	RPMSG_PERF_PREAMBLE_RECEIVER_END_ACK = 0xBECAACEF, /* Linux as Receiver */
};

struct packet_header {
	uint32_t preamble;
	bool no_copy;
	uint32_t packet_size; /* every packet size*/
	uint32_t packet_cnt;  /* packet count */
	uint32_t test_time;   /* when Linux as receiver, FreeRTOS need it, unit: second */
};

struct sender_args {
	uint32_t packet_size;
	bool no_copy;
	TickType_t end_ticks;
	uint32_t send_packet_cnt;
};

static TaskHandle_t rpmsg_recv_task_handle;
static TaskHandle_t rpmsg_send_task_handle;

static volatile struct sender_args args;
static SemaphoreHandle_t send_packet_sig;

static uint32_t sender_poll_times;
static uint32_t delayed_packets_count;
static bool wait_buffer;

void APP_SendPacket(struct rpmsg_ept *ept, void *p_buf, uint32_t buf_size, bool no_copy, bool have_preamble)
{
	uint32_t size = 0;
	void *tx_buf;
	int32_t result;

	do {
		if (no_copy) {
			/* Get tx buffer from RPMsg */
			uint32_t tmp_size = 0;

			do {
				tx_buf = rpmsg_lite_alloc_tx_buffer(ept->ri->rl_inst, &tmp_size, RL_FALSE);
				if (!have_preamble && (tx_buf == RL_NULL)) {
					sender_poll_times++; /* record poll times when send data(invalid) packet */
					wait_buffer = true;
				}
			} while (tx_buf == RL_NULL);

			size = (buf_size > tmp_size) ? (tmp_size) : (buf_size);

			if (have_preamble) {
				memcpy(tx_buf, p_buf, size);
			} else if (wait_buffer) {
				wait_buffer = false;
				delayed_packets_count++;
			}
			result = rpmsg_lite_send_nocopy(ept->ri->rl_inst, ept->rl_ept, ept->remote_addr, tx_buf, size);
		} else {
			size = (buf_size > APP_BUFFER_PAYLOAD_SIZE) ? (APP_BUFFER_PAYLOAD_SIZE) : (buf_size);
			do {
				result = rpmsg_send(ept, p_buf, size, RL_FALSE);
				if (!have_preamble && (result != RL_SUCCESS)) {
					sender_poll_times++; /* record poll times when send data(valid) packet */
					wait_buffer = true;
				}
			} while (result != RL_SUCCESS);

			if (wait_buffer) {
				wait_buffer = false;
				delayed_packets_count++;
			}
		}

		os_assert(result == RL_SUCCESS, "ept%d: rpmsg send failed! (ret = %d)", ept->rl_ept->addr, result);

		if (buf_size > size) {
			buf_size -= size;
			p_buf += size;
		} else {
			buf_size = 0;
		}

	} while (buf_size > 0);
}

static void rpmsg_send_task(void *param)
{
	struct rpmsg_ept *ept = (struct rpmsg_ept *)param;
	char sender_buffer[APP_BUFFER_PAYLOAD_SIZE];
	TickType_t current_ticks = 0;
	struct packet_header out = {0};
	int result;
	int i;

	for (i = 0; i < sizeof(sender_buffer); i++) {
		char c = 'A' + (i % 26);
		sender_buffer[i] = c;
	}

	for (;;) {
		/* wait a signal to send packet */
		result = xSemaphoreTake(send_packet_sig, portMAX_DELAY);
		os_assert(result == pdTRUE, "Failed to take semaphore send_packet_sig (ret: %d)", result);
		os_printf("Sender Start:\r\n");
		os_printf("Test case: Packet size: %u, Buffer copy: %s\r\n", args.packet_size, args.no_copy ? "NO" : "YES");
		os_cpu_load_stats();

		do {
			if (args.packet_size < sizeof(sender_buffer)) {
				APP_SendPacket(ept, sender_buffer, args.packet_size, args.no_copy, false);
			} else {
				uint32_t count = args.packet_size;
				do {
					APP_SendPacket(ept, sender_buffer, (count > sizeof(sender_buffer)) ?
							(sizeof(sender_buffer)) : (count), args.no_copy, false);
					if (count > sizeof(sender_buffer)) {
						count -= sizeof(sender_buffer);
					} else {
						count = 0;
					}
				} while (count > 0);
			}

			args.send_packet_cnt++;
			current_ticks = xTaskGetTickCount();
			if (current_ticks >= args.end_ticks) {
				/* send a packet to remote core about count of send packets */
				out.packet_cnt = args.send_packet_cnt;
				out.preamble = RPMSG_PERF_PREAMBLE_RECEIVER_END; /* Linux will change role from receiver to sender */
				APP_SendPacket(ept, &out, sizeof(out), args.no_copy, true);
				break;
			}
		} while (1);

		os_cpu_load_stats();
		os_printf("Sender End:\r\n");
	}
}

static void rpmsg_recv_task(void *param)
{
	struct rpmsg_ept *ept = (struct rpmsg_ept *)param;
	char buf[APP_BUFFER_PAYLOAD_SIZE];
	void *rx_buf;
	struct packet_header *p_in = NULL;
	struct packet_header out = {0};
	bool release_buf = false;
	bool no_copy_old = true;
	bool no_copy = true;
	uint32_t rx_len = 0U;
	uint32_t msg_src_addr;
	uint32_t packet_size = 1;
	uint32_t len;
	int result;

	for (;;) {
		if (no_copy) {
			result = rpmsg_queue_recv_nocopy(ept->ri->rl_inst, ept->ept_q, (uint32_t *)&msg_src_addr,
							 (char **)&rx_buf, &len, RL_BLOCK);
			if (ept->remote_addr == RL_ADDR_ANY)
				ept->remote_addr = msg_src_addr;
			else
				os_assert((ept->remote_addr == msg_src_addr), "ept%d: rpmsg_queue_recv_nocopy() get msg from (%d)!",
					  ept->rl_ept->addr, msg_src_addr);

			release_buf = true;
		} else {
			rx_buf = buf;
			len = APP_BUFFER_PAYLOAD_SIZE;
			result = rpmsg_recv(ept, rx_buf, &len, RL_BLOCK);

			release_buf = false;
		}

		os_assert(!result, "ept%d: rpmsg receive failed! (ret = %d)", ept->rl_ept->addr, result);

		os_assert(len <= APP_BUFFER_PAYLOAD_SIZE, "ept%d: rpmsg received msg.len = %d", ept->rl_ept->addr, len);

		p_in = (struct packet_header *)rx_buf;

		switch (p_in->preamble) {
		case RPMSG_PERF_PREAMBLE_SENDER_START:
			no_copy_old = no_copy;
			no_copy = p_in->no_copy;
			packet_size = p_in->packet_size;
			rx_len = 0;
			os_printf("Receiver Start:\r\n");
			os_printf("Test case: Packet size: %u, Buffer copy: %s\r\n", packet_size, no_copy ? "NO" : "YES");
			os_cpu_load_stats();
			break;
		case RPMSG_PERF_PREAMBLE_SENDER_END:
			out.packet_cnt = rx_len / packet_size;
			out.preamble = RPMSG_PERF_PREAMBLE_SENDER_END_ACK;
			/* send SENDER_END_ACK packet to remote core(Linux) */
			APP_SendPacket(ept, &out, sizeof(out), no_copy, true);
			os_cpu_load_stats();
			os_printf("Receiver End:\r\n");
			os_printf("Received packets: %u\r\n", out.packet_cnt);
			rx_len = 0;
			no_copy = no_copy_old;
			break;
		case RPMSG_PERF_PREAMBLE_RECEIVER_START: {
			uint32_t test_time = p_in->test_time; /* unit: second */
			TickType_t test_time_ticks = pdMS_TO_TICKS((uint64_t)test_time * MS_PER_SECOND);
			TickType_t current_ticks = 0;

			memset((void *)&args, 0, sizeof(args));
			current_ticks = xTaskGetTickCount();
			args.packet_size = p_in->packet_size;
			args.end_ticks = current_ticks + test_time_ticks;
			no_copy = p_in->no_copy;
			args.no_copy = p_in->no_copy;
			sender_poll_times = 0U;
			delayed_packets_count = 0U;
			result = xSemaphoreGive(send_packet_sig);
			os_assert(result == pdTRUE, "Failed to give semaphore send_packet_sig (ret: %d)", result);
			break;
		}
		case RPMSG_PERF_PREAMBLE_RECEIVER_END_ACK: {
			uint32_t remote_received_packet_cnt = p_in->packet_cnt;
			uint32_t packet_loss = 0;

			if (args.send_packet_cnt > 0) {
				packet_loss = (uint32_t)(((float)(args.send_packet_cnt - remote_received_packet_cnt) /
						(float)(args.send_packet_cnt)) * 100);
			}

			os_printf("\r\nSent out packets: %u, Linux received packets: %u, packet_loss: %u %%\r\n",
					args.send_packet_cnt, remote_received_packet_cnt, packet_loss);
			break;
		}
		default:
			rx_len += len;
			break;
		}

		/* Release held RPMsg rx buffer */
		if (release_buf) {
			result = rpmsg_queue_nocopy_free(ept->ri->rl_inst, rx_buf);
			os_assert(result == RL_SUCCESS, "ept%d: free rpmsg buffer failed! (ret = %d)", ept->rl_ept->addr, result);
		}
	}
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

	send_packet_sig = xSemaphoreCreateBinary();
	os_assert(send_packet_sig, "\r\nCreate send_packet_sig failed!\r\n");

	/* Print the initial banner */
	os_printf("\r\n%s: RPMsg performance with linux:\r\n", CPU_CORE_NAME);

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

	if (xTaskCreate(rpmsg_recv_task, "RPMsg_recv_task", RPMSG_RECV_TASK_STACK_SIZE, ept,
			rpmsg_recv_task_PRIORITY, &rpmsg_recv_task_handle) != pdPASS) {
		os_printf("\r\nFailed to create recv task for RPMsg EPT(%d)!\r\n", ept_addr);
		goto err_rpmsg_create_recv_task;
	}

	if (xTaskCreate(rpmsg_send_task, "RPMsg_send_task", RPMSG_SEND_TASK_STACK_SIZE, ept,
			rpmsg_send_task_PRIORITY, &rpmsg_send_task_handle) != pdPASS) {
		os_printf("\r\nFailed to create send task for RPMsg EPT(%d)!\r\n", ept_addr);
		goto err_rpmsg_create_send_task;
	}

	vTaskSuspend(NULL);

err_rpmsg_create_send_task:
	vTaskDelete(rpmsg_recv_task_handle);
	rpmsg_recv_task_handle = NULL;

err_rpmsg_create_recv_task:
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
