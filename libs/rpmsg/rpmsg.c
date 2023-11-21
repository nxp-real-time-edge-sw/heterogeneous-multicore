/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include "os/stdlib.h"
#include "os/stdio.h"
#include "os/irq.h"
#include "log/log.h"
#include "rpmsg.h"

#ifdef RL_USE_GEN_SW_MBOX
#include "os/mmu.h"
#include "gen_sw_mbox.h"
#include "gen_sw_mbox_config.h"
#endif

#define rpmsg_nameservice_task_PRIORITY	(configMAX_PRIORITIES - 1)

struct ns_msg {
	const char *name;
	uint32_t dst;
	uint32_t flags;
};

#ifdef RL_USE_GEN_SW_MBOX
static void rpmsg_mailbox_init(void)
{
	gen_sw_mbox_init();
	gen_sw_mbox_register((void *)GEN_SW_MBOX_BASE, GEN_SW_MBOX_IRQ,
			     GEN_SW_MBOX_REMOTE_IRQ, GEN_SW_MBOX_IRQ_PRIO);
}
#endif

static struct rpmsg_ept *rl_ept_to_ept(struct rpmsg_lite_endpoint *rl_ept)
{
	return rl_ept ? rl_ept->rfu : NULL;
}

static uint32_t rpmsg_get_ept_addr(struct rpmsg_instance *ri)
{
	return ri->rpmsg_src++;
}

static void rpmsg_nameservice_task(void *param)
{
	struct rpmsg_instance *ri = param;
	struct rpmsg_ept *ept;
	uint32_t src, dst, flags;
	struct ns_msg msg;

	while (1) {
		if (os_mq_receive(&ri->ns_mq, &msg, 0, OS_QUEUE_EVENT_TIMEOUT_MAX))
			continue;

		dst = msg.dst;
		flags = msg.flags;
		if (flags == RL_NS_CREATE) {
			src = rpmsg_get_ept_addr(ri);
			ept = rpmsg_create_ept(ri, src, msg.name);
			if (!ept) {
				log_err("failed to create EPT (src: %d, dst: %d)\n", src, dst);
			} else {
				ept->remote_addr = dst;
				log_info("created EPT (src: %d, dst: %d)\n", src, dst);
			}
		} else if (flags == RL_NS_DESTROY) {
			ept = rpmsg_get_endpoint_by_dst(ri, dst);
			if (!ept || rpmsg_destroy_ept(ept)) {
				log_err("failed to destroy EPT (dts: %d)\n", dst);
			} else {
				src = ept->rl_ept->addr;
				log_info("destroyed EPT (src: %d, dts: %d)\n", src, dst);
			}
		} else {
			log_err("EPT (dst: %d): invalid flags (0x%x)\n", dst, flags);
		}
	}
}

static void rpmsg_nameservice_isr_cb(uint32_t ept_dst, const char *ept_name, uint32_t flags, void *user_data)
{
	struct rpmsg_instance *ri = user_data;
	struct ns_msg msg;

	msg.dst = ept_dst;
	msg.flags = flags;
	msg.name = ept_name;

	os_mq_send(&ri->ns_mq, &msg, OS_MQUEUE_FLAGS_ISR_CONTEXT, 0);
}

struct rpmsg_ept *rpmsg_get_endpoint_by_dst(struct rpmsg_instance *ri, uint32_t addr)
{
	struct rpmsg_lite_endpoint *rl_ept;
	struct rpmsg_ept *ept;
	uint32_t i;

	for (i = 1; i < ri->rpmsg_src; i++) {
		rl_ept = rpmsg_lite_get_endpoint(ri->rl_inst, i);
		if (rl_ept) {
			ept = rl_ept_to_ept(rl_ept);
			if (ept && ept->remote_addr == addr)
				return ept;
		}
	}

	return NULL;
}

static struct rpmsg_instance *rpmsg_init_pre(const struct rpmsg_config *cfg)
{
	struct rpmsg_instance *ri;

	ri = os_malloc(sizeof(struct rpmsg_instance));
	if (!ri)
		return ri;

