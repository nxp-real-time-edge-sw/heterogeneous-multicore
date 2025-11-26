/*
 * Copyright 2023-2025 NXP
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

extern int32_t mu_irq_handler(void);

phy_rtl8211f_resource_t phy_resource;

void virtio_board_init(void)
{
	/* Init board hardware. */
	/* M7 has its local cache and enabled by default,
	 * need to set smart subsystems (0x28000000 ~ 0x3FFFFFFF)
	 * non-cacheable before accessing this address region */
	BOARD_InitMemory();

	BOARD_RdcInit();

	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();
	copyResourceTable();
}

int32_t MU1_M7_IRQHandler(void)
{
	return mu_irq_handler();
}
