/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_BOARD_H_
#define _APP_BOARD_H_

#include "os/stdio.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_clock.h"
#include "rtos_memory.h"

#define CPU_CORE_NAME		"Cortex-A53"
#define RTOS_CNT	(2)

void board_init(void);

#endif /* _APP_BOARD_H_ */
