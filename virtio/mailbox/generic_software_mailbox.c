/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "irq.h"
#include "os/assert.h"
#include "os/semaphore.h"
#include "gen_sw_mbox.h"
#include "gen_sw_mbox_config.h"

/* Max supported ISR counts */
#define ISR_COUNT			(32U)

#ifndef GEN_SW_MBOX_BASE
#error "GEN_SW_MBOX_BASE is NOT defined, define it in gen_sw_mbox_config.h!"
#endif

#ifndef GEN_SW_MBOX_IRQ
#error "GEN_SW_MBOX_IRQ is NOT defined, define it in gen_sw_mbox_config.h!"
#endif

#ifndef GEN_SW_MBOX_REMOTE_IRQ
#error "GEN_SW_MBOX_REMOTE_IRQ is NOT defined, define it in gen_sw_mbox_config.h!"
#endif

#ifndef GEN_SW_MBOX_IRQ_PRIO
#error "GEN_SW_MBOX_IRQ_PRIO is NOT defined, define it in gen_sw_mbox_config.h!"
#endif

#ifndef VIRTIO_GEN_SW_MBOX_CHANNEL
#define VIRTIO_GEN_SW_MBOX_CHANNEL	(1)
#endif

static int32_t disable_counter = 0;
static os_sem_t mbox_lock;

struct virtio_mmio_wr_ops {
	uint64_t mmio_base;
	uint32_t off;
	uint32_t val;
	uint8_t len;
};

/*
 * Structure to keep track of registered ISR's.
 */
struct isr_info {
	uintptr_t event;
	void (*write_cb)(uintptr_t addr, uint32_t len, void* data);
	void *data;
};

static struct isr_info isr_table[ISR_COUNT];

static int find_isr_entry(uintptr_t event)
{
	int i;

	for (i = 0; i < ISR_COUNT; i++) {
		if (isr_table[i].event == event)
			return i;
	}

	return -1;
}

static int find_empty_isr_entry(void)
{
	return find_isr_entry(0);
}


/*
 * register_isr
 * Registers interrupt handler data for the given interrupt vector.
 * @param event - index
 * @param fn    - handler
 * @param data  - interrupt handler data
 */
static void register_isr(uintptr_t event, void (*fn)(uintptr_t, uint32_t, void*), void *data)
{
	int idx = find_empty_isr_entry();

	if (idx < 0)
		return;

	isr_table[idx].event = event;
	isr_table[idx].write_cb = fn;
	isr_table[idx].data = data;
}

/*
 * unregister_isr
 * Unregisters interrupt handler data for the given interrupt vector.
 * @param event - index
 */
static void unregister_isr(uintptr_t event)
{
	int idx = find_isr_entry(event);

	if (idx < 0)
		return;

	isr_table[idx].event = 0;
	isr_table[idx].write_cb = ((void *)0);
	isr_table[idx].data = ((void *)0);
}

static void call_isr(uintptr_t vector)
{
	struct virtio_mmio_wr_ops *wr_ops = (struct virtio_mmio_wr_ops *)vector;
	int idx = find_isr_entry(wr_ops->mmio_base);
	struct isr_info *info;

	if (idx < 0) {
		os_printf("%s: ERROR: Doesn't find ISR for 0x%llx\n", __func__, wr_ops->mmio_base);
		return;
	}

	info = &isr_table[idx];
	if (info->write_cb)
		info->write_cb(wr_ops->mmio_base + wr_ops->off, wr_ops->len, info->data);
}

static void mbox_recv_cb(void *data, uint32_t msg)
{
	call_isr(msg);
}

int32_t mbox_register_callback(uintptr_t event, void (*fn)(uintptr_t, uint32_t, void*), void *isr_data)
{
	uint32_t flags = platform_in_isr() ? OS_SEM_FLAGS_ISR_CONTEXT : 0;

	os_sem_take(&mbox_lock, flags, OS_SEM_FLAGS_ISR_CONTEXT);
	register_isr(event, fn, isr_data);
	os_sem_give(&mbox_lock, flags);

	return 0;
}

int32_t mbox_unregister_callback(uintptr_t event)
{
	uint32_t flags = platform_in_isr() ? OS_SEM_FLAGS_ISR_CONTEXT : 0;

	os_sem_take(&mbox_lock, flags, OS_SEM_FLAGS_ISR_CONTEXT);
	unregister_isr(event);
	os_sem_give(&mbox_lock, flags);

	return 0;
}

void mbox_kick_front(uintptr_t event)
{
	uint32_t flags = platform_in_isr() ? OS_SEM_FLAGS_ISR_CONTEXT : 0;
	uint32_t msg = (uint32_t)(event);

	os_sem_take(&mbox_lock, flags, OS_SEM_FLAGS_ISR_CONTEXT);
	gen_sw_mbox_sendmsg((void *)GEN_SW_MBOX_BASE, VIRTIO_GEN_SW_MBOX_CHANNEL, msg, false);
	os_sem_give(&mbox_lock, flags);
}

/*
 * mbox_interrupt_enable
 * Enable peripheral-related interrupt
 * @param event Virtual vector ID that needs to be converted to IRQ number
 * @return event Return value is never checked.
 */
int32_t mbox_interrupt_enable(uintptr_t event)
{
	os_assert(0 < disable_counter, " disable_counter is %d",  disable_counter);

	platform_global_isr_disable();
	disable_counter--;

	if (disable_counter == 0) {
		GIC_EnableIRQ(GEN_SW_MBOX_IRQ);
	}
	platform_global_isr_enable();

	return ((int32_t)event);
}

/*
 * mbox_interrupt_disable
 * Disable peripheral-related interrupt.
 * @param event Virtual vector ID that needs to be converted to IRQ number
 * @return event Return value is never checked.
 */
int32_t mbox_interrupt_disable(uintptr_t event)
{
	os_assert(0 <= disable_counter, " disable_counter is %d",  disable_counter);

	platform_global_isr_disable();
	if (disable_counter == 0)
		GIC_DisableIRQ(GEN_SW_MBOX_IRQ);

	disable_counter++;
	platform_global_isr_enable();

	return ((int32_t)event);
}

/*
 * mbox_init
 */
int32_t mbox_init(bool enable_irq)
{
	if (0 != os_sem_init(&mbox_lock, 1))
		return -1;

	gen_sw_mbox_init();
	gen_sw_mbox_register((void *)GEN_SW_MBOX_BASE, GEN_SW_MBOX_IRQ,
			     GEN_SW_MBOX_REMOTE_IRQ, GEN_SW_MBOX_IRQ_PRIO);
	gen_sw_mbox_register_chan_callback((void *)GEN_SW_MBOX_BASE, VIRTIO_GEN_SW_MBOX_CHANNEL, mbox_recv_cb, NULL);


	return 0;
}

/*
 * mbox_deinit
 */
int32_t mbox_deinit(void)
{
	gen_sw_mbox_unregister_chan_callback((void *)GEN_SW_MBOX_BASE, VIRTIO_GEN_SW_MBOX_CHANNEL);
	gen_sw_mbox_unregister((void *)GEN_SW_MBOX_BASE);
	gen_sw_mbox_deinit();
	os_sem_destroy(&mbox_lock);
	return 0;
}
