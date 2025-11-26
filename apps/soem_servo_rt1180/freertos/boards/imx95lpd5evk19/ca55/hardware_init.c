/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*${header:start}*/
#include "app_board.h"
#include "os/stdio.h"
#include "os/irq.h"
#include "FreeRTOS.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "rtos_memory.h"
#include "sm_platform.h"
/*${header:end}*/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Buffer descriptor and buffer memeory. */
typedef uint8_t rx_buffer_t[EP_RXBUFF_SIZE_ALIGN];

AT_NONCACHEABLE_SECTION_ALIGN(static netc_rx_bd_t g_rxBuffDescrip[EP_RING_NUM][EP_RXBD_NUM], EP_BD_ALIGN);
AT_NONCACHEABLE_SECTION_ALIGN(static netc_tx_bd_t g_txBuffDescrip[EP_RING_NUM][EP_TXBD_NUM], EP_BD_ALIGN);
AT_NONCACHEABLE_SECTION_ALIGN(static rx_buffer_t g_rxDataBuff[EP_RING_NUM][EP_RXBD_NUM], EP_BUFF_SIZE_ALIGN);
AT_NONCACHEABLE_SECTION_ALIGN(static uint8_t g_txFrame[EP_TXBUFF_SIZE], EP_BUFF_SIZE_ALIGN);
uint64_t rxBuffAddrArray[EP_RING_NUM][EP_RXBD_NUM];

static netc_tx_frame_info_t g_txDirty[EP_RING_NUM][EP_TXBD_NUM];
static netc_tx_frame_info_t txFrameInfo = {0};

static netc_rx_bdr_config_t rxBdrConfig = {0};
static netc_tx_bdr_config_t txBdrConfig = {0};
static netc_bdr_config_t bdrConfig;
static netc_msix_entry_t msixEntry[2];
static ep_config_t ep_config;

static struct netc_ep_if_port if_port;

static netc_mdio_handle_t s_mdio_handle;
static netc_mdio_handle_t s_emdio_handle;
static phy_rtl8211f_resource_t s_phy_rtl8211f_resource;


/*******************************************************************************
 * Code
 ******************************************************************************/
extern void MSGINTR2_IRQHandler(void *);

static status_t ReclaimCallback(ep_handle_t *handle, uint8_t ring, netc_tx_frame_info_t *frameInfo, void *userData)
{
    txFrameInfo = *frameInfo;
    return kStatus_Success;
}

void msgintrCallback(MSGINTR_Type *base, uint8_t channel, uint32_t pendingIntr)
{
    /* Transmit interrupt */
    if ((pendingIntr & (1U << TX_INTR_MSG_DATA)) != 0U)
    {
        EP_CleanTxIntrFlags(&if_port.g_ep_handle, 1, 0);
        if_port.txOver = true;
    }
    /* Receive interrupt */
    if ((pendingIntr & (1U << RX_INTR_MSG_DATA)) != 0U)
    {
        EP_CleanRxIntrFlags(&if_port.g_ep_handle, 1);
    }
}

/*!
 * @brief Initializes soem interface.
 */

