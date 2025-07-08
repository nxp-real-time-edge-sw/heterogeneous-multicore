/*
 * Copyright 2022-2023, 2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/list.h>
#include <net/data_pkt.h>
#include <os/stdbool.h>
#include <os/stdlib.h>
#include <os/stdio.h>
#include <os/string.h>

struct data_pkt *data_pkt_build(uint8_t *addr, uint32_t buf_len, uint32_t flags)
{
	struct data_pkt *pkt;

	os_assert(buf_len > (DATA_PKT_HEAD_ROOM + DATA_PKT_TAIL_ROOM), "build data pkt failed");

	pkt = (struct data_pkt *)addr;
	pkt->data = addr + DATA_PKT_HEAD_ROOM;
	pkt->data_len = 0;
	pkt->tail = addr + buf_len - DATA_PKT_TAIL_ROOM;
	pkt->end = addr + buf_len;

	pkt->flags = flags;

	list_init(&pkt->pkt_entry);

	return pkt;
}

struct data_pkt *data_pkt_alloc(uint32_t data_room_size, uint32_t flags)
{
	uint32_t size = data_room_size + DATA_PKT_HEAD_ROOM + DATA_PKT_TAIL_ROOM;
	void *addr;

	addr = os_malloc(size);
	if (!addr) {
		os_printf("No enough memory for data pkt\r\n");
		return NULL;
	}
	memset(addr, 0, size);

	return data_pkt_build(addr, size, flags);
}

struct data_pkt *data_pkt_clone(struct data_pkt *pkt)
{
	struct data_pkt *new_pkt;

	new_pkt = data_pkt_alloc(pkt->tail - pkt->data, pkt->flags);
	if (!new_pkt)
		return NULL;

	memcpy(new_pkt->data, pkt->data, pkt->data_len);
	data_pkt_set_len(new_pkt, pkt->data_len);

	return new_pkt;
}

void data_pkt_free(struct data_pkt *pkt)
{
	if (pkt->flags & DATA_PKT_FLAG_STATIC_BUFF)
		pkt->used = false;
	else
		os_free(pkt);
}

void data_pkt_set_len(struct data_pkt *pkt, uint32_t data_len)
{
	pkt->data_len = data_len;
}
