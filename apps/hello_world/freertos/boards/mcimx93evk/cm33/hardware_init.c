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

void BOARD_InitHardware(void)
{
    board_InitBootPins();
    board_clock_setup();
    BOARD_InitDebugConsole();
    BOARD_ConfigMPU();
    copyResourceTable();
}
