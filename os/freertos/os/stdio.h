/*
 * Copyright 2021-2022 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FREERTOS_STDIO_H_
#define _FREERTOS_STDIO_H_

#include "fsl_debug_console.h"

#if SDK_DEBUGCONSOLE == DEBUGCONSOLE_RAM_CONSOLE

#define os_printf RamConsole_Printf
#define os_vprintf RamConsole_Vprintf

#else

static inline int os_printf(const char *fmt_s, ...)
{
    int rc;
    va_list ap;

    va_start(ap, fmt_s);
    rc = DbgConsole_Vprintf(fmt_s, ap);
    va_end(ap);

    return rc;
}

static inline int os_vprintf(const char *fmt_s, va_list ap)
{
    return DbgConsole_Vprintf(fmt_s, ap);
}

#endif

#endif /* #ifndef _FREERTOS_STDIO_H_ */
