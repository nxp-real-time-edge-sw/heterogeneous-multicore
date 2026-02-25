/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_H_
#define _APP_H_

#include <stdint.h>
#include "app_board.h"
#include "app_mmu.h"

#define LPSPI_TEST
#define FLEXCAN_TEST
#define LPI2C_TEST

void BOARD_InitHardware(void);
uint64_t get_core_mpid(void);
int get_test_idx(void);

extern int lpspi_test(void);
extern int flexcan_test(void);
extern int lpi2c_test(void);

#endif