	ri->mode = cfg->mode;
	ri->rpmsg_shmem_sz = cfg->rpmsg_shmem_sz;
	ri->rpmsg_buf_sz = cfg->rpmsg_buf_sz;

#ifdef RL_USE_GEN_SW_MBOX
	if (os_mmu_map("RPMSG", (uint8_t **)&ri->rpmsg_shmem_va,
			(uintptr_t)cfg->rpmsg_shmem_pa, ri->rpmsg_shmem_sz,
			(cfg->is_coherent ? OS_MEM_CACHE_WB : OS_MEM_DEVICE_nGnRE) |
			OS_MEM_PERM_RW)) {
		log_err("RPMSG os_mmu_map() failed\n");
		goto err_map_rpmsg;
	}

	if (os_mmu_map("VRINGBUF", (uint8_t **)&ri->rpmsg_buf_va,
			(uintptr_t)cfg->rpmsg_buf_pa, ri->rpmsg_buf_sz,
			(cfg->is_coherent ? OS_MEM_CACHE_WB : OS_MEM_CACHE_NONE) |
			OS_MEM_PERM_RW)) {
		log_err("VRINGBUF os_mmu_map() failed\n");
		goto err_map_vringbuf;
	}

	rpmsg_mailbox_init();
#else
	ri->rpmsg_shmem_va = (void *)cfg->rpmsg_shmem_pa;
	ri->rpmsg_buf_va = (void *)cfg->rpmsg_buf_pa;
#endif

	return ri;

#ifdef RL_USE_GEN_SW_MBOX
err_map_vringbuf:
	os_mmu_unmap((uintptr_t)ri->rpmsg_shmem_va, ri->rpmsg_shmem_sz);
err_map_rpmsg:
	os_free(ri);

	return NULL;
#endif
}

static void rpmsg_deinit_pre(struct rpmsg_instance *ri)
{
#ifdef RL_USE_GEN_SW_MBOX
	os_mmu_unmap((uintptr_t)ri->rpmsg_buf_va, ri->rpmsg_buf_sz);
	os_mmu_unmap((uintptr_t)ri->rpmsg_shmem_va, ri->rpmsg_shmem_sz);
#endif
	os_free(ri);
}

int rpmsg_send(struct rpmsg_ept *ept, void *data, uint32_t len, uintptr_t timeout)
{
	int32_t ret;

	ret = rpmsg_lite_send(ept->ri->rl_inst, ept->rl_ept, ept->remote_addr, data, len, timeout);

	return ret;
}

int rpmsg_recv(struct rpmsg_ept *ept, void *data, uint32_t *len, uintptr_t timeout)
{
	uint32_t msg_src_addr;
	int32_t ret;

	ret = rpmsg_queue_recv(ept->ri->rl_inst, ept->ept_q, (uint32_t *)&msg_src_addr, (char *)data, *len, len, timeout);
	if (ret != RL_SUCCESS) {
		if (ret != RL_ERR_NO_BUFF)
			log_err("rpmsg_queue_recv() failed\n");
		return ret;
	}

	if (ept->remote_addr ==  RL_ADDR_ANY)
		ept->remote_addr = msg_src_addr;
	else if (ept->remote_addr != msg_src_addr)
		return -1;

	return ret;
}

struct rpmsg_ept *rpmsg_create_ept(struct rpmsg_instance *ri, int ept_addr, const char *sn)
{
	struct rpmsg_ept *ept;
	int ret;

	ept = os_malloc(sizeof(struct rpmsg_ept));
	if (!ept)
		return ept;

	ept->ri = ri;
	ept->sn = sn;
	ept->remote_addr =  RL_ADDR_ANY;
	ept->ept_q = rpmsg_queue_create(ri->rl_inst);
	if (!ept->ept_q) {
		log_err("rpmsg failed to create ept queue\n");
		goto err_create_q;
	}

	ept->rl_ept = rpmsg_lite_create_ept(ri->rl_inst, ept_addr, rpmsg_queue_rx_cb, ept->ept_q);

	if (!ept->rl_ept) {
		log_err("rpmsg failed to create ept\n");
		goto err_create;
	}

	ept->rl_ept->rfu = ept;

	if (!ri->mode) {
		ret = rpmsg_ns_announce(ri->rl_inst, ept->rl_ept, sn, RL_NS_CREATE);
		if (ret != RL_SUCCESS) {
			log_err("Nameservice: Create channel failed\n");
			goto err_announce;
		}
	}

	return ept;

err_announce:
	rpmsg_lite_destroy_ept(ri->rl_inst, ept->rl_ept);
	ept->rl_ept = NULL;
err_create:
	rpmsg_queue_destroy(ri->rl_inst, ept->ept_q);
err_create_q:
	os_free(ept);

	return NULL;
}

