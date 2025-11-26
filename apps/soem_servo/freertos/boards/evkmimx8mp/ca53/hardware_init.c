/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*${header:start}*/
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "app_board.h"
/*${header:end}*/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static phy_resource_t phy_resource;

static struct enet_if_port if_port;

/*! @brief Buffer descriptors should be in non-cacheable region and should be align to "ENET_BUFF_ALIGNMENT". */
AT_NONCACHEABLE_SECTION_ALIGN(static enet_rx_bd_struct_t g_rxBuffDescrip[ENET_RXBD_NUM], ENET_BUFF_ALIGNMENT);
AT_NONCACHEABLE_SECTION_ALIGN(static enet_tx_bd_struct_t g_txBuffDescrip[ENET_TXBD_NUM], ENET_BUFF_ALIGNMENT);

/*
 * The data buffers can be in cacheable region or in non-cacheable region.
 * If use cacheable region, the alignment size should be the maximum size of "CACHE LINE SIZE" and "ENET_BUFF_ALIGNMENT"
 * If use non-cache region, the alignment size is the "ENET_BUFF_ALIGNMENT".
 */
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && (FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL)
SDK_ALIGN(static uint8_t g_enet_rx_buffer[STATIC_BUFF_COUNT][SDK_SIZEALIGN(ENET_RXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)], ENET_BUFF_ALIGNMENT);
SDK_ALIGN(static uint8_t g_enet_tx_buffer[ENET_TXBD_NUM][SDK_SIZEALIGN(ENET_TXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)], ENET_BUFF_ALIGNMENT);
#else
NONCACHEABLE(static uint8_t g_enet_rx_buffer[STATIC_BUFF_COUNT][SDK_SIZEALIGN(ENET_RXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)], ENET_BUFF_ALIGNMENT);
NONCACHEABLE(static uint8_t g_enet_tx_buffer[ENET_TXBD_NUM][SDK_SIZEALIGN(ENET_TXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)], ENET_BUFF_ALIGNMENT);
#endif

static enet_buffer_config_t buffConfig[] = {{
    ENET_RXBD_NUM,
    ENET_TXBD_NUM,
    SDK_SIZEALIGN(ENET_RXBUFF_SIZE, ENET_BUFF_ALIGNMENT),
    SDK_SIZEALIGN(ENET_TXBUFF_SIZE, ENET_BUFF_ALIGNMENT),
    &g_rxBuffDescrip[0],
    &g_txBuffDescrip[0],
    &g_enet_rx_buffer[0][0],
    &g_enet_tx_buffer[0][0],
    true,
    true,
    NULL,
}};

/*******************************************************************************
 * Code
 ******************************************************************************/
static status_t MDIO_Write(uint8_t phyAddr, uint8_t regAddr, uint16_t data)
{
    return ENET_MDIOWrite(BOARD_ENET, phyAddr, regAddr, data);
}

static status_t MDIO_Read(uint8_t phyAddr, uint8_t regAddr, uint16_t *pData)
{
    return ENET_MDIORead(BOARD_ENET, phyAddr, regAddr, pData);
}

/*!
 * @brief Initializes soem interface.
 */

int if_port_init(void)
{
    struct soem_if_port soem_port;

    ENET_SetSMI(BOARD_ENET, ENET_CLOCK_FREQ, false);
    phy_resource.read  = MDIO_Read;
    phy_resource.write = MDIO_Write;
    memset(&if_port, 0, sizeof(if_port));
    if_port.bufferConfig = buffConfig;
    if_port.base         = BOARD_ENET;

    /* The miiMode should be set according to the different PHY interfaces. */
    if_port.mii_mode                   = kENET_RgmiiMode;
    if_port.phy_config.autoNeg       = true;
    if_port.phy_config.phyAddr       = ENET_PHY_ADDRESS;
    if_port.phy_config.resource     = &phy_resource;
    if_port.phy_config.ops           = &ENET_PHY_OPS;
    if_port.srcClock_Hz             = ENET_CLOCK_FREQ;
    if_port.phy_autonego_timeout_count = PHY_AUTONEGO_TIMEOUT_COUNT;
    if_port.phy_stability_delay_us   = PHY_STABILITY_DELAY_US;

    soem_port.port_init = enet_init;
    soem_port.port_send = enet_send;
    soem_port.port_recv = enet_recv;
    soem_port.port_link_status = enet_link_status;
    soem_port.port_close = enet_close;
    strncpy(soem_port.ifname, SOEM_PORT_NAME, SOEM_IF_NAME_MAXLEN);
    strncpy(soem_port.dev_name, "enet", SOEM_DEV_NAME_MAXLEN);
    soem_port.port_pri = &if_port;
    return register_soem_port(&soem_port);
}

void BOARD_InitHardware(void)
{
    BOARD_InitMemory();
    BOARD_RdcInit();
    /* Enable GIC before register any interrupt handler*/
    GIC_Enable();
    board_pins_setup();
    board_clock_setup();
    BOARD_InitDebugConsole();

    return;
}
