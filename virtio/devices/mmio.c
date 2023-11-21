/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include "virtio.h"
#include "virtio-mmio-transport.h"
#include "os/stdio.h"
#include "mailbox.h"
#include "app_virtio_config.h"

#ifdef VM_DEBUG
#define VM_DBG(fmt, ...)			\
	os_printf(fmt, ##__VA_ARGS__)
#else
#define VM_DBG(fmt, ...)			\
({						\
	if (0)					\
		os_printf(fmt, ##__VA_ARGS__);	\
 })
#endif

int virtio_mmio_interrupt_front(struct virtio_dev *vdev, bool is_vring)
{
	struct virtio_mmio *vmmio = vdev->virtio;
	uint32_t status = vmmio->hdr->interrupt_status;
	uint32_t ack = vmmio->hdr->interrupt_ack;

	/* handle the last ACK */
	status &= ~ack;

	if (is_vring)
		status |= VIRTIO_MMIO_INT_VRING;
	else
		status |= VIRTIO_MMIO_INT_CONFIG;

	vmmio->hdr->interrupt_status = status;
	mbox_kick_front(vmmio->mmio);

	return 0;
}

static void virtio_mmio_exit_vq(struct virtio_dev *vdev, int vq)
{
	virtio_exit_vq(vdev, vq);
}

static void virtio_mmio_device_specific(uint64_t addr, uint32_t len,
					struct virtio_dev *vdev)
{
	size_t config_size;

	config_size = vdev->cfg_size;
	if (addr + len <= config_size)
		vdev->ops->dev_cfg_accessor(vdev, addr, len);
}

static void virtio_mmio_config_out(uint64_t addr, uint32_t len,
				   struct virtio_dev *vdev)
{
	struct virtio_mmio *vmmio = vdev->virtio;
	unsigned int vq_count = vdev->vqs;
	uint32_t val = 0;

	switch (addr) {
	case VIRTIO_MMIO_HOST_FEATURES_SEL:
		val = vmmio->hdr->dev_features_sel;
		VM_DBG("%s: dev_features_sel has been set to %d\r\n", __func__, val);
		if (val < FEATURES_SIZE) {
			vmmio->hdr->dev_features = vdev->dev_features[val];
			VM_DBG("%s: Updated dev_features[%d] to 0x%x\r\n",
			       __func__, val, vmmio->hdr->dev_features);
		} else {
			vmmio->hdr->dev_features_sel = 0;
			vmmio->hdr->dev_features = vdev->dev_features[0];
			os_printf("Invalid value, reset dev_features_sel to 0\r\n");
		}

		break;
	case VIRTIO_MMIO_GUEST_FEATURES_SEL:
		break;
	case VIRTIO_MMIO_QUEUE_SEL:
		val = vmmio->hdr->queue_sel;
		if (val >= vq_count) {
			os_printf("QUEUE_SEL value (%u) is larger than VQ count (%u)\r\n",
				  val, vq_count);
			val = vmmio->hdr->queue_sel = 0;
		}
		struct virtqueue *vq;
		vq = vdev->ops->get_vq(vdev, val);
		vmmio->hdr->queue_pfn = vq->pfn;
		vmmio->hdr->queue_num_max = vdev->ops->get_vq_size(vdev, val);

		break;
	case VIRTIO_MMIO_STATUS:
		virtio_handle_dev_status(vdev, vmmio->hdr->status);
		break;
	case VIRTIO_MMIO_GUEST_FEATURES:
		if (vmmio->hdr->drv_features_sel == 0) {
			val = vmmio->hdr->drv_features;
			virtio_set_drv_features(vdev, val);
		}
		break;
	case VIRTIO_MMIO_GUEST_PAGE_SIZE:
		break;
	case VIRTIO_MMIO_QUEUE_NUM:
		val = vmmio->hdr->queue_num;
		vdev->ops->set_vq_size(vdev, vmmio->hdr->queue_sel, val);
		break;
	case VIRTIO_MMIO_QUEUE_ALIGN:
		break;
	case VIRTIO_MMIO_QUEUE_PFN:
		val = vmmio->hdr->queue_pfn;
		if (val)
			vdev->ops->init_vq(vdev,
					   vmmio->hdr->queue_sel,
					   vmmio->hdr->guest_page_size,
					   vmmio->hdr->queue_align,
					   val);
		else
			virtio_mmio_exit_vq(vdev, vmmio->hdr->queue_sel);
		break;
	case VIRTIO_MMIO_QUEUE_NOTIFY:
		val = vmmio->hdr->queue_notify;
		if (val >= vq_count) {
			os_printf("QUEUE_NOTIFY value (%u) is larger than VQ count (%u)\r\n",
				  val, vq_count);
			break;
		}
		vdev->ops->notify_vq(vdev, val);
		break;
	case VIRTIO_MMIO_INTERRUPT_ACK:
		val = vmmio->hdr->interrupt_ack;
		vmmio->hdr->interrupt_status &= ~val;
		break;
	default:
		break;
	};
}

static void virtio_mmio_mmio_callback(uintptr_t addr, uint32_t len, void *ptr)
{
	struct virtio_dev *vdev = ptr;
	struct virtio_mmio *vmmio = vdev->virtio;
	uint32_t offset = addr - vmmio->mmio;

	if (offset >= VIRTIO_MMIO_CONFIG) {
		offset -= VIRTIO_MMIO_CONFIG;
		virtio_mmio_device_specific(offset, len, ptr);
		goto out;
	}

	virtio_mmio_config_out(offset, len, ptr);
out:
	vmmio->hdr->wd_status = 1;
}

int virtio_mmio_init(struct virtio_dev *vdev, void *mmio_base,
		     int device_id, int subsys_id, int class)
{
	struct virtio_mmio *vmmio = vdev->virtio;

	vmmio->mmio = (uint32_t)(uintptr_t)mmio_base;

	mbox_interrupt_disable(vmmio->mmio);
	mbox_init(false);
	mbox_register_callback(vmmio->mmio, virtio_mmio_mmio_callback, vdev);
	mbox_interrupt_enable(vmmio->mmio);

	vmmio->hdr		= mmio_base;
	vmmio->hdr->magic[0]	= 'v';
	vmmio->hdr->magic[1]	= 'i';
	vmmio->hdr->magic[2]	= 'r';
	vmmio->hdr->magic[3]	= 't';
	vmmio->hdr->version	= 1;
	vmmio->hdr->device_id	= subsys_id;
	vmmio->hdr->vendor_id	= 0x1957; /* NXP */
	vmmio->hdr->queue_num_max = vdev->ops->get_vq_size(vdev, 0);
	vmmio->hdr->dev_features  = vdev->dev_features[0];

	VM_DBG("virtio-mmio.devices=0x%x@0x%x\r\n",
	       VIRTIO_MMIO_IO_SIZE, vmmio->mmio);

	return 0;
}

int virtio_mmio_reset(struct virtio_dev *vdev)
{
	unsigned int vq;

	for (vq = 0; vq < vdev->vqs; vq++)
		virtio_mmio_exit_vq(vdev, vq);

	return 0;
}

int virtio_mmio_exit(struct virtio_dev *vdev)
{
	virtio_mmio_reset(vdev);

	return 0;
}
