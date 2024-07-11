/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "rsc_table.h"

void virtio_board_init(void)
{
	/* Init board hardware. */
	/* M7 has its local cache and enabled by default,
	 * need to set smart subsystems (0x28000000 ~ 0x3FFFFFFF)
	 * non-cacheable before accessing this address region */
	BOARD_InitMemory();

	BOARD_RdcInit();

	BOARD_InitBootPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();
	copyResourceTable();
}
