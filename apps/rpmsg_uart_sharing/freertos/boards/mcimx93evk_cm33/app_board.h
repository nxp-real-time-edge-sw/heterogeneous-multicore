/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_BOARD_H_
#define _APP_BOARD_H_

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "rsc_table.h"
#include "fsl_debug_console.h"
#include "app_srtm.h"

void board_init(void);
void srtm_init(void);

#endif /* _APP_BOARD_H_ */
