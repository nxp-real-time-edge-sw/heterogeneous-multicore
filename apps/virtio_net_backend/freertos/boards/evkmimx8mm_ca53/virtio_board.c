/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "clock_config.h"
#include "pin_mux.h"

void virtio_board_init(void)
{
	/* Init board cpu and hardware. */
	BOARD_InitMemory();
	board_pins_setup();
	board_clock_setup();
	BOARD_InitDebugConsole();
}
