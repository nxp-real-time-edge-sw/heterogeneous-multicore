/*
 * Copyright 2021-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FREERTOS_ASSERT_H_
#define _FREERTOS_ASSERT_H_

#include "cmsis_compiler.h"
#include "os/stdio.h"

#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#ifndef unlikely
#define unlikely(x)     __builtin_expect(x, 0)
#endif /* !unlikely */
#ifndef likely
#define likely(x)       __builtin_expect(x, 1)
#endif /* !likely */

#define os_assert(cond, msg, ...)       \
do { \
    if (unlikely(!(cond))) { \
        os_printf("\tAssertion failed at %s: %d: %s:\n\r" msg "\n\r", __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
        while(1); \
    } \
} while (0)

#define os_assert_equal(a, b, msg, ...)      os_assert((a) == (b), msg, ##__VA_ARGS__)

#endif /* #ifndef _FREERTOS_ASSERT_H_ */
