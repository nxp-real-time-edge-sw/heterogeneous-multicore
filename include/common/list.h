/*
 * Copyright 2022 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _COMMON_LIST_H_
#define _COMMON_LIST_H_

#include <os/assert.h>

struct list_head {
	struct list_head *prev;	/* Previous node */
	struct list_head *next;	/* Next node */
	struct list_head *head; /* List head node */
	uint32_t count;		/* Count of node in the list */
};

/* Initialize list */
static inline void list_init(struct list_head *list)
{
	os_assert(list, "list is NULL at %s", __func__);

	list->prev = list;
	list->next = list;
	list->head = list;
	list->count = 0;
}

/*
 * Insert a new node to the given list head.
 * list -> old_node -> ...
 * list -> new_node -> old_node -> ...
 */
static inline void list_add_head(struct list_head *list, struct list_head *node)
{
	os_assert(list, "list is NULL at %s", __func__);
	os_assert(list, "node is NULL at %s", __func__);

	node->next = list->next;
	node->prev = list;
	node->head = list;
	list->next->prev = node;
	list->next = node;
	list->count++;
}

/*
 * Insert a new node to the given list tail.
 * list -> old_node -> ... -> last_node
 * list -> old_node -> ... -> last_node -> new_node
 */
static inline void list_add_tail(struct list_head *list, struct list_head *node)
{
	os_assert(list, "list is NULL at %s", __func__);
	os_assert(node, "node is NULL at %s", __func__);

	node->prev = list->prev;
	node->next = list;
	node->head = list;
	list->prev->next = node;
	list->prev = node;
	list->count++;
}

static inline bool list_is_last(struct list_head *list, struct list_head *node)
{
	return node->next == list;
}

/* Delete node from the list it is in */
static inline void list_del(struct list_head *node)
{
	os_assert(node, "node is NULL at %s", __func__);

	node->prev->next = node->next;
	node->next->prev = node->prev;
	node->head->count--;
	list_init(node);
}

static inline bool list_empty(struct list_head *list)
{
	os_assert(list, "list is NULL at %s", __func__);

	return list->next == list;
}

#define container_of(ptr, type, member) \
	((type *)(((char *)(ptr)) - offsetof(type, member)))
	//((type *)(((char *)(ptr)) - (char *) &((type *)0)->member))

/* Get struct pointer for which the list node is in */
#define list_entry(ptr, type, member)	\
	container_of(ptr, type, member)

/* Get struct pointer for which the first list node is in */
#define list_first_entry(ptr, type, member)	\
	list_entry((ptr)->next, type, member)

/* Get struct pointer for which the last list node is in */
#define list_last_entry(ptr, type, member)	\
	list_entry((ptr)->prev, type, member)

/* Get struct pointer for which the next list node is in */
#define list_next_entry(pos, member)					\
	list_entry((pos)->member.next, typeof(*(pos)), member)

/* Iterate a list, not safe for removing entry during iteration */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_first_entry(head, typeof(*pos), member);	\
		&pos->member != head;					\
		pos = list_next_entry(pos, member))

/* Iterate a list, safe for removing entry during iteration */
#define list_for_each_entry_safe(pos, tmp, head, member)		\
	for (pos = list_first_entry(head, typeof(*pos), member),	\
		tmp = list_next_entry(pos, member);			\
		&pos->member != head;					\
		pos = tmp, tmp = list_next_entry(pos, member))

#endif /* _COMMON_LIST_H_ */
