/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020,2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "lwip/opt.h"

#if LWIP_IPV4 && LWIP_RAW && LWIP_SOCKET

#include "os/stdio.h"
#include "os/unistd.h"
#include "os/assert.h"
#include "os/irq.h"

#include "lwip/netifapi.h"
#include "lwip/tcpip.h"
#include "netif/ethernet.h"
#include "ethernetif.h"
#include "ping.h"

#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "app_enet.h"

#ifndef configMAC_ADDR
#include "fsl_silicon_id.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* @TEST_ANCHOR */

/* IP address configuration. */
#ifndef configIP_ADDR0
#define configIP_ADDR0 192
#endif
#ifndef configIP_ADDR1
#define configIP_ADDR1 168
#endif
#ifndef configIP_ADDR2
#define configIP_ADDR2 0
#endif
#ifndef configIP_ADDR3
#define configIP_ADDR3 100
#endif

/* Netmask configuration. */
#ifndef configNET_MASK0
#define configNET_MASK0 255
#endif
#ifndef configNET_MASK1
#define configNET_MASK1 255
#endif
#ifndef configNET_MASK2
#define configNET_MASK2 255
#endif
#ifndef configNET_MASK3
#define configNET_MASK3 0
#endif

/* Gateway address configuration. */
#ifndef configGW_ADDR0
#define configGW_ADDR0 192
#endif
#ifndef configGW_ADDR1
#define configGW_ADDR1 168
#endif
#ifndef configGW_ADDR2
#define configGW_ADDR2 0
#endif
#ifndef configGW_ADDR3
#define configGW_ADDR3 254
#endif

#ifndef EXAMPLE_NETIF_INIT_FN
/*! @brief Network interface initialization function. */
#define EXAMPLE_NETIF_INIT_FN ethernetif0_init
#endif /* EXAMPLE_NETIF_INIT_FN */

/*! @brief Stack size of the temporary lwIP initialization thread. */
#define INIT_THREAD_STACKSIZE 1024

/*! @brief Priority of the temporary lwIP initialization thread. */
#define INIT_THREAD_PRIO DEFAULT_THREAD_PRIO

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static phy_resource_t phy_resource;
static phy_handle_t phyHandle;

/*******************************************************************************
 * Code
 ******************************************************************************/
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

/*!
 * @brief Initializes lwIP stack.
 */
static void stack_init(void *arg)
{
	static struct netif netif;
	ip4_addr_t netif_ipaddr, netif_netmask, netif_gw;
	ethernetif_config_t enet_config = {
		.phyHandle   = &phyHandle,
		.phyAddr     = ENET_PHY_ADDRESS,
		.phyOps      = &ENET_PHY_OPS,
		.phyResource = &phy_resource,
		.srcClockHz  = ENET_CLOCK_FREQ,
#ifdef configMAC_ADDR
		.macAddress = configMAC_ADDR
#endif
	};

	LWIP_UNUSED_ARG(arg);

	/* Set MAC address. */
#ifndef configMAC_ADDR
	(void)SILICONID_ConvertToMacAddr(&enet_config.macAddress);
#endif

	IP4_ADDR(&netif_ipaddr, configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3);
	IP4_ADDR(&netif_netmask, configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3);
	IP4_ADDR(&netif_gw, configGW_ADDR0, configGW_ADDR1, configGW_ADDR2, configGW_ADDR3);

	tcpip_init(NULL, NULL);

	netifapi_netif_add(&netif, &netif_ipaddr, &netif_netmask, &netif_gw, &enet_config,
			   EXAMPLE_NETIF_INIT_FN, tcpip_input);
	netifapi_netif_set_default(&netif);
	netifapi_netif_set_up(&netif);

	while (ethernetif_wait_linkup(&netif, 5000) != ERR_OK)
		os_printf("PHY Auto-negotiation failed. Please check the cable connection and link partner setting.\r\n");

	os_printf("\r\n************************************************\r\n");
	os_printf(" PING example\r\n");
	os_printf("************************************************\r\n");
	os_printf(" IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&netif_ipaddr)[0], ((u8_t *)&netif_ipaddr)[1],
		((u8_t *)&netif_ipaddr)[2], ((u8_t *)&netif_ipaddr)[3]);
	os_printf(" IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&netif_netmask)[0], ((u8_t *)&netif_netmask)[1],
		((u8_t *)&netif_netmask)[2], ((u8_t *)&netif_netmask)[3]);
	os_printf(" IPv4 Gateway     : %u.%u.%u.%u\r\n", ((u8_t *)&netif_gw)[0], ((u8_t *)&netif_gw)[1],
		((u8_t *)&netif_gw)[2], ((u8_t *)&netif_gw)[3]);
	os_printf("************************************************\r\n");

	ping_init(&netif_gw);

	vTaskDelete(NULL);
}

int main(void)
{
	uint32_t enet_ipg_freq;

	BOARD_InitMemory();
	BOARD_RdcInit();
	/* Enable GIC before register any interrupt handler*/
	GIC_Enable(1);
	board_pins_setup();
	board_clock_setup();
	BOARD_InitDebugConsole();

	enet_ipg_freq = ENET_CLOCK_FREQ;
	MDIO_Init(enet_ipg_freq);

	/*
	 * The default value of ENET_PRIORITY is defined in lwip:ethernetif.h, but it can
	 * be customized by defining a different value in lwipopts.h.
	 */
	os_irq_register(ENET_IRQ, enet_irq_handler, NULL, (ENET_PRIORITY >> portPRIORITY_SHIFT));

	/* Initialize lwIP from thread */
	if (sys_thread_new("main", stack_init, NULL, INIT_THREAD_STACKSIZE, INIT_THREAD_PRIO) == NULL)
		os_assert(0, "main(): Task creation failed.");

	vTaskStartScheduler();

	for (;;)
		;

	/* Will not get here unless a task calls vTaskEndScheduler ()*/
	return 0;
}
#endif
