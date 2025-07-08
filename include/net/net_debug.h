/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NET_DEBUG_H_
#define _NET_DEBUG_H_

#include <stdint.h>

typedef enum {
    FMT_1BYTE, /* Format: 0x0000: BYTE0 BYTE1 BYTE2 ...*/
    FMT_2BYTE, /* Format: 0x0000: BYTE0BYTE1 BYTE2BYTE3 ...*/
} dump_fmt_t;

#ifdef DEBUG

static inline void dbg_hexdump(uint8_t *buf, int len, dump_fmt_t format)
{
	int i;

	for (i = 0; i < len; i++) {
        if (i % 16 == 0)
            os_printf("0x%04x: ", i);
		os_printf("%02x", buf[i]);
        if (format == FMT_1BYTE)
            os_printf(" ");
        else if (format == FMT_2BYTE) {
            if ((i + 1) % 2 == 0)
                os_printf(" ");
        }
		if ((i + 1) % 16 == 0)
			os_printf("\r\n");
	}
	os_printf("\r\n");
}

#else /* DEBUG */

static inline void dbg_hexdump(uint8_t *buf, int len, dump_fmt_t format)
{
    return;
}

#endif /* DEBUG */

#endif /* _NET_DEBUG_H_ */
