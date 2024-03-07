/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "app_board.h"

#define RDC_DISABLE_A53_ACCESS	0xFC
#define RDC_DISABLE_M4_ACCESS	0xF3

#define APP_PowerUpSlot		(5U)
#define APP_PowerDnSlot		(6U)

void gpc_config(void)
{
	/* Configure GPC */
	GPC_Init(BOARD_GPC_BASEADDR, APP_PowerUpSlot, APP_PowerDnSlot);
	GPC_EnableIRQ(BOARD_GPC_BASEADDR, BOARD_MU_IRQ_NUM);
}

static void Peripheral_RdcSetting(void)
{
	rdc_domain_assignment_t assignment = {0};
	rdc_periph_access_config_t periphConfig;

	assignment.domainId = BOARD_DOMAIN_ID;

	/* Only configure the RDC if the RDC peripheral write access is allowed. */
	if ((0x1U & RDC_GetPeriphAccessPolicy(RDC, kRDC_Periph_RDC, assignment.domainId)) != 0U) {
		RDC_SetMasterDomainAssignment(RDC, kRDC_Master_SDMA3_PERIPH, &assignment);
		RDC_SetMasterDomainAssignment(RDC, kRDC_Master_SDMA3_BURST, &assignment);
		RDC_SetMasterDomainAssignment(RDC, kRDC_Master_SDMA3_SPBA2, &assignment);

		RDC_GetDefaultPeriphAccessConfig(&periphConfig);
		/* Do not allow the A53 domain(domain0) to access the following peripherals. */
		periphConfig.periph = kRDC_Periph_UART3;
		RDC_SetPeriphAccessConfig(RDC, &periphConfig);
		periphConfig.periph = kRDC_Periph_UART4;
		RDC_SetPeriphAccessConfig(RDC, &periphConfig);
	}
}

void srtm_init(void)
{
	APP_SRTM_Init();
}

void board_init(void)
{
	BOARD_RdcInit();
	Peripheral_RdcSetting();

	BOARD_InitBootPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();
	BOARD_InitMemory();

	/* Setup clock for uart3 */
	CLOCK_SetRootMux(kCLOCK_RootUart3, kCLOCK_UartRootmuxSysPll1Div10); /* Set UART source to SysPLL1 Div10 80MHZ */
	CLOCK_SetRootDivider(kCLOCK_RootUart3, 1U, 1U);                     /* Set root clock to 80MHZ/ 1= 80MHZ */
	CLOCK_EnableClock(kCLOCK_Uart3);
}
