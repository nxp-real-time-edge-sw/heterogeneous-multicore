/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/ipm.h>
#include <openamp/open_amp.h>
#include <metal/sys.h>
#include <metal/io.h>
#include <resource_table.h>
#include <os/stdio.h>
#include "common/iovec.h"
#include "app_board.h"
#include "app_rpmsg.h"
#include "log/log.h"

#ifdef CONFIG_MMU
#include <os/mmu.h>
#endif

#define SHM_NODE			DT_CHOSEN(zephyr_ipc_shm)
#define SHM_START_ADDR			DT_REG_ADDR(SHM_NODE)
#define SHM_SIZE			DT_REG_SIZE(SHM_NODE)

#define RPMSG_INIT_TASK_STACK_SIZE	(1024)
#define RPMSG_NOTIFY_TASK_STACK_SIZE	(1024)
#define RPMSG_EPT_TASK_STACK_SIZE	(1536)

K_THREAD_STACK_DEFINE(thread_rpmsg_init_stack, RPMSG_INIT_TASK_STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_notify_stack, RPMSG_NOTIFY_TASK_STACK_SIZE);
K_THREAD_STACK_ARRAY_DEFINE(thread_ept_stack, RPMSG_EPT_NUM, RPMSG_EPT_TASK_STACK_SIZE);

struct rpmsg_ept_priv {
	const char *sn;
	uint32_t addr;
	struct rpmsg_endpoint ept;
	struct k_sem data_sem;
	struct iovec msg;
	struct k_thread thread_data;
};

static struct rpmsg_ept_priv ept_priv[RPMSG_EPT_NUM];

static const struct device *const ipm_handle = DEVICE_DT_GET(DT_CHOSEN(zephyr_ipc));

static metal_phys_addr_t shm_physmap = SHM_START_ADDR;
static struct metal_io_region shm_io_data;
static struct metal_io_region *shm_io = &shm_io_data;

static metal_phys_addr_t rsc_tab_physmap;
static struct metal_io_region rsc_io_data;
static struct metal_io_region *rsc_io = &rsc_io_data;
static void *rsc_table;

static struct rpmsg_virtio_device rvdev;
static struct rpmsg_device *rpdev;

static struct k_thread thread_rpmsg_init_data;
static struct k_thread thread_notify_data;
static K_SEM_DEFINE(notify_sem, 0, 1);
static uint32_t notify_msg;

static void ipm_callback(const struct device *dev, void *context,
			 uint32_t id, volatile void *data)
{
	log_debug("%s: msg received from mbox chan: %d\n", __func__, id);

	if (data)
		notify_msg = *(uint32_t *)data;

	k_sem_give(&notify_sem);
}

static void rpmsg_notify_task(void *arg1, void *arg2, void *arg3)
{
	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

	uint32_t vring_id;

	for (;;) {
		k_sem_take(&notify_sem, K_FOREVER);

		vring_id = notify_msg >> 16;

		rproc_virtio_notified(rvdev.vdev, vring_id);
	}
}

static int rpmsg_ept_recv_callback(struct rpmsg_endpoint *ept, void *data,
				   size_t len, uint32_t src, void *priv)
{
	struct rpmsg_ept_priv *ept_priv = (struct rpmsg_ept_priv *)priv;
	struct iovec *msg = &ept_priv->msg;

	rpmsg_hold_rx_buffer(ept, data);
	msg->addr = data;
	msg->len = len;
	k_sem_give(&ept_priv->data_sem);

	return RPMSG_SUCCESS;
}

static void rpmsg_ns_callback(struct rpmsg_device *rdev, const char *name, uint32_t src)
{
	log_err("%s: unexpected ns service receive for name %s\n", __func__, name);
}

static int rpmsg_notify_callback(void *priv, uint32_t id)
{
	ARG_UNUSED(priv);

	uint32_t vring_id = id << 16;

	log_debug("%s: notify vring_id %d\r\n", __func__, id);
	ipm_send(ipm_handle, 0, 0, &vring_id, sizeof(id));

	return 0;
}

