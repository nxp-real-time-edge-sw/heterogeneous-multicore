/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef VIRTIO_PLATFORM_H_
#define VIRTIO_PLATFORM_H_

#include <stdint.h>
#include "os/stdbool.h"

/* platform interrupt related functions */
int32_t platform_register_callback(uintptr_t event, void (*fn)(uintptr_t, uint32_t, void*), void *isr_data);
int32_t platform_unregister_callback(uintptr_t event);
int32_t platform_interrupt_enable(uintptr_t event);
int32_t platform_interrupt_disable(uintptr_t event);
int32_t platform_in_isr(void);
void platform_kick_front(uintptr_t event);

/* platform init/deinit */
int32_t platform_init(bool enable_irq);
int32_t platform_deinit(void);

#endif
