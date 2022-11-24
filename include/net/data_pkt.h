/*
 * Copyright 2022-2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/list.h>

#ifndef _DATA_PKT_H_
#define _DATA_PKT_H_

/*
 * Head room must be enough for sizeof(struct data_pkt), and
 * also need to be aligned with enet buffer aligment.
 */
#define DATA_PKT_HEAD_ROOM 	128

#define DATA_PKT_TAIL_ROOM	0

/* Data PKT flags */
/* Data PKT buffer can be freed directly */
#define DATA_PKT_FLAG_CAN_FREE	(1 << 0)
/* Data PKT using static buffer */
#define DATA_PKT_FLAG_STATIC_BUFF	(1 << 1)

/*
 * Data Buffer Layout
 * ____________________
 * |struct data_pkt    | <-- *data_pkt
 * |                   |
 * |___________________|
 * |                   | <-- *data, offset is DATA_PKT_HEAD_ROOM
 * |  data             |
 * |___________________|
 * |                   | <-- *tail
 * |  tail             |
 * |___________________| <-- *end
 */


/* Take care to make sure sizeof(struct dat_pkt) < DATA_PKT_HEAD_ROOM */
struct data_pkt {
	struct list_head pkt_entry;

	void *data;
	uint32_t data_len;

	void *tail;
	void *end;

	uint32_t flags;

	/* only for static allocated buffers */
	bool used;
};

struct data_pkt *data_pkt_alloc(uint32_t data_room_size, uint32_t flags);
struct data_pkt *data_pkt_build(void *addr, uint32_t buf_len, uint32_t flags);
struct data_pkt *data_pkt_clone(struct data_pkt *pkt);
void data_pkt_free(struct data_pkt *pkt);
void data_pkt_set_len(struct data_pkt *pkt, uint32_t data_len);

static inline struct data_pkt *data_pkt_get(void *data_addr)
{
	return (struct data_pkt *)((char *)data_addr - DATA_PKT_HEAD_ROOM);
};

#endif /* _DATA_PKT_H */
