/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"

static void clock_config_netc(void)
{
}
void board_clock_setup(void)
{
	BOARD_InitClock();
	clock_config_netc();
}
