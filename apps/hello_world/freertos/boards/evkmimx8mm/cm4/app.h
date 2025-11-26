/*
 * Copyright 2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _HELLO_WORLD_BOARD_H_
#define _HELLO_WORLD_BOARD_H_

#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CPU_CORTEX_M4
#define CPU_CORE_NAME	"Cortex-M4"
#define RTOS_CNT	(1)

void BOARD_InitHardware(void);

#endif
