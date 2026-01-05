/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "pin_mux.h"

void BOARD_InitHardware(void)
{
    SystemPlatformInit();
    BOARD_InitDebugConsolePins();
    BOARD_InitBootPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
}