int if_port_init(void)
{
    struct soem_if_port soem_port;
    bdrConfig.rxBdrConfig = &rxBdrConfig;
    bdrConfig.txBdrConfig = &txBdrConfig;
    uint32_t msgAddr;

    os_irq_register(MSGINTR2_IRQn, MSGINTR2_IRQHandler, NULL, portLOWEST_USABLE_INTERRUPT_PRIORITY - 1);
    os_irq_enable(MSGINTR2_IRQn);

    /* MSIX and interrupt configuration. */
    MSGINTR_Init(MSGINTR2, &msgintrCallback);
    msgAddr           = MSGINTR_GetIntrSelectAddr(MSGINTR2, 0);
    msixEntry[0].control = kNETC_MsixIntrMaskBit;
    msixEntry[0].msgAddr = msgAddr;
    msixEntry[0].msgData = TX_INTR_MSG_DATA;
    msixEntry[1].control = kNETC_MsixIntrMaskBit;
    msixEntry[1].msgAddr = msgAddr;
    msixEntry[1].msgData = RX_INTR_MSG_DATA;

    /* BD ring configuration. */
    bdrConfig.rxBdrConfig[0].bdArray       = &g_rxBuffDescrip[0][0];
    bdrConfig.rxBdrConfig[0].len           = EP_RXBD_NUM;
    bdrConfig.rxBdrConfig[0].buffAddrArray = &rxBuffAddrArray[0][0];
    bdrConfig.rxBdrConfig[0].buffSize     = EP_RXBUFF_SIZE_ALIGN;
    bdrConfig.rxBdrConfig[0].msixEntryIdx  = RX_MSIX_ENTRY_IDX;
    bdrConfig.rxBdrConfig[0].extendDescEn  = false;
    bdrConfig.rxBdrConfig[0].enThresIntr   = true;
    bdrConfig.rxBdrConfig[0].enCoalIntr = true;
    bdrConfig.rxBdrConfig[0].intrThreshold = 1;

    bdrConfig.txBdrConfig[0].bdArray      = &g_txBuffDescrip[0][0];
    bdrConfig.txBdrConfig[0].len          = EP_TXBD_NUM;
    bdrConfig.txBdrConfig[0].dirtyArray   = &g_txDirty[0][0];
    bdrConfig.txBdrConfig[0].msixEntryIdx = TX_MSIX_ENTRY_IDX;
    bdrConfig.txBdrConfig[0].enIntr    = true;

    /* Endpoint configuration. */
    (void)EP_GetDefaultConfig(&ep_config);
    ep_config.si                    = kNETC_ENETC0PSI0;
    ep_config.siConfig.txRingUse    = 1;
    ep_config.siConfig.rxRingUse    = 1;
    ep_config.reclaimCallback      = ReclaimCallback;
    ep_config.msixEntry          = &msixEntry[0];
    ep_config.entryNum            = 2;
    ep_config.port.ethMac.miiMode   = kNETC_RgmiiMode;
    ep_config.port.ethMac.miiSpeed  = kNETC_MiiSpeed100M;
    ep_config.port.ethMac.miiDuplex = kNETC_MiiFullDuplex;
#ifdef EXAMPLE_ENABLE_CACHE_MAINTAIN
    ep_config.rxCacheMaintain = true;
    ep_config.txCacheMaintain = true;
#endif

    for (uint8_t ring = 0U; ring < EP_RING_NUM; ring++)
    {
        for (uint8_t index = 0U; index < EP_RXBD_NUM; index++)
        {
            rxBuffAddrArray[ring][index] = (uint64_t)(uintptr_t)&g_rxDataBuff[ring][index];
        }
    }

    if_port.ep_config = &ep_config;
    if_port.bdrConfig = &bdrConfig;
    if_port.g_txFrame = g_txFrame;
    strncpy(soem_port.ifname, SOEM_PORT_NAME, SOEM_IF_NAME_MAXLEN);
    strncpy(soem_port.dev_name, "netc_ep", SOEM_DEV_NAME_MAXLEN);
    soem_port.port_init = netc_ep_init;
    soem_port.port_send = netc_ep_send;
    soem_port.port_recv = netc_ep_recv;
    soem_port.port_close = netc_ep_close;
    soem_port.port_link_status= netc_ep_link_status;
    soem_port.port_pri = &if_port;
    return register_soem_port(&soem_port);
    return 0;
}

status_t APP_MDIO_Init(void)
{
    status_t result = kStatus_Success;

    netc_mdio_config_t mdioConfig = {
        .isPreambleDisable = false,
        .isNegativeDriven  = false,
        .srcClockHz        = HAL_ClockGetIpFreq(hal_clock_enet),
    };

    /* EMDIO init */
    mdioConfig.mdio.type = kNETC_EMdio;
    result               = NETC_MDIOInit(&s_emdio_handle, &mdioConfig);
    if (result != kStatus_Success)
    {
        return result;
    }

    /* Internal MDIO init */
    ENETC2_PCI_HDR_TYPE0->PCI_CFH_CMD |=
        (ENETC_PCI_TYPE0_PCI_CFH_CMD_MEM_ACCESS_MASK | ENETC_PCI_TYPE0_PCI_CFH_CMD_BUS_MASTER_EN_MASK);

    mdioConfig.mdio.type = kNETC_InternalMdio;
    mdioConfig.mdio.port = kNETC_ENETC2EthPort;
    result               = NETC_MDIOInit(&s_mdio_handle, &mdioConfig);
    return result;
}

static status_t NETC_EP_EMDIOWrite(uint8_t phyAddr, uint8_t regAddr, uint16_t data)
{
    return NETC_MDIOWrite(&if_port.s_emdio_handle, phyAddr, regAddr, data);
}

static status_t NETC_EP_EMDIORead(uint8_t phyAddr, uint8_t regAddr, uint16_t *pData)
{
    return NETC_MDIORead(&if_port.s_emdio_handle, phyAddr, regAddr, pData);
}
/* PHY operation. */