static int platform_init(void)
{
	struct metal_init_params metal_params = METAL_INIT_DEFAULTS;
	int rsc_size;
	int status;

	status = metal_init(&metal_params);
	if (status) {
		log_err("metal_init: failed: %d\n", status);
		return -1;
	}

	/* declare shared memory region */
	metal_io_init(shm_io, (void *)SHM_START_ADDR, &shm_physmap,
		      SHM_SIZE, -1, 0, NULL);

	/* declare resource table region */
	rsc_table_get(&rsc_table, &rsc_size);
	rsc_tab_physmap = (uintptr_t)rsc_table;
	metal_io_init(rsc_io, rsc_table, &rsc_tab_physmap, rsc_size, -1, 0, NULL);

	if (!device_is_ready(ipm_handle)) {
		log_err("IPM device is not ready\n");
		return -1;
	}

	ipm_register_callback(ipm_handle, ipm_callback, NULL);

	status = ipm_set_enabled(ipm_handle, 1);
	if (status) {
		log_err("ipm_set_enabled failed\n");
		return -1;
	}

	return 0;
}

static struct rpmsg_device *platform_create_rpmsg_vdev(void)
{
	struct fw_rsc_vdev_vring *vring_rsc;
	struct virtio_device *vdev;
	int ret;

	vdev = rproc_virtio_create_vdev(VIRTIO_DEV_DEVICE, VDEV_ID, rsc_table_to_vdev(rsc_table),
					rsc_io, NULL, rpmsg_notify_callback, NULL);

	if (!vdev) {
		log_err("failed to create vdev\n");
		return NULL;
	}

	/* wait master rpmsg init completion */
	rproc_virtio_wait_remote_ready(vdev);

	vring_rsc = rsc_table_get_vring0(rsc_table);
	ret = rproc_virtio_init_vring(vdev, 0, vring_rsc->notifyid,
				      (void *)(uintptr_t)vring_rsc->da, rsc_io,
				      vring_rsc->num, vring_rsc->align);
	if (ret) {
		log_err("failed to init vring 0\n");
		goto failed;
	}

	vring_rsc = rsc_table_get_vring1(rsc_table);
	ret = rproc_virtio_init_vring(vdev, 1, vring_rsc->notifyid,
				      (void *)(uintptr_t)vring_rsc->da, rsc_io,
				      vring_rsc->num, vring_rsc->align);
	if (ret) {
		log_err("failed to init vring 1\n");
		goto failed;
	}

	ret = rpmsg_init_vdev(&rvdev, vdev, rpmsg_ns_callback, shm_io, NULL);
	if (ret) {
		log_err("failed rpmsg_init_vdev\n");
		goto failed;
	}

	return rpmsg_virtio_get_rpmsg_device(&rvdev);

failed:
	rproc_virtio_remove_vdev(vdev);

	return NULL;
}

static void rpmsg_ept_task(void *priv, void *arg2, void *arg3)
{
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

	struct rpmsg_ept_priv *ept_priv = (struct rpmsg_ept_priv *)priv;
	struct rpmsg_endpoint *ept = &ept_priv->ept;
	struct iovec *msg = &ept_priv->msg;
	unsigned char tx_buf[512];

	for (;;) {
		k_sem_take(&ept_priv->data_sem, K_FOREVER);

		if (msg->len) {
			memcpy(tx_buf, msg->addr, msg->len);
			tx_buf[msg->len] = 0;
#ifdef RPMSG_PRINT_RECV
			if ((msg->len == 2) && (tx_buf[0] == 0xd) && (tx_buf[1] == 0xa))
				os_printf("ept%d: Get New Line From Master Side\r\n", ept_priv->addr);
			else
				os_printf("ept%d: Get Message From Master Side : \"%s\" [len : %d]\r\n", ept_priv->addr, tx_buf, msg->len);
#endif

#ifdef RPMSG_ECHO_BACK
			rpmsg_send(ept, tx_buf, msg->len);
#endif
			rpmsg_release_rx_buffer(ept, msg->addr);
		}

		msg->len = 0;
		msg->addr = NULL;
	}
}

static void rpmsg_deinit(void)
{
	ipm_set_enabled(ipm_handle, 0);
	rpmsg_deinit_vdev(&rvdev);
	metal_finish();
}

