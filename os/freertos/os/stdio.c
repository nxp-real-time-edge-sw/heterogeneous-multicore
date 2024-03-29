/*
 * Copyright 2021-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_debug_console.h"

#if SDK_DEBUGCONSOLE == DEBUGCONSOLE_RAM_CONSOLE

int os_printf(const char *fmt_s, ...)
{
    int rc;
    va_list ap;

    va_start(ap, fmt_s);
    rc = RamConsole_Vprintf(fmt_s, ap);
    va_end(ap);

    return rc;
}

int os_vprintf(const char *fmt_s, va_list ap)
{
    return RamConsole_Vprintf(fmt_s, ap);
}

#else

int os_printf(const char *fmt_s, ...)
{
    int rc;
    va_list ap;

    va_start(ap, fmt_s);
    rc = DbgConsole_Vprintf(fmt_s, ap);
    va_end(ap);

    return rc;
}

int os_vprintf(const char *fmt_s, va_list ap)
{
    return DbgConsole_Vprintf(fmt_s, ap);
}

#endif
