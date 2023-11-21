/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RPMSG_H_
#define _RPMSG_H_

#include "os/mqueue.h"
#include "rpmsg_lite.h"
#include "rpmsg_queue.h"
#include "rpmsg_ns.h"

struct rpmsg_config {
	bool mode;
	int32_t link_id;
	uint32_t flags;
	bool is_coherent;
	uintptr_t rpmsg_shmem_pa;
	size_t rpmsg_shmem_sz;
	uintptr_t rpmsg_buf_pa;
	size_t rpmsg_buf_sz;
};

struct rpmsg_instance {
	struct rpmsg_lite_instance *volatile rl_inst;
	void *rpmsg_shmem_va;
	size_t rpmsg_shmem_sz;
	void *rpmsg_buf_va;
	size_t rpmsg_buf_sz;
	bool mode; /* 1: master; 0: remote */
	/* The following only for master */
	uint32_t rpmsg_src;
	rpmsg_ns_handle ns_handle;
	os_mqd_t ns_mq;
};

struct rpmsg_ept {
	struct rpmsg_lite_endpoint *rl_ept;
	struct rpmsg_instance *ri;
	rpmsg_queue_handle ept_q;
	uint32_t remote_addr;
	const char *sn;
};

struct rpmsg_instance *rpmsg_init(const struct rpmsg_config *cfg);
void rpmsg_deinit(struct rpmsg_instance *ri);
struct rpmsg_ept *rpmsg_create_ept(struct rpmsg_instance *ri, int ept_addr, const char *sn);
int rpmsg_destroy_ept(struct rpmsg_ept *ept);
int rpmsg_send(struct rpmsg_ept *ept, void *data, uint32_t len, uintptr_t timeout);
int rpmsg_recv(struct rpmsg_ept *ept, void *data, uint32_t *len, uintptr_t timeout);
struct rpmsg_ept *rpmsg_get_endpoint_by_dst(struct rpmsg_instance *ri, uint32_t addr);

#endif /* _RPMSG_H_ */