static void rpmsg_init_task(void *arg1, void *arg2, void *arg3)
{
	int ret = 0;
	int i;

	ret = platform_init();
	if (ret) {
		log_err("Failed to initialize platform\n");
		goto end;
	}

	rpdev = platform_create_rpmsg_vdev();
	if (!rpdev) {
		log_err("Failed to create rpmsg virtio device\n");
		goto end;
	}

	for (i = 0; i < RPMSG_EPT_NUM; i++) {
		ret = rpmsg_create_ept(&ept_priv[i].ept, rpdev, ept_priv[i].sn,
				       ept_priv[i].addr, RPMSG_ADDR_ANY,
				       rpmsg_ept_recv_callback, NULL);
		if (ret) {
			os_printf("Failed to create RPMsg EPT(%d)!\r\n", ept_priv[i].addr);
			goto end;
		}

		k_thread_create(&ept_priv[i].thread_data, thread_ept_stack[i], RPMSG_EPT_TASK_STACK_SIZE,
				rpmsg_ept_task, &ept_priv[i], NULL, NULL, K_PRIO_COOP(7), 0, K_NO_WAIT);
	}

	k_thread_create(&thread_notify_data, thread_notify_stack, RPMSG_NOTIFY_TASK_STACK_SIZE,
			rpmsg_notify_task, NULL, NULL, NULL, K_PRIO_COOP(6), 0, K_NO_WAIT);

	return;

end:
	rpmsg_deinit();
}

static void rpmsg_ept_priv_setup(void)
{
	uint32_t ept_addr[RPMSG_EPT_NUM] = RPMSG_EPT_ADDR;
	int i;

	for (i = 0; i < RPMSG_EPT_NUM; i++) {
		ept_priv[i].sn = RPMSG_NS_ANNOUNCE_STRING;
		ept_priv[i].addr = ept_addr[i];
		ept_priv[i].ept.priv = &ept_priv[i];
		k_sem_init(&ept_priv[i].data_sem, 0, 1);
	}
}

#ifdef CONFIG_MMU
static void rpmsg_mmu_map_setup(void)
{
	void *rsc_table_va;
	void *rpmsg_va;
	void *shm_va;
	int ret;

	ret = os_mmu_map("RPMSG", (uint8_t **)&rpmsg_va,
			 (uintptr_t)DT_REG_ADDR(DT_NODELABEL(rpmsg)),
			 DT_REG_SIZE(DT_NODELABEL(rpmsg)),
			 (RPMSG_IS_COHERENT ? OS_MEM_CACHE_WB : OS_MEM_DEVICE_nGnRE) |
			 OS_MEM_DIRECT_MAP | OS_MEM_PERM_RW);
	if (ret) {
		log_err("RPMSG os_mmu_map() failed\n");
	}

	ret = os_mmu_map("SHRAM", (uint8_t **)&shm_va,
			 (uintptr_t)SHM_START_ADDR, SHM_SIZE,
			 (RPMSG_IS_COHERENT ? OS_MEM_CACHE_WB : OS_MEM_DEVICE_nGnRE) |
			 OS_MEM_DIRECT_MAP | OS_MEM_PERM_RW);
	if (ret) {
		log_err("SHRAM os_mmu_map() failed\n");
	}

	ret = os_mmu_map("RSC_TABLE", (uint8_t **)&rsc_table_va,
			 (uintptr_t)DT_REG_ADDR(DT_NODELABEL(rsc_table)),
			 DT_REG_SIZE(DT_NODELABEL(rsc_table)),
			 OS_MEM_DEVICE_nGnRE | OS_MEM_DIRECT_MAP | OS_MEM_PERM_RW);
	if (ret) {
		log_err("RSC_TABLE os_mmu_map() failed\n");
	}
}
#endif

int main(void)
{
	os_printf("\r\n%s: RTOS%d: ", CPU_CORE_NAME, RTOSID);
	os_printf("Multiple Endpoints RPMsg String Echo Zephyr Demo\r\n");

#ifdef CONFIG_MMU
	rpmsg_mmu_map_setup();
#endif
	rpmsg_ept_priv_setup();

	k_thread_create(&thread_rpmsg_init_data, thread_rpmsg_init_stack, RPMSG_INIT_TASK_STACK_SIZE,
			rpmsg_init_task,
			NULL, NULL, NULL, K_PRIO_COOP(8), 0, K_NO_WAIT);

	return 0;
}
