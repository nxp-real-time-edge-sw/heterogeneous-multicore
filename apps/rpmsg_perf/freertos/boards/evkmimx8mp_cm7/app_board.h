/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_BOARD_H_
#define _APP_BOARD_H_

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "rsc_table.h"

#define CPU_CORE_NAME		"Cortex-M7"

void board_init(void);

#endif /* _APP_BOARD_H_ */
