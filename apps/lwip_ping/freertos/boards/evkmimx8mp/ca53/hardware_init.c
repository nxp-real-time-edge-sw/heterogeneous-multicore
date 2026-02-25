/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "os/irq.h"
#include "ethernetif.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "app_enet.h"
#include "app_board.h"

phy_resource_t phy_resource;

static status_t MDIO_Write(uint8_t phyAddr, uint8_t regAddr, uint16_t data)
{
	return ENET_MDIOWrite(ENET, phyAddr, regAddr, data);
}

static status_t MDIO_Read(uint8_t phyAddr, uint8_t regAddr, uint16_t *pData)
{
	return ENET_MDIORead(ENET, phyAddr, regAddr, pData);
}

static void MDIO_Init(uint32_t enet_ipg_freq)
{
	ENET_SetSMI(ENET, enet_ipg_freq, false);

	phy_resource.write = MDIO_Write;
	phy_resource.read = MDIO_Read;
}

void BOARD_InitHardware(void)
{
	uint32_t enet_ipg_freq;

	BOARD_InitMemory();
	BOARD_RdcInit();
	/* Enable GIC before register any interrupt handler*/
	GIC_Enable();
	board_pins_setup();
	board_clock_setup();
	BOARD_InitDebugConsole();

	enet_ipg_freq = ENET_CLOCK_FREQ;
	MDIO_Init(enet_ipg_freq);

	/*
	 * The default value of ENET_PRIORITY is defined in lwip:ethernetif.h, but it can
	 * be customized by defining a different value in lwipopts.h.
	 */
	os_irq_register((unsigned int)ENET_IRQ, enet_irq_handler, NULL, ENET_PRIORITY);
}
