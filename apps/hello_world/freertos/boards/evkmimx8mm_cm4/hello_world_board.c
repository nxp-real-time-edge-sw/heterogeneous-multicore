/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"

void hello_world_board_init(void)
{
    BOARD_InitMemory();

    /* Board specific RDC settings */
    BOARD_RdcInit();

    board_pins_setup();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
}

void print_ram_console_addr(void)
{
}
