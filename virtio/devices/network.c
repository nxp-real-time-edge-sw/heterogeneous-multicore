/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <unistd.h>
#include <string.h>
#include <os/mqueue.h>
#include <os/stdio.h>
#include <os/stdlib.h>
#include <os/semaphore.h>
#include <net/data_pkt.h>
#include <net/net_switch.h>
#include "common/util.h"
#include "virtio.h"
#include "virtio_mmio.h"
#include "virtio_ring.h"
#include "virtio_ids.h"
#include "virtio_net.h"
#include "virtio-net.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef VN_DEBUG
#define VN_DBG(fmt, ...)			\
	os_printf(fmt, ##__VA_ARGS__)
#else
#define VN_DBG(fmt, ...)			\
({						\
	if (0)					\
		os_printf(fmt, ##__VA_ARGS__);	\
 })
#endif

#define VIRTIO_NET_QUEUE_SIZE		1024
#define VIRTIO_NET_NUM_QUEUES		1

#define STACK_SIZE			0x1000
#define TASK_PRIORITY			(configMAX_PRIORITIES - 3)

#define VNET_COAL_CNT			100
#define VNET_KICK_PERIOD		1

enum net_dev_status {
	VNET_RUN,
	VNET_STOP,
};

struct net_dev;

struct net_dev_queue {
	int				id;
	struct net_dev			*ndev;
	struct virtqueue		vq;
	TaskHandle_t			task;
	os_sem_t			sig;
};

struct net_dev {
	struct virtio_dev		vdev;
	struct virtio_net_config	*config;
	uint64_t			features;
	enum net_dev_status		status;
	bool				link_on;

	struct net_dev_queue		queues[VIRTIO_NET_NUM_QUEUES * 2 + 1];
	uint32_t			queue_pairs;

	void				*switch_dev;
	void				*port;
	size_t				hdr_len;

	TaskHandle_t			kick_task;
	os_sem_t			kick_sig;
};

static struct vnet_priv_stats {
	uint64_t rx_pkts;
	uint64_t tx_pkts;
} priv_stats;

static uint8_t default_mac[6] = {0x00, 0x04, 0x9f, 0x00, 0x01, 0x02};

static bool has_virtio_feature(struct net_dev *ndev, uint32_t feature)
{
	return ndev->features & (1ULL << feature);
}

static int virtio_net_hdr_len(struct net_dev *ndev)
{
	if (has_virtio_feature(ndev, VIRTIO_NET_F_MRG_RXBUF))
		return sizeof(struct virtio_net_hdr_mrg_rxbuf);

	return sizeof(struct virtio_net_hdr);
}

static void virtio_net_tx_task(void *p)
{
	//coverity[stack_use : FALSE ]
	struct iovec iov[VIRTIO_NET_QUEUE_SIZE];
	struct net_dev_queue *queue = p;
	struct virtqueue *vq = &queue->vq;
	struct net_dev *ndev = queue->ndev;
	struct switch_port *port = ndev->port;
	uint16_t out, in;
	int head;
	int len;
	int i;
	uint64_t tx_cnt = 0;

	os_sem_take(&queue->sig, 0, OS_SEM_TIMEOUT_MAX);
	VN_DBG("%s: TX task started\r\n", __func__);

	while (1) {
		while ((head = virtio_queue_get_iov(vq, iov, &out, &in)) < 0) {
			ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

			/* TODO: remove this workaround */
			/* delay here to reduce frontend notification */
			vTaskDelay(1);
		}

		len = iov_size(iov, out);

		/* If link is off, discard the packet directly */
		if (!cable_link_is_on(&port->cable))
			goto out;

		/*
		 * Currently, the buffer merge feature is not supported,
		 * so the iov[0] must be the virtio header, and do NOT
		 * send the virtio header to switch port.
		 */
		for (i = 1; i < out; i++)
			port_in_pkt(port, iov[i].addr, iov[i].len, false);
		notify_ports(port->switch_dev);
		priv_stats.tx_pkts++;
		tx_cnt++;
out:
		virtio_queue_update_used_elem(vq, (uint16_t)head, len);

		if (virtio_queue_must_notify(vq)) {
			if (tx_cnt >= VNET_COAL_CNT) {
				ndev->vdev.ops->interrupt_front(&ndev->vdev, true);
			} else {
				os_sem_give(&ndev->kick_sig, 0);
			}
			tx_cnt = 0;
		}
	}
}

static void virtio_net_ctrl_task(void *p)
{
	//coverity[stack_use : FALSE ]
	struct iovec iov[VIRTIO_NET_QUEUE_SIZE];
	struct net_dev_queue *queue = p;
	struct virtqueue *vq = &queue->vq;
	struct net_dev *ndev = queue->ndev;
	uint16_t out, in;
	int head;
	struct virtio_net_ctrl_hdr ctrl;
	virtio_net_ctrl_ack ack;
	size_t len;

	os_sem_take(&queue->sig, 0, OS_SEM_TIMEOUT_MAX);
	VN_DBG("%s: Ctrl task started\r\n", __func__);

	while (1) {
		while ((head = virtio_queue_get_iov(vq, iov, &out, &in)) < 0) {
			ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		}

		len = min(iov_size(iov, 1), sizeof(ctrl));
		memcpy_fromiovec((void *)&ctrl, iov, len);

		switch (ctrl.class) {
		case VIRTIO_NET_CTRL_MAC:
			ack = VIRTIO_NET_ERR;
			if (ctrl.cmd == VIRTIO_NET_CTRL_MAC_ADDR_SET) {
				struct switch_port *port = ndev->port;
				uint8_t mac[6];

				memcpy_fromiovec((void *)mac, iov + 1, 6);
				port_update_addr(port, mac);
				ack = VIRTIO_NET_OK;
			}
			break;
		case VIRTIO_NET_CTRL_MQ:
			ack = VIRTIO_NET_OK;
			break;
		default:
			ack = VIRTIO_NET_ERR;
			break;
		}

		memcpy_toiovec(iov + out, &ack, sizeof(ack));
		virtio_queue_update_used_elem(vq, (uint16_t)head, sizeof(ack));

		if (virtio_queue_must_notify(vq))
			ndev->vdev.ops->interrupt_front(&ndev->vdev, true);
	}
}

static void virtio_net_kick_task(void *p)
{
	const TickType_t kick_tick = pdMS_TO_TICKS(VNET_KICK_PERIOD) ? pdMS_TO_TICKS(VNET_KICK_PERIOD) : 1;
	struct net_dev *ndev = p;

	VN_DBG("%s: Kick task started\r\n", __func__);

	while (1) {
		os_sem_take(&ndev->kick_sig, 0, OS_SEM_TIMEOUT_MAX);
		VN_DBG("%s: Get kick semophore\r\n", __func__);
		ndev->vdev.ops->interrupt_front(&ndev->vdev, true);
		vTaskDelay(kick_tick);
	}
}

static uint64_t get_host_features(struct net_dev *ndev)
{
	uint64_t features;

	features = 1UL << VIRTIO_NET_F_MAC
		| 1UL << VIRTIO_RING_F_EVENT_IDX
		| 1UL << VIRTIO_RING_F_INDIRECT_DESC
		| 1UL << VIRTIO_NET_F_CTRL_VQ
		| 1UL << VIRTIO_NET_F_CTRL_MAC_ADDR
		| 1ULL << VIRTIO_F_ACCESS_PLATFORM;

	if (ndev->queue_pairs > 1)
		features |= 1UL << VIRTIO_NET_F_MQ;

	return features;
}

static void virtio_net_update_status(struct net_dev *ndev)
{
	struct virtio_net_config *conf = ndev->config;

	conf->status = VIRTIO_NET_S_LINK_UP;
	if (ndev->status == VNET_RUN)
		ndev->link_on = true;
}

static int virtio_net_port_add(struct net_dev *ndev);

static void virtio_net_start(struct net_dev *ndev)
{
	struct virtio_net_config *conf = ndev->config;

	VN_DBG("%s: Set ndev->status to VNET_RUN\r\n", __func__);
	ndev->status = VNET_RUN;
	if (conf->status == VIRTIO_NET_S_LINK_UP)
		ndev->link_on = true;
}

static void virtio_net_stop(struct net_dev *ndev)
{
	VN_DBG("%s: Set ndev->status to VNET_STOP\r\n", __func__);
	ndev->status = VNET_STOP;
	ndev->link_on = false;
}

static void handle_dev_status(struct virtio_dev *vdev, uint32_t status)
{
	struct net_dev *ndev = vdev->devp;

	if (!status)
		virtio_net_update_status(ndev);

	if ((status & VIRTIO_CONFIG_S_DRIVER_OK) && !(vdev->state & VIRTIO_STATE_LIVE))
		virtio_net_start(ndev);
	else if (!status && (vdev->state & VIRTIO_STATE_LIVE))
		virtio_net_stop(ndev);
}

static int init_vq(struct virtio_dev *vdev, uint32_t vq, uint32_t page_size,
		   uint32_t align, uint32_t pfn)
{
	struct net_dev_queue *net_queue;
	struct net_dev *ndev = vdev->devp;
	struct virtqueue *queue;
	void *p;

	net_queue	= &ndev->queues[vq];
	queue		= &net_queue->vq;

	queue->pfn	= pfn;
	p		= virtio_get_vq(queue->pfn, page_size);
	vring_init(&queue->vring, VIRTIO_NET_QUEUE_SIZE, p, align);

	virtio_init_device_vq(vdev, queue);

	/* No RX virtqueue task to start */
	if ((vq & 1) == 0 && vq != (2 * ndev->queue_pairs))
		return 0;

	/* start virtqueue task */
	os_sem_give(&net_queue->sig, OS_SEM_FLAGS_ISR_CONTEXT);

	return 0;
}

static int virtio_net_create_tasks(struct net_dev *ndev)
{
	struct net_dev_queue *net_queue;
	int i;

	for (i = 0; i < ndev->queue_pairs; i++) {
		net_queue = &ndev->queues[2 * i + 1];
		if (pdPASS != xTaskCreate(virtio_net_tx_task, "net_tx_task", STACK_SIZE,
				net_queue, TASK_PRIORITY, &net_queue->task)) {
			os_printf("tx_task creation failed!\r\n");
			return -1;
		} else {
			VN_DBG("tx_task creation succeed!\r\n");
		}

	}
	net_queue = &ndev->queues[2 * i];
	if (pdPASS != xTaskCreate(virtio_net_ctrl_task, "net_ctrl_task", STACK_SIZE,
			net_queue, TASK_PRIORITY, &net_queue->task)) {
		os_printf("ctrl_task creation failed!\r\n");
		return -1;
	} else {
		VN_DBG("ctrl_task creation succeed!\r\n");
	}

	if (pdPASS != xTaskCreate(virtio_net_kick_task, "net_kick_task",
	    configMINIMAL_STACK_SIZE + 100, ndev, TASK_PRIORITY, &ndev->kick_task)) {
		os_printf("kick_task creation failed!\r\n");
		return -1;
	} else {
		VN_DBG("kick_task creation succeed!\r\n");
	}

	return 0;
}

static void exit_vq(struct virtio_dev *vdev, uint32_t vq)
{
}

static int notify_vq(struct virtio_dev *vdev, uint32_t vq)
{
	struct net_dev *ndev = vdev->devp;
	struct net_dev_queue *net_queue = &ndev->queues[vq];
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if ((vq & 1) == 0 && vq != (2 * ndev->queue_pairs))
		return 0;

	vTaskNotifyGiveFromISR(net_queue->task, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	return 0;
}

static struct virtqueue *get_vq(struct virtio_dev *vdev, uint32_t vq)
{
	struct net_dev *ndev = vdev->devp;

	return &ndev->queues[vq].vq;
}

static int get_vq_size(struct virtio_dev *vdev, uint32_t vq)
{
	return VIRTIO_NET_QUEUE_SIZE;
}

static int set_vq_size(struct virtio_dev *vdev, uint32_t vq, int size)
{
	return size;
}

static void dev_cfg_accessor(struct virtio_dev *vdev, uint64_t addr, uint32_t len)
{
}

static struct virtio_ops net_dev_virtio_ops = {
	.dev_cfg_accessor	= dev_cfg_accessor,
	.init_vq		= init_vq,
	.exit_vq		= exit_vq,
	.get_vq			= get_vq,
	.get_vq_size		= get_vq_size,
	.set_vq_size		= set_vq_size,
	.notify_vq		= notify_vq,
	.handle_dev_status	= handle_dev_status,
};

static void set_config(struct net_dev *ndev, void *addr)
{
	int i;
	ndev->config = addr;

	ndev->config->max_virtqueue_pairs = ndev->queue_pairs;
	for (i = 0 ; i < 6 ; i++) {
		ndev->config->mac[i] = default_mac[i];
	}
}

static int virtio_net_queue_pre_init(struct net_dev *ndev)
{
	int i;

	for (i = 0; i < ndev->vdev.vqs; i++) {
		ndev->queues[i].id = i;
		ndev->queues[i].ndev = ndev;
		if (0 != os_sem_init(&ndev->queues[i].sig, 0))
			return -1;
	}

	return 0;
}

static int virtio_net_dev_init(struct net_dev *ndev, void *mmio_base)
{
	struct virtio_dev *vdev = &ndev->vdev;
	int r;

	vdev->cfg= (void *)ndev->config;
	vdev->cfg_size = sizeof(struct virtio_net_config);
	vdev->devp = ndev;
	vdev->vqs = ndev->queue_pairs * 2 + 1;

	vdev->dev_features[0] = ndev->features;
	vdev->dev_features[1] = ndev->features >> 32;

	if (0 != os_sem_init(&ndev->kick_sig, 0)) {
		os_printf("%s: kick_sig init failed\r\n", __func__);
		return -1;
	}

	r = virtio_net_queue_pre_init(ndev);
	if (r < 0) {
		os_printf("%s: virtio_net_queue_pre_init() failed\r\n", __func__);
		return r;
	}

	r = virtio_init(vdev, mmio_base, &net_dev_virtio_ops, VIRTIO_MMIO,
			0, VIRTIO_ID_NET, 0);
	if (r < 0) {
		os_printf("%s: virtio_init() failed\r\n", __func__);
		return r;
	}

	return 0;
}

static struct net_dev_queue *virtio_net_get_rx_queue(struct net_dev *ndev)
{
	return &ndev->queues[0];
}

static int virtio_net_rx_callback(struct switch_port *port, void *data_paket)
{
	struct net_dev *ndev = (struct net_dev *)port->priv;
	struct net_dev_queue *queue = virtio_net_get_rx_queue(ndev);
	struct data_pkt *pkt = data_paket;
	struct virtqueue *vq = &queue->vq;
	uint16_t num_buffers;
	struct iovec iov[1];
	uint16_t out, in;
	int len, copied;
	uint8_t *buffer;
	int head;
	static uint64_t rx_cnt;

	if (ndev->status != VNET_RUN)
		return 0;

	buffer = pkt->data - ndev->hdr_len;
	if ((uintptr_t)buffer < (uintptr_t)pkt) {
		os_printf("%s: data_pkt header room less than virtio hdr_len\r\n", __func__);
		return -1;
	}

	len = pkt->data_len + ndev->hdr_len;

	copied = num_buffers = 0;
	do {
		size_t iovsize;
		bool wait_flag = false;

		while ((head = virtio_queue_get_iov(vq, iov, &out, &in)) < 0) {
			vTaskDelay(1);
			wait_flag = true;
		}

		if (wait_flag) {
			/* TODO: remove this workaround */
			/* delay here to reduce frontend notification */
			vTaskDelay(1);
			wait_flag = false;
		}

		iovsize = min_t(size_t, len - copied, iov_size(iov, in));

		memcpy_toiovec(iov, buffer + copied, iovsize);
		copied += iovsize;
		virtio_queue_add_used_elem(vq, head, iovsize, num_buffers++);
	} while (copied < len);

	priv_stats.rx_pkts++;
	rx_cnt++;

	virtio_queue_update_used_idx(vq, num_buffers);
	if (virtio_queue_must_notify(vq)) {
		if (rx_cnt >= VNET_COAL_CNT) {
			ndev->vdev.ops->interrupt_front(&ndev->vdev, true);
		} else {
			os_sem_give(&ndev->kick_sig, 0);
		}
		rx_cnt = 0;
	}

	return 0;
}

static int virtio_net_port_init(struct net_dev *ndev)
{
	void *switch_dev = ndev->switch_dev;
	struct switch_port *port;

	port = switch_init_port(switch_dev);
	port->priv = ndev;
	ndev->port = port;
	virtio_net_port_add(ndev);

	return 0;
}

static void print_priv_stats(void)
{
	os_printf("[Virtio_Net_Backend]:\r\t\t\tRX packets\t%llu\tTX packets\t%llu\r\n",
			priv_stats.rx_pkts,
			priv_stats.tx_pkts);
}

static int virtio_net_port_add(struct net_dev *ndev)
{
	struct switch_port *port = ndev->port;
	struct port_config config;
	int i;

	memset(&config, 0, sizeof(struct port_config));
	config.is_local = true;
	config.port_out_cb = virtio_net_rx_callback;
	config.print_priv_stats = print_priv_stats;
	for (i = 0 ; i < 6 ; i++) {
		config.mac_addr[i] = ndev->config->mac[i];
	}
	config.dev_end_on = &ndev->link_on;
	strlcpy(config.name, "VirtIO_Switch_Port", sizeof(config.name));
	switch_add_port(port, &config);

	return 0;
}

int virtio_net_init(void *mmio_base, void *switch_dev)
{
	int r;
	struct net_dev *ndev;

	ndev = os_malloc(sizeof(struct net_dev));
	if (ndev == NULL) {
		os_printf("%s: Failed to malloc net_dev structure\r\n", __func__);
		return -1;
	}
	memset(ndev, 0, sizeof(struct net_dev));

	ndev->status = VNET_STOP;
	ndev->link_on = false;
	ndev->switch_dev = switch_dev;
	ndev->queue_pairs = VIRTIO_NET_NUM_QUEUES;
	ndev->features = get_host_features(ndev);
	ndev->hdr_len = virtio_net_hdr_len(ndev);
	set_config(ndev, mmio_base + VIRTIO_MMIO_CONFIG);

	r = virtio_net_port_init(ndev);
	if (r < 0) {
		os_printf("%s: virtio_net_port_init() failed\r\n", __func__);
		goto ndev_port_err;
	}

	r = virtio_net_dev_init(ndev, mmio_base);
	if (r < 0) {
		os_printf("%s: virtio_net_dev_init() failed\r\n", __func__);
		goto ndev_init_err;
	}

	r = virtio_net_create_tasks(ndev);
	if (r < 0) {
		os_printf("%s: virtio_net_create_tasks() failed\r\n", __func__);
		goto ndev_task_err;
	}

	return 0;

ndev_task_err:
	//TODO: deinit
ndev_init_err:
	//TODO: port_deinit
ndev_port_err:
	os_free(ndev);

	return r;
}

int virtio_net_exit(void *mmio_base, void *bridge)
{
	return 0;
}