status_t NETC_EP_PHY_Init(void)
{
    status_t result            = kStatus_Success;

    /* EP0 PHY Init */
    phy_config_t phy8211Config = {
        .autoNeg   = true,
        .speed     = kPHY_Speed1000M,
        .duplex    = kPHY_FullDuplex,
        .enableEEE = false,
        .ops       = &phyrtl8211f_ops,
    };

    /* For a complete PHY reset of RTL8211FDI-CG, this pin must be asserted low for at least 10ms. And
     * wait for a further 72ms(for internal circuits settling time) before accessing the PHY register */
    pcal6408_handle_t handle;
    BOARD_InitPCAL6408_I2C5(&handle);
    PCAL6408_SetDirection(&handle, (1 << BOARD_PCAL6408_ENET1_RST_B), kPCAL6408_Output);
    PCAL6408_ClearPins(&handle, (1 << BOARD_PCAL6408_ENET1_RST_B));

    SDK_DelayAtLeastUs(100000, 1800000000UL);
    PCAL6408_SetPins(&handle, (1 << BOARD_PCAL6408_ENET1_RST_B));
    SDK_DelayAtLeastUs(100000, 1800000000UL);

    /* Initialize PHY for EP0. */
    s_phy_rtl8211f_resource.write = NETC_EP_EMDIOWrite;
    s_phy_rtl8211f_resource.read  = NETC_EP_EMDIORead;
    phy8211Config.resource = &s_phy_rtl8211f_resource;
    phy8211Config.phyAddr  = EP0_PHY_ADDR;
    result = PHY_Init(&if_port.phy_handle, &phy8211Config);
    if (result != kStatus_Success)
    {
        return result;
    }
    SDK_DelayAtLeastUs(200000, 1800000000UL);
    return result;
}

void BOARD_InitHardware(void)
{
    status_t result = kStatus_Success;	
    pcal6524_handle_t handle1;
    pcal6408_handle_t handle2;

    IRQn_Type const s_muIrqs[] = MU_IRQS;
    IRQn_Type irq = s_muIrqs[SM_PLATFORM_MU_INST];
    /* Init board cpu and hardware. */
    BOARD_InitMemory();
    /* Enable GIC before register any interrupt handler*/
    GIC_Enable();
    SM_Platform_Init();
    os_irq_register(irq, SM_platform_MU_IRQHandler, NULL, portLOWEST_USABLE_INTERRUPT_PRIORITY - 1);

    board_pins_setup();
    board_clock_setup();
    BOARD_InitDebugConsole();
    /* Enable 156.25MHz clock to 10G ETH_CLKIN_P/ETH_CLKIN_N */
    BOARD_InitPCAL6524(&handle1);
    PCAL6524_SetDirection(&handle1, (1 << BOARD_PCAL6524_SI5332_RST), kPCAL6524_Output);
    PCAL6524_ClearPins(&handle1, (1 << BOARD_PCAL6524_SI5332_RST));
    SDK_DelayAtLeastUs(200000, 1800000000UL);

    BOARD_InitPCAL6408_I2C5(&handle2);
    PCAL6408_SetDirection(&handle2, (1 << BOARD_PCAL6408_ETH_CLK_EN), kPCAL6408_Output);
    PCAL6408_ClearPins(&handle2, (1 << BOARD_PCAL6408_ETH_CLK_EN));
    SDK_DelayAtLeastUs(200000, 1800000000UL);

    /* Protocol configure */
    BLK_CTRL_NETCMIX->CFG_LINK_MII_PROT = 0x00000522;
    BLK_CTRL_NETCMIX->CFG_LINK_PCS_PROT_2 = 0x00000040;

    /* Unlock the IERB. It will warm reset whole NETC. */
    NETC_PRIV->NETCRR &= ~NETC_PRIV_NETCRR_LOCK_MASK;
    while ((NETC_PRIV->NETCRR & NETC_PRIV_NETCRR_LOCK_MASK) != 0U)
    {
    }

    /* Lock the IERB. */
    NETC_PRIV->NETCRR |= NETC_PRIV_NETCRR_LOCK_MASK;
    while ((NETC_PRIV->NETCSR & NETC_PRIV_NETCSR_STATE_MASK) != 0U)
    {
    }

    result = APP_MDIO_Init();
    if (result != kStatus_Success)
    {
        return;
    }

    result = NETC_EP_PHY_Init();
    if (result != kStatus_Success)
    {
        return;
    }

    bool link = false;
    do
    {
        PHY_GetLinkStatus(&if_port.phy_handle, &link);
    } while (!link);
    return;
}
