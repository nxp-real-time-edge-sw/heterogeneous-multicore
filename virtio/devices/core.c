/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <string.h>
#include "virtio.h"
#include "virtio_ring.h"
#include "virtio-mmio-transport.h"
#include "os/stdio.h"
#include <os/stdlib.h>

#include "fsl_device_registers.h"

#ifdef VC_DEBUG
#define VC_DBG(fmt, ...)			\
	os_printf(fmt, ##__VA_ARGS__)
#else
#define VC_DBG(fmt, ...)			\
({						\
	if (0)					\
		os_printf(fmt, ##__VA_ARGS__);	\
 })

#endif

void virtio_queue_update_used_idx(struct virtqueue *vq, uint16_t num)
{
	uint16_t idx = vq->vring.used->idx;

	 /* Ensure the id and len has been updated. */
	__DMB();
	idx += num;
	vq->vring.used->idx = idx;
}

void virtio_queue_add_used_elem(struct virtqueue *vq, uint32_t elem_id, uint32_t len, uint16_t off)
{
	struct vring_used_elem *used_elem;
	uint16_t idx = vq->vring.used->idx;

	idx += off;
	used_elem	= &vq->vring.used->ring[idx % vq->vring.num];
	used_elem->id	= elem_id;
	used_elem->len	= len;
}

void virtio_queue_update_used_elem(struct virtqueue *vq, uint32_t elem_id, uint32_t len)
{
	virtio_queue_add_used_elem(vq, elem_id, len, 0);
	virtio_queue_update_used_idx(vq, 1);
}

int virtio_queue_get_iov(struct virtqueue *vq, struct iovec iov[],
			 uint16_t *cnt_out, uint16_t *cnt_in)
{
	uint16_t last_avail_idx = vq->last_avail_idx;
	struct vring_desc *desc = vq->vring.desc;
	uint16_t handle_idx, cur_idx;
	uint16_t flags;

	*cnt_out = 0;
	*cnt_in = 0;

	/* Ensure the available ring has been updated */
	__DMB();

	/* check if there is a available entry */
	if (vq->vring.avail->idx == last_avail_idx)
		return -1;

	handle_idx = vq->vring.avail->ring[vq->last_avail_idx++ % vq->vring.num];
	cur_idx = handle_idx;

	if (desc[cur_idx].flags & VRING_DESC_F_INDIRECT) {
		desc = (struct vring_desc *)(uintptr_t)desc[cur_idx].addr;
		cur_idx = 0;
	}

	while (1) {
		iov[*cnt_out + *cnt_in].addr = (void *)(uintptr_t)desc[cur_idx].addr;
		iov[*cnt_out + *cnt_in].len = desc[cur_idx].len;

		flags = desc[cur_idx].flags;
		if (flags & VRING_DESC_F_WRITE)
			(*cnt_in)++;
		else
			(*cnt_out)++;

		if (!(flags & VRING_DESC_F_NEXT))
			break;

		cur_idx = desc[cur_idx].next;
	}

	if (vq->use_event_idx)
		vring_avail_event(&vq->vring) = vq->last_avail_idx;

	return handle_idx;
}

void virtio_exit_vq(struct virtio_dev *vdev, int num)
{
	struct virtqueue *vq = vdev->ops->get_vq(vdev, num);

	if (vq->enabled && vdev->ops->exit_vq)
		vdev->ops->exit_vq(vdev, num);
	memset(vq, 0, sizeof(*vq));
}

bool virtio_queue_must_notify(struct virtqueue *vq)
{
	uint16_t last_idx, new_idx, event_idx;

	/* Ensure the used idx has been updated before notify the front-end. */
	__DMB();

	if (vq->use_event_idx)
	{
		event_idx	= vring_used_event(&vq->vring);
		new_idx		= vq->vring.used->idx;
		last_idx	= vq->last_notified_idx;

		if (!vring_need_event(event_idx, new_idx, last_idx))
			return false;

		vq->last_notified_idx = new_idx;
		return true;
	}

	return !(vq->vring.avail->flags & VRING_AVAIL_F_NO_INTERRUPT);
}

void virtio_set_drv_features(struct virtio_dev *vdev, uint64_t features)
{
	vdev->drv_features = features;
}

void virtio_handle_dev_status(struct virtio_dev *vdev, uint32_t status)
{
	if (vdev->ops->handle_dev_status)
		vdev->ops->handle_dev_status(vdev, status);

	if (!(vdev->state & VIRTIO_STATE_LIVE)) {
		if (status & VIRTIO_CONFIG_S_DRIVER_OK)
			vdev->state |= VIRTIO_STATE_LIVE;
	} else {
		if (!status) { /* reset */
			vdev->state &= ~VIRTIO_STATE_LIVE;
			vdev->ops->reset(vdev);
		}
	}
}

int virtio_init(struct virtio_dev *vdev, void *mmio_base,
		struct virtio_ops *ops, enum virtio_trans trans,
		int device_id, int subsys_id, int class)
{
	void *virtio;
	int r;

	switch (trans) {
	case VIRTIO_MMIO:
		virtio = os_malloc(sizeof(struct virtio_mmio));
		if (!virtio)
			return -1;
		memset(virtio, 0, sizeof(struct virtio_mmio));

		vdev->virtio			= virtio;
		vdev->ops			= ops;
		vdev->ops->interrupt_front	= virtio_mmio_interrupt_front;
		vdev->ops->init			= virtio_mmio_init;
		vdev->ops->exit			= virtio_mmio_exit;
		vdev->ops->reset		= virtio_mmio_reset;
		r = vdev->ops->init(vdev, mmio_base, device_id, subsys_id, class);
		break;
	default:
		r = -1;
	};

	return r;
}
