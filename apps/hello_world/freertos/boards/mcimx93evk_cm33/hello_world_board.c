/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "rsc_table.h"

void hello_world_board_init(void)
{
    board_pins_setup();
    board_clock_setup();
    BOARD_InitDebugConsole();
    BOARD_ConfigMPU();
    copyResourceTable();
}
