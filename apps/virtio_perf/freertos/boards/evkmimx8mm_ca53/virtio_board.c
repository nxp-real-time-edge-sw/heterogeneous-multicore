/*
 * Copyright 2022-2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

void virtio_board_init(void)
{
    /* Init board cpu and hardware. */
    BOARD_InitMemory();
    clock_config_uart4();
    pin_mux_uart4();
    BOARD_InitDebugConsole();
}
