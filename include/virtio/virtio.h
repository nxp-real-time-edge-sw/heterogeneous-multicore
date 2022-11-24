/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _VIRTIO_H
#define _VIRTIO_H

#include <stdbool.h>
#include "virtio_ring.h"
#include "virtio_config.h"
#include "common/iovec.h"

#define VIRTIO_CONFIG_S_MASK						\
	(VIRTIO_CONFIG_S_ACKNOWLEDGE | VIRTIO_CONFIG_S_DRIVER |		\
	 VIRTIO_CONFIG_S_FEATURES_OK | VIRTIO_CONFIG_S_DRIVER_OK |	\
	 VIRTIO_CONFIG_S_FAILED)

/* Device live state */
#define VIRTIO_STATE_LIVE		(1)
/* Device feature size */
#define FEATURES_SIZE			(2)

struct virtqueue {
	struct vring	vring;
	uint32_t	pfn;
	uint16_t	last_avail_idx;
	uint16_t	last_notified_idx;
	bool		use_event_idx;
	bool		enabled;
};

void virtio_queue_update_used_idx(struct virtqueue *vq, uint16_t num);
void virtio_queue_add_used_elem(struct virtqueue *vq, uint32_t elem_id, uint32_t len, uint16_t off);
void virtio_queue_update_used_elem(struct virtqueue *queue, uint32_t elem_id,
				   uint32_t len);
int virtio_queue_get_iov(struct virtqueue *vq, struct iovec iov[],
			 uint16_t *cnt_out, uint16_t *cnt_in);
bool virtio_queue_must_notify(struct virtqueue *vq);

enum virtio_trans {
	VIRTIO_MMIO,
};

struct virtio_dev {
	void			*virtio;
	struct virtio_ops	*ops;
	uint64_t		drv_features;
	uint32_t		dev_features[FEATURES_SIZE];
	uint32_t		state;
	void			*devp;
	void			*cfg;
	uint32_t		cfg_size;
	uint32_t		vqs;
};

struct virtio_ops {
	int (*init_vq)(struct virtio_dev *vdev, uint32_t vq, uint32_t page_size,
		       uint32_t align, uint32_t pfn);
	void (*exit_vq)(struct virtio_dev *vdev, uint32_t vq);
	int (*notify_vq)(struct virtio_dev *vdev, uint32_t vq);
	struct virtqueue *(*get_vq)(struct virtio_dev *vdev, uint32_t vq);
	int (*get_vq_size)(struct virtio_dev *vdev, uint32_t vq);
	int (*set_vq_size)(struct virtio_dev *vdev, uint32_t vq, int size);
	void (*handle_dev_status)(struct virtio_dev *vdev, uint32_t status);
	void (*dev_cfg_accessor)(struct virtio_dev *vdev, uint64_t addr, uint32_t len);
	int (*interrupt_front)(struct virtio_dev *vdev, bool is_vring);
	int (*init)(struct virtio_dev *vdev, void *mmio_base,
		    int device_id, int subsys_id, int class);
	int (*exit)(struct virtio_dev *vdev);
	int (*reset)(struct virtio_dev *vdev);
};

int virtio_init(struct virtio_dev *vdev, void *mmio_base,
		struct virtio_ops *ops, enum virtio_trans trans,
		int device_id, int subsys_id, int class);

static inline void *virtio_get_vq(uint32_t pfn, uint32_t page_size)
{
	return (void *)(uintptr_t)(pfn * page_size);
}

static inline void virtio_init_device_vq(struct virtio_dev *vdev,
					 struct virtqueue *vq)
{
	vq->use_event_idx = (vdev->drv_features & (1 << VIRTIO_RING_F_EVENT_IDX));
	vq->enabled = true;
}

void virtio_exit_vq(struct virtio_dev *vdev, int num);
void virtio_set_drv_features(struct virtio_dev *vdev, uint64_t features);
void virtio_handle_dev_status(struct virtio_dev *vdev, uint32_t status);

#endif
