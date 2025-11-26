/*
 * Copyright 2022,2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <unistd.h>
#include <string.h>
#include "virtio.h"
#include "virtio_mmio.h"
#include "virtio_trans.h"
#include "virtio_ring.h"
#include "virtio_ids.h"
#include "os/stdio.h"
#include "os/semaphore.h"
#include <os/cpu_load.h>
#include "FreeRTOS.h"
#include "task.h"

#ifdef VT_DEBUG
#define VT_DBG(fmt, ...)			\
	os_printf(fmt, ##__VA_ARGS__)
#else
#define VT_DBG(fmt, ...)			\
({						\
	if (0)					\
		os_printf(fmt, ##__VA_ARGS__);	\
 })
#endif

#define MAX_VT_BUF_SIZE			2048
#define MAX_INDIRECT_NUM		100

#define VIRTIO_TRANS_QUEUE_SIZE		1024
#define VIRTIO_TRANS_NUM_QUEUES		2
#define VIRTIO_TRANS_RX_QUEUE		0
#define VIRTIO_TRANS_TX_QUEUE		1

#define VIRTIO_TRANS_FEATURES			\
	(1ULL << VIRTIO_RING_F_EVENT_IDX |	\
	 1ULL << VIRTIO_RING_F_INDIRECT_DESC |	\
	 1ULL << VIRTIO_F_ACCESS_PLATFORM)

#define STACK_SIZE			0x400
#define TASK_PRIORITY			(configMAX_PRIORITIES - 2)

struct trans_dev {
	TaskHandle_t			tasks[VIRTIO_TRANS_NUM_QUEUES];
	struct virtio_dev		vdev;
	struct virtqueue		vqs[VIRTIO_TRANS_NUM_QUEUES];
	struct virtio_trans_config	*config;
	uint64_t			features;
	bool				rx_running;
	bool				tx_running;
	bool				front_poll_mode;
	bool				back_poll_mode;
	bool				do_buf_copy;
};

static struct trans_dev tdev = {
	.rx_running	= 0,
	.tx_running	= 0,
	.features	= VIRTIO_TRANS_FEATURES,
};

static char app_buf[MAX_VT_BUF_SIZE];

static os_sem_t send_packet_sig;
static os_sem_t recv_packet_sig;

static struct iovec iov[MAX_INDIRECT_NUM];

static void *vt_memcpy(void *dts, const void *src, uint32_t len)
{
	uint32_t loop;
	uint32_t *d, *s;

	if (!len)
		return dts;

	if ((((uint32_t)(uintptr_t)dts ^ (uint32_t)(uintptr_t)src) &
	    (sizeof(uint32_t) - 1)) != 0)
		return memcpy(dts, src, len);

	d = (uint32_t *)dts;
	s = (uint32_t *)src;

	loop = len / sizeof(uint32_t);

	while (loop--)
		*d++ = *s++;

	return dts;
}

static void tx_task(void *param)
{
	const TickType_t block_time = pdMS_TO_TICKS(1);
	struct virtqueue *vq;
	uint16_t out, in;
	int head;
	int len;
	uint32_t regression = 0;
	TickType_t start_tick = 0, end_tick = 0;
	int i;
	uint32_t msecs;

	vq = param;

	while (1) {
		VT_DBG("%s:%d enter test loop: waiting Semaphore\r\n", __func__, __LINE__);
		/* wait a signal to send packet */
		os_sem_take(&send_packet_sig, 0, OS_SEM_TIMEOUT_MAX);
		VT_DBG("%s:%d get Semaphore\r\n", __func__, __LINE__);

		os_cpu_load_stats();
		start_tick = xTaskGetTickCount();
		regression = tdev.config->regression;
		while (regression) {
			while ((head = virtio_queue_get_iov(vq, iov, &out, &in)) < 0) {
				if (!tdev.back_poll_mode) {
					ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
				} else {
					vTaskDelay(block_time);
				}
			}


			if (tdev.do_buf_copy)
				for (i = 0; i < in; i++)
					vt_memcpy(iov[i].addr, app_buf, iov[i].len);

			len = iov[0].len * in;
			virtio_queue_update_used_elem(vq, (uint16_t)head, len);
			regression -= in;

			if ((!tdev.front_poll_mode) && (virtio_queue_must_notify(vq)))
				tdev.vdev.ops->interrupt_front(&tdev.vdev, true);
		}

		if (!tdev.front_poll_mode)
			tdev.vdev.ops->interrupt_front(&tdev.vdev, true);

		end_tick = xTaskGetTickCount();
		tdev.config->rx_count = tdev.config->regression;
		msecs = (end_tick - start_tick) * portTICK_PERIOD_MS;
		os_cpu_load_stats();
		os_printf("start_tick = %lld; end_tick = %lld; Sending %d packets used %d ms\r\n",
			  start_tick, end_tick, tdev.config->regression, msecs);
	}
}

