/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _MT_PLATFORM_H_
#define _MT_PLATFORM_H_

#include <stdint.h>
#include "os/stdbool.h"

/* platform interrupt related functions */
void platform_global_isr_disable(void);
void platform_global_isr_enable(void);
int32_t platform_in_isr(void);

#endif /* _MT_PLATFORM_H_ */
