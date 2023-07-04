/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _MT_MAILBOX_H_
#define _MT_MAILBOX_H_

#include <stdint.h>
#include "os/stdbool.h"

int32_t mbox_register_callback(uintptr_t event, void (*fn)(uintptr_t, uint32_t, void*), void *isr_data);
int32_t mbox_unregister_callback(uintptr_t event);
int32_t mbox_interrupt_enable(uintptr_t event);
int32_t mbox_interrupt_disable(uintptr_t event);
void mbox_kick_front(uintptr_t event);

int32_t mbox_init(bool enable_irq);
int32_t mbox_deinit(void);

#endif /* _MT_MAILBOX_H_ */