static void rx_task(void *param)
{
	const TickType_t block_time = pdMS_TO_TICKS(1);
	uint32_t regression = 0;
	struct virtqueue *vq;
	uint16_t out, in;
	int head;
	uint32_t len;
	TickType_t start_tick = 0, end_tick = 0;
	uint32_t msecs;
	int i;

	vq = param;

	while (1) {
		VT_DBG("%s:%d enter test loop: waiting Semaphore\r\n", __func__, __LINE__);
		/* wait a signal to recv packet */
		os_sem_take(&recv_packet_sig, 0, OS_SEM_TIMEOUT_MAX);
		VT_DBG("%s:%d get Semaphore\r\n", __func__, __LINE__);

		regression = tdev.config->regression;
		os_cpu_load_stats();
		start_tick = xTaskGetTickCount();
		while (regression) {
			while ((head = virtio_queue_get_iov(vq, iov, &out, &in)) < 0) {
				if (!tdev.back_poll_mode) {
					ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
				} else {
					vTaskDelay(block_time);
				}
			}

			if (tdev.do_buf_copy)
				for (i = 0; i < out; i++)
					vt_memcpy(app_buf, iov[i].addr, iov[i].len);

			len = iov[0].len * out;
			virtio_queue_update_used_elem(vq, (uint16_t)head, len);
			regression -= out;

			if ((!tdev.front_poll_mode) && (virtio_queue_must_notify(vq)))
				tdev.vdev.ops->interrupt_front(&tdev.vdev, true);
		}

		if (!tdev.front_poll_mode)
			tdev.vdev.ops->interrupt_front(&tdev.vdev, true);

		end_tick = xTaskGetTickCount();
		tdev.config->tx_count = tdev.config->regression;
		msecs = (end_tick - start_tick) * portTICK_PERIOD_MS;
		os_cpu_load_stats();
		os_printf("start_tick = %lld; end_tick = %lld; receiving %d packets used %d ms\r\n",
			  start_tick, end_tick, tdev.config->regression, msecs);
	}
}

static void set_config(struct trans_dev *tdev, void *addr)
{
	tdev->config = addr;

	tdev->config->status		= 0;
	tdev->config->config		= 0;
	tdev->config->control		= 0;
	tdev->config->pkt_size		= 64;
	tdev->config->tx_count		= 0;
	tdev->config->rx_count		= 0;
	tdev->config->regression	= 0;
}

static int init_vq(struct virtio_dev *vdev, uint32_t vq, uint32_t page_size,
		   uint32_t align, uint32_t pfn)
{
	struct trans_dev *tdev = vdev->devp;
	struct virtqueue *queue;
	void *p;

	if (vq >= VIRTIO_TRANS_NUM_QUEUES)
		return -1;

	queue		= &tdev->vqs[vq];
	queue->pfn	= pfn;
	p		= virtio_get_vq(queue->pfn, page_size);

	vring_init(&queue->vring, VIRTIO_TRANS_QUEUE_SIZE, p, align);
	virtio_init_device_vq(vdev, queue);

	return 0;
}

static void exit_vq(struct virtio_dev *vdev, uint32_t vq)
{
}