int rpmsg_destroy_ept(struct rpmsg_ept *ept)
{
	int ret;

	if (!ept->ri->mode) {
		ret = rpmsg_ns_announce(ept->ri->rl_inst, ept->rl_ept, ept->sn, RL_NS_DESTROY);
		if (ret != RL_SUCCESS)
			return ret;
	}

	ret = rpmsg_lite_destroy_ept(ept->ri->rl_inst, ept->rl_ept);
	if (ret == RL_SUCCESS) {
		rpmsg_queue_destroy(ept->ri->rl_inst, ept->ept_q);
		os_free(ept);
	}

	return ret;
}

static int rpmsg_master_init(struct rpmsg_instance *ri, const struct rpmsg_config *cfg)
{
	TaskHandle_t rpmsg_ns_task_handle;
	int ret;

	ri->rl_inst = rpmsg_lite_master_init(ri->rpmsg_shmem_va, ri->rpmsg_shmem_sz, cfg->link_id, cfg->flags);
	if (!ri->rl_inst) {
		log_err("rpmsg_lite_master_init() failed\n");
		return -1;
	}

	ri->rpmsg_src = 1;

	ret = os_mq_open(&ri->ns_mq, "nameservice_mqueue", 100, sizeof(struct ns_msg));
	if (ret) {
		log_err("os_mq_open() failed\n");
		ret = -2;
		goto err_ns_mq_open;
	}

	if (xTaskCreate(rpmsg_nameservice_task, "rpmsg_nameservice_task", configMINIMAL_STACK_SIZE + 100, ri,
			rpmsg_nameservice_task_PRIORITY, &rpmsg_ns_task_handle) != pdPASS) {
		log_err("rpmsg nameservice task creation failed!\n");
		ret = -3;
		goto err_rpmsg_ns_task;
	}

	ri->ns_handle = rpmsg_ns_bind(ri->rl_inst, rpmsg_nameservice_isr_cb, (void *)ri);
	if (!ri->ns_handle) {
		log_err("rpmsg_ns_bind() failed\n");
		ret = -4;
		goto err_rpmsg_ns_bind;
	}

	log_info("RPMSG master NS service ready\n");

	return 0;

err_rpmsg_ns_bind:
	vTaskDelete(rpmsg_ns_task_handle);
err_rpmsg_ns_task:
	os_mq_close(&ri->ns_mq);
err_ns_mq_open:
	rpmsg_lite_deinit(ri->rl_inst);

	return ret;
}

static void rpmsg_master_deinit(struct rpmsg_instance *ri)
{
	rpmsg_ns_unbind(ri->rl_inst, ri->ns_handle);
	rpmsg_lite_deinit(ri->rl_inst);
}

static int rpmsg_remote_init(struct rpmsg_instance *ri, const struct rpmsg_config *cfg)
{
	ri->rl_inst = rpmsg_lite_remote_init(ri->rpmsg_shmem_va, cfg->link_id, cfg->flags);
	if (!ri->rl_inst) {
		log_err("rpmsg_lite_remote_init() failed\n");
		return -1;
	}

	log_info("waiting for link establish ...\n");
	rpmsg_lite_wait_for_link_up(ri->rl_inst, RL_BLOCK);
	log_info("RPMSG link up\n");

	return 0;
}

static void rpmsg_remote_deinit(struct rpmsg_instance *ri)
{
	rpmsg_lite_deinit(ri->rl_inst);
}

struct rpmsg_instance *rpmsg_init(const struct rpmsg_config *cfg)
{
	struct rpmsg_instance *ri;
	bool mode;
	int ret;

	if (!cfg)
		return NULL;

	mode = cfg->mode;
	log_info("RPMSG %s init ...\n", mode ? "master" : "remote");
	ri = rpmsg_init_pre(cfg);
	if (!ri) {
		log_err("rpmsg_init_pre() failed\n");
		return ri;
	}

	if (mode)
		ret = rpmsg_master_init(ri, cfg);
	else
		ret = rpmsg_remote_init(ri, cfg);

	if (ret)
		goto err_rpmsg_init;

	return ri;

 err_rpmsg_init:
	rpmsg_deinit_pre(ri);

	return NULL;
}

void rpmsg_deinit(struct rpmsg_instance *ri)
{
	if (ri->mode)
		rpmsg_master_deinit(ri);
	else
		rpmsg_remote_deinit(ri);

	rpmsg_deinit_pre(ri);
}
