/*
 * Copyright 2022,2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "rsc_table.h"

extern int32_t mu_irq_handler(void);

void virtio_board_init(void)
{
    BOARD_RdcInit();

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    BOARD_InitMemory();
    copyResourceTable();
}

int32_t MU_M4_IRQHandler(void)
{
	return mu_irq_handler();
}
