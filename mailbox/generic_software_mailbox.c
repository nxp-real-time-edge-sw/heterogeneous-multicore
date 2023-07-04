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

#include "app_virtio_config.h"

/* Max supported ISR counts */
#define ISR_COUNT			(32U)

#ifndef VIRTIO_GEN_SW_MBOX_BASE
#error "VIRTIO_GEN_SW_MBOX_BASE is NOT defined, define it in app_virtio_config.h!"
#endif

#ifndef VIRTIO_GEN_SW_MBOX_IRQ
#error "VIRTIO_GEN_SW_MBOX_IRQ is NOT defined, define it in app_virtio_config.h!"
#endif

#ifndef VIRTIO_GEN_SW_MBOX_REMOTE_IRQ
#error "VIRTIO_GEN_SW_MBOX_REMOTE_IRQ is NOT defined, define it in app_virtio_config.h!"
#endif

#ifndef VIRTIO_GEN_SW_MBOX_IRQ_PRIO
#define VIRTIO_GEN_SW_MBOX_IRQ_PRIO	(0)
#endif

/*
 * Generic software Registers:
 *
 * RX_STATUS[n]: RX channel n status
 * TX_STATUS[n]: TX channel n status
 * 	0: indicates message in T/RX_CH[n] is invalid and channel ready.
 * 	1: indicates message in T/RX_CH[n] is valid and channel busy.
 * 	2: indicates message in T/RX_CH[n] has been received by the peer.
 * RX_CH[n]: Receive data register for channel n
 * TX_CH[n]: Transmit data register for channel n
 *
 * To send a message:
 * Update the data register TX_CH[n] with the message, then set the
 * TX_STATUS[n] to 1, inject a interrupt to remote side; after the
 * transmission done set the TX_STATUS[n] back to 0.
 *
 * When received a message:
 * Get the received data from RX_CH[n] and then set the RX_STATUS[n] to
 * 2 and inject a interrupt to notify the remote side transmission done.
 */

#define MAX_CH				(4)

#ifndef VIRTIO_GEN_SW_MBOX_CHANNEL
#define VIRTIO_GEN_SW_MBOX_CHANNEL	(1)
#endif

struct gen_sw_mbox {
	uint32_t rx_status[MAX_CH];
	uint32_t tx_status[MAX_CH];
	uint32_t reserved[MAX_CH];
	uint32_t rx_ch[MAX_CH];
	uint32_t tx_ch[MAX_CH];
};

enum sw_mbox_channel_status {
	S_READY,
	S_BUSY,
	S_DONE,
};

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

static void gen_sw_mbox_handler(void *data)
{
	struct gen_sw_mbox *base = (struct gen_sw_mbox *)data;
	uintptr_t wr_ops;

	if (base->tx_status[VIRTIO_GEN_SW_MBOX_CHANNEL] == S_DONE)
		base->tx_status[VIRTIO_GEN_SW_MBOX_CHANNEL] = S_READY;

	__DSB();
	/* Check if the interrupt is for us */
	if (base->rx_status[VIRTIO_GEN_SW_MBOX_CHANNEL] != S_BUSY)
		return;

	wr_ops = base->rx_ch[VIRTIO_GEN_SW_MBOX_CHANNEL];

	base->rx_status[VIRTIO_GEN_SW_MBOX_CHANNEL] = S_DONE;
	GIC_SetPendingIRQ(VIRTIO_GEN_SW_MBOX_REMOTE_IRQ);

	call_isr(wr_ops);
}

static void gen_sw_mailbox_init(struct gen_sw_mbox *base, bool enable_irq)
{
	/* Clear status register */
	base->rx_status[VIRTIO_GEN_SW_MBOX_CHANNEL] = 0;
	base->tx_status[VIRTIO_GEN_SW_MBOX_CHANNEL] = 0;

	irq_register(VIRTIO_GEN_SW_MBOX_IRQ, gen_sw_mbox_handler, base, VIRTIO_GEN_SW_MBOX_IRQ_PRIO);
	if (enable_irq)
		GIC_EnableIRQ(VIRTIO_GEN_SW_MBOX_IRQ);
}

static void gen_sw_mbox_sendmsg(struct gen_sw_mbox *base, uint32_t ch, uint32_t msg)
{
	while (base->tx_status[ch] != S_READY)
		/* Avoid sending the same vq id multiple times when channel is busy */
		if (msg == base->tx_ch[ch])
			return;

	base->tx_ch[ch] = msg;
	__DSB();
	base->tx_status[ch] = S_BUSY;

	GIC_SetPendingIRQ(VIRTIO_GEN_SW_MBOX_REMOTE_IRQ);
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
	gen_sw_mbox_sendmsg((struct gen_sw_mbox *)VIRTIO_GEN_SW_MBOX_BASE, VIRTIO_GEN_SW_MBOX_CHANNEL, msg);
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
		GIC_EnableIRQ(VIRTIO_GEN_SW_MBOX_IRQ);
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
		GIC_DisableIRQ(VIRTIO_GEN_SW_MBOX_IRQ);

	disable_counter++;
	platform_global_isr_enable();

	return ((int32_t)event);
}

/*
 * mbox_init
 */
int32_t mbox_init(bool enable_irq)
{
	gen_sw_mailbox_init((struct gen_sw_mbox *)VIRTIO_GEN_SW_MBOX_BASE, enable_irq);

	if (0 != os_sem_init(&mbox_lock, 1))
		return -1;

	return 0;
}

/*
 * mbox_deinit
 */
int32_t mbox_deinit(void)
{
	os_sem_destroy(&mbox_lock);
	return 0;
}
