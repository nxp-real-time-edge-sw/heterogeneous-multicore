/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

/* Init board cpu and hardware. */
void virtio_board_init(void)
{
    BOARD_InitMemory();
    BOARD_RdcInit();
    /* Enable GIC before register any interrupt handler*/
    GIC_Enable(1);
    clock_config_uart4();
    pin_mux_uart4();
    BOARD_InitDebugConsole();
}
