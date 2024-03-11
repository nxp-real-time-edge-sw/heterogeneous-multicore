/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RTOS_MEMORY_H_
#define _RTOS_MEMORY_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef RTOSID
#define RTOSID			0
#endif

#define RTOS_INST_STRIDE	0x1000000

#if (RTOSID == 0)
#define SRAM_BASE		c0000000
#define RAM_CONSOLE_BASE	c0100000
#elif (RTOSID == 1)
#define SRAM_BASE		c1000000
#define RAM_CONSOLE_BASE	c1100000
#elif (RTOSID == 2)
#define SRAM_BASE		c2000000
#define RAM_CONSOLE_BASE	c2100000
#elif (RTOSID == 3)
#define SRAM_BASE		c3000000
#define RAM_CONSOLE_BASE	c3100000
#else
#error "Unsupported RTOSID!"
#endif

#ifndef DT_ADDR
#define DT_ADDR(a)		 _CONCAT(0x, a)
#endif

#define RAM_CONSOLE_ADDR	DT_ADDR(RAM_CONSOLE_BASE)

#endif /* _RTOS_MEMORY_H_ */
