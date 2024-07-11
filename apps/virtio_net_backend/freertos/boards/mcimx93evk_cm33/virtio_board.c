/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_phyrtl8211f.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "rsc_table.h"

phy_rtl8211f_resource_t phy_resource;

void virtio_board_init(void)
{
    board_pins_setup();
    board_clock_setup();
    BOARD_InitDebugConsole();
    BOARD_ConfigMPU();
    copyResourceTable();
}
