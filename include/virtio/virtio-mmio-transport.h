/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _VIRTIO_MMIO_H
#define _VIRTIO_MMIO_H

#include <stdint.h>
#include "virtio.h"
#include "virtio_mmio.h"

#define VIRTIO_MMIO_IO_SIZE	0x200

struct virtio_mmio_hdr {
	char		magic[4];
	uint32_t	version;
	uint32_t	device_id;
	uint32_t	vendor_id;
	uint32_t	dev_features;
	uint32_t	dev_features_sel;
	uint32_t	reserved_1[2];
	uint32_t	drv_features;
	uint32_t	drv_features_sel;
	uint32_t	guest_page_size;
	uint32_t	reserved_2;
	uint32_t	queue_sel;
	uint32_t	queue_num_max;
	uint32_t	queue_num;
	uint32_t	queue_align;
	uint32_t	queue_pfn;
	uint32_t	reserved_3[3];
	uint32_t	queue_notify;
	uint32_t	reserved_4[3];
	uint32_t	interrupt_status;
	uint32_t	interrupt_ack;
	uint32_t	reserved_5[2];
	uint32_t	status;
	uint32_t	wd_status;
} __attribute__((packed));

struct virtio_mmio {
	struct virtio_mmio_hdr	*hdr;
	uint32_t		mmio;
};

int virtio_mmio_interrupt_front(struct virtio_dev *vdev, bool is_vring);
int virtio_mmio_exit(struct virtio_dev *vdev);
int virtio_mmio_reset(struct virtio_dev *vdev);
int virtio_mmio_init(struct virtio_dev *vdev, void *mmio_base,
		     int device_id, int subsys_id, int class);
#endif
