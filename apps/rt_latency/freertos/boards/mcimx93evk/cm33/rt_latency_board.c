/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "rsc_table.h"
#include "os/counter.h"

extern void tpm_irq_handler(void *irq_dev);

void rt_latency_board_init(void)
{
	board_InitBootPins();
	board_clock_setup();
	BOARD_InitDebugConsole();
	BOARD_ConfigMPU();
	copyResourceTable();
}

void TPM2_IRQHandler(void)
{
	os_counter_t *tpm2 = GET_COUNTER_DEVICE_INSTANCE(0);

	tpm_irq_handler(tpm2);
}

void TPM4_IRQHandler(void)
{
	os_counter_t *tpm4 = GET_COUNTER_DEVICE_INSTANCE(1);

	tpm_irq_handler(tpm4);
}
