/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "fsl_device_registers.h"
#include "fsl_mu.h"
#include "os/assert.h"
#include "os/semaphore.h"

#include "app_virtio_config.h"

#define APP_MU_IRQ_PRIORITY	(3U)

#ifndef VIRTIO_MU_CHANNEL
#define VIRTIO_MU_CHANNEL	2
#endif

#define _RX_FULL_FLAG(c)	kMU_Rx##c##FullFlag
#define RX_FULL_FLAG(c)		_RX_FULL_FLAG(c)
#define _RX_FULL_INT(c)		kMU_Rx##c##FullInterruptEnable
#define RX_FULL_INT(c)		_RX_FULL_INT(c)

/* Max supported ISR counts */
#define ISR_COUNT		(32U)

static os_sem_t platform_lock;
static int32_t isr_counter     = 0;
static int32_t disable_counter = 0;

struct virtio_mmio_wr_ops {
	uint64_t mmio_base;
	uint32_t off;
	uint32_t val;
	uint8_t len;
};

/*
 * Structure to keep track of registered ISR's.
 */
struct isr_info
{
	uint32_t event;
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
	int idx = find_isr_entry((uint32_t)wr_ops->mmio_base);
	struct isr_info *info;

	if (idx < 0)
		return;

	info = &isr_table[idx];
	if (info->write_cb)
		info->write_cb((uint32_t)wr_ops->mmio_base + wr_ops->off, wr_ops->len, info->data);
}

int32_t mbox_register_callback(uintptr_t event, void (*fn)(uintptr_t, uint32_t, void*), void *isr_data)
{
	uint32_t flags = platform_in_isr() ? OS_SEM_FLAGS_ISR_CONTEXT : 0;

	/* Register ISR to environment layer */
	register_isr(event, fn, isr_data);

	/* Prepare the MU Hardware, enable channel 1 interrupt */
	os_sem_take(&platform_lock, flags, OS_SEM_TIMEOUT_MAX);

	os_assert(0 <= isr_counter, "isr_counter is %d", isr_counter);
	if (isr_counter == 0)
	{
		MU_EnableInterrupts(MU_CM_IF, RX_FULL_INT(VIRTIO_MU_CHANNEL));
	}
	isr_counter++;

	os_sem_give(&platform_lock, flags);

	return 0;
}

int32_t mbox_unregister_callback(uintptr_t event)
{
	uint32_t flags = platform_in_isr() ? OS_SEM_FLAGS_ISR_CONTEXT : 0;

	/* Prepare the MU Hardware */
	os_sem_take(&platform_lock, flags, OS_SEM_TIMEOUT_MAX);

	os_assert(0 < isr_counter, "isr_counter is %d", isr_counter);
	isr_counter--;
	if (isr_counter == 0)
	{
		MU_DisableInterrupts(MU_CM_IF, RX_FULL_INT(VIRTIO_MU_CHANNEL));
	}

	unregister_isr(event);

	os_sem_give(&platform_lock, flags);

	return 0;
}

void mbox_kick_front(uintptr_t event)
{
	uint32_t flags = platform_in_isr() ? OS_SEM_FLAGS_ISR_CONTEXT : 0;

	os_sem_take(&platform_lock, flags, OS_SEM_TIMEOUT_MAX);
	MU_SendMsgNonBlocking(MU_CM_IF, VIRTIO_MU_CHANNEL, event);
	os_sem_give(&platform_lock, flags);
}

/*
 * MU Interrrupt handler
 */
int32_t mu_irq_handler(void)
{
	uint32_t wr_ops;

	if (((RX_FULL_FLAG(VIRTIO_MU_CHANNEL)) & MU_GetStatusFlags(MU_CM_IF)) != 0UL)
	{
		wr_ops = MU_ReceiveMsgNonBlocking(MU_CM_IF, VIRTIO_MU_CHANNEL);
		call_isr(wr_ops);
	}
	/* ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
	 * exception return operation might vector to incorrect interrupt.
	 * For Cortex-M7, if core speed much faster than peripheral register write speed,
	 * the peripheral interrupt flags may be still set after exiting ISR, this results to
	 * the same error similar with errata 83869 */
#if (defined __CORTEX_M) && ((__CORTEX_M == 4U) || (__CORTEX_M == 7U))
	__DSB();
#endif

	return 0;
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

	if (disable_counter == 0)
	{
		NVIC_EnableIRQ(MU_IRQ_NUM);
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
	{
		NVIC_DisableIRQ(MU_IRQ_NUM);
	}
	disable_counter++;
	platform_global_isr_enable();
	return ((int32_t)event);
}

/*
 * mbox_init
 */
int32_t mbox_init(bool enable_irq)
{
	/*
	 * Prepare for the MU Interrupt
	 */
	MU_Init(MU_CM_IF);
	NVIC_SetPriority(MU_IRQ_NUM, APP_MU_IRQ_PRIORITY);
	if (enable_irq)
		NVIC_EnableIRQ(MU_IRQ_NUM);

	if (0 != os_sem_init(&platform_lock, 1))
		return -1;

	return 0;
}

/*
 * mbox_deinit
 */
int32_t mbox_deinit(void)
{
	os_sem_destroy(&platform_lock);
	return 0;
}
