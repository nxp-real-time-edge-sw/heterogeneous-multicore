/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_MMU_H_
#define _APP_MMU_H_

#ifdef CONFIG_RAM_CONSOLE
#include "rtos_memory.h"

#define APP_MMU_ENTRIES		\
	MMU_REGION_FLAT_ENTRY("RAM_CONSOLE",					\
			      RAM_CONSOLE_ADDR, RAM_CONSOLE_SIZE,			\
			      MT_NORMAL_NC | MT_P_RW_U_RW | MT_NS),
#endif

#endif /* _APP_MMU_H_ */
