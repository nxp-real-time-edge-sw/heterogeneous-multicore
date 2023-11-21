/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RTOS_MEMORY_H_
#define _RTOS_MEMORY_H_

/* Memory used by RTOS kernel */
#define M_INTERRUPTS_BASE	(0xD0000000 + 0x1000000 * RTOSID)
#define M_INTERRUPTS_LEN	0x00002000 /*  8 kB */

#define M_TEXT_BASE		(M_INTERRUPTS_BASE + M_INTERRUPTS_LEN)
#define M_TEXT_LEN		0x005FE000 /* ~6 MB */

#define M_DATA_BASE		(M_TEXT_BASE + M_TEXT_LEN)
#define M_DATA_LEN		0x005FE000 /* ~6 MB */

#define M_STACKS_BASE		(M_DATA_BASE + M_DATA_LEN)
#define M_STACKS_LEN		0x00002000 /*  8 kB */

#define M_STACKS_NC_BASE	(M_STACKS_BASE + M_STACKS_LEN)
#define M_STACKS_NC_LEN		0x003FF000 /*  ~4 MB */

/* Memory used by RAM Console */
#define RAM_CONSOLE_ADDR	(M_STACKS_NC_BASE + M_STACKS_NC_LEN)
#define RAM_CONSOLE_SIZE	0x00001000 /* 4KB */

#define RTOS_MEM_LEN		0x01000000 /* 16 MB */

#endif
