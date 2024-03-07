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
#define SRAM_BASE		d0000000
#define RAM_CONSOLE_BASE	d0100000
#elif (RTOSID == 1)
#define SRAM_BASE		d1000000
#define RAM_CONSOLE_BASE	d1100000
#else
#error "Unsupported RTOSID!"
#endif

#ifndef DT_ADDR
#define DT_ADDR(a)		 _CONCAT(0x, a)
#endif

#define RAM_CONSOLE_ADDR	DT_ADDR(RAM_CONSOLE_BASE)

#endif /* _RTOS_MEMORY_H_ */
