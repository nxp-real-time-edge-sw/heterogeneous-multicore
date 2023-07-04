/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_phyar8031.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

phy_ar8031_resource_t phy_resource;

void virtio_board_init(void)
{
    BOARD_RdcInit();

    BOARD_InitBootPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitMemory();
}