static int notify_vq(struct virtio_dev *vdev, uint32_t vq)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	struct trans_dev *tdev = vdev->devp;

	if (!tdev->back_poll_mode) {
		vTaskNotifyGiveFromISR(tdev->tasks[vq], &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	return 0;
}

static struct virtqueue *get_vq(struct virtio_dev *vdev, uint32_t vq)
{
	struct trans_dev *tdev = vdev->devp;

	return &tdev->vqs[vq];
}

static int get_vq_size(struct virtio_dev *vdev, uint32_t vq)
{
	return VIRTIO_TRANS_QUEUE_SIZE;
}

static int set_vq_size(struct virtio_dev *vdev, uint32_t vq, int size)
{
	return size;
}

static void dev_cfg_accessor(struct virtio_dev *vdev, uint64_t addr, uint32_t len)
{
	struct trans_dev *tdev = vdev->devp;

	switch(addr) {
	case VT_CONFIG:
		if (tdev->config->config & VT_CFG_TX)
			VT_DBG("%s:%d VT_CONFIG TX\r\n", __func__, __LINE__);
		if (tdev->config->config & VT_CFG_RX)
			VT_DBG("%s:%d VT_CONFIG RX\r\n", __func__, __LINE__);

		tdev->do_buf_copy = false;
		tdev->back_poll_mode = false;
		tdev->front_poll_mode = false;

		if (tdev->config->config & VT_CFG_COPY)
			tdev->do_buf_copy = true;
		if (tdev->config->config & VT_CFG_B_POLL)
			tdev->back_poll_mode = true;
		if (tdev->config->config & VT_CFG_F_POLL)
			tdev->front_poll_mode = true;
		os_printf("Front-end: %s mode\r\n", tdev->front_poll_mode ? "Poll" : "Interrupt");
		os_printf("Back-end:  %s mode\r\n", tdev->back_poll_mode ? "Poll" : "Interrupt");
		os_printf("Packet size: %d Bytes\r\n", tdev->config->pkt_size);
		os_printf("Buffer copy: %s\r\n", tdev->do_buf_copy ? "Yes" : "No");
		break;
	case VT_CONTROL:
		if (tdev->config->control & VT_CTRL_START) {
			if ((tdev->config->config & VT_CFG_TX) && !tdev->rx_running) {
				VT_DBG("%s:%d VT_CONTROL rx_task start\r\n",
				       __func__, __LINE__);
				tdev->rx_running = true;
				tdev->config->tx_count = 0;
				os_sem_give(&recv_packet_sig, OS_SEM_FLAGS_ISR_CONTEXT);
			}
			if ((tdev->config->config & VT_CFG_RX) && !tdev->tx_running) {
				VT_DBG("%s:%d VT_CONTROL tx_task start\r\n",
				       __func__, __LINE__);

				tdev->tx_running = true;
				tdev->config->rx_count = 0;
				os_sem_give(&send_packet_sig, OS_SEM_FLAGS_ISR_CONTEXT);
			}
		} else {
			if ((tdev->config->config & VT_CFG_TX) && tdev->rx_running) {
				VT_DBG("%s:%d VT_CONTROL rx_task stop\r\n",
				       __func__, __LINE__);
				tdev->rx_running = false;
			}
			if ((tdev->config->config & VT_CFG_RX) && tdev->tx_running) {
				VT_DBG("%s:%d VT_CONTROL tx_task stop\r\n",
				       __func__, __LINE__);
				tdev->tx_running = false;
			}
		}

		break;
	case VT_REGRESSION:
		VT_DBG("Updated regression to %d\r\n", tdev->config->regression);
		break;
	default:
		return;
	}
}

static struct virtio_ops trans_dev_virtio_ops = {
	.dev_cfg_accessor	= dev_cfg_accessor,
	.init_vq		= init_vq,
	.exit_vq		= exit_vq,
	.notify_vq		= notify_vq,
	.get_vq			= get_vq,
	.get_vq_size		= get_vq_size,
	.set_vq_size		= set_vq_size,
};

static int virtio_trans_create_tasks(struct trans_dev *tdev)
{
	if (0 != os_sem_init(&send_packet_sig, 0))
		return -1;

	if (0 != os_sem_init(&recv_packet_sig, 0))
		return -1;

	if (pdPASS != xTaskCreate(rx_task, "rx_task", STACK_SIZE,
	    &tdev->vqs[VIRTIO_TRANS_RX_QUEUE], TASK_PRIORITY - 1,
	    &tdev->tasks[VIRTIO_TRANS_RX_QUEUE])) {
		os_printf("rx_task creation failed!\r\n");
		return -1;
	} else {
		VT_DBG("rx_task creation succeed!\r\n");
	}

	if (pdPASS != xTaskCreate(tx_task, "tx_task", STACK_SIZE,
	    &tdev->vqs[VIRTIO_TRANS_TX_QUEUE], TASK_PRIORITY,
	    &tdev->tasks[VIRTIO_TRANS_TX_QUEUE])) {
		os_printf("tx_task creation failed!\r\n");
		return -1;
	} else {
		VT_DBG("tx_task creation succeed!\r\n");
	}

	return 0;
}

static int virtio_trans_dev_init(struct trans_dev *tdev, void *mmio_base)
{
	struct virtio_dev *vdev = &tdev->vdev;

	set_config(tdev, mmio_base + VIRTIO_MMIO_CONFIG);

	vdev->cfg= (void *)tdev->config;
	vdev->cfg_size = sizeof(struct virtio_trans_config);
	vdev->devp = tdev;
	vdev->vqs = VIRTIO_TRANS_NUM_QUEUES;

	vdev->dev_features[0] = tdev->features;
	vdev->dev_features[1] = tdev->features >> 32;

	return virtio_init(vdev, mmio_base, &trans_dev_virtio_ops,
			   VIRTIO_MMIO, 1111, VIRTIO_ID_TRANS, 0);
}

int virtio_trans_init(void *mmio_base)
{
	int r;

	r = virtio_trans_dev_init(&tdev, mmio_base);
	if (r) {
		os_printf("virtio initialization failed!\r\n");
		goto err;
	}

	r = virtio_trans_create_tasks(&tdev);
	if (r) {
		os_printf("virtio trans create tasks failed!\r\n");
		return r;
	}

	return 0;
err:
	return r;
}

int virtio_trans_exit(void)
{
	return 0;
}
