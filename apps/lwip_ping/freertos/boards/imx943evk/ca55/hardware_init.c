/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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

extern void MSGINTR2_IRQHandler(void *);

static phy_rtl8211f_resource_t s_phy_rtl8211f_res;
static phy_operations_t s_phy_rtl8211f_ops;
static netc_mdio_handle_t s_emdio_handle;

static status_t ENETC_MDIO_Init(void)
{
	status_t result = kStatus_Success;

	netc_mdio_config_t mdioConfig = {
		.isPreambleDisable = false,
		.isNegativeDriven  = false,
		.srcClockHz = CLOCK_GetRate(kCLOCK_Enet),
	};

	/* EMDIO init */
	mdioConfig.mdio.type = kNETC_EMdio;
	result = NETC_MDIOInit(&s_emdio_handle, &mdioConfig);
	if (result != kStatus_Success) {
		PRINTF("%s() failed!\r\n", __func__);
		return result;
	}

	return result;
}

static status_t ENETC_EMDIOWrite(uint8_t phyAddr, uint8_t regAddr, uint16_t data)
{
	return NETC_MDIOWrite(&s_emdio_handle, phyAddr, regAddr, data);
}

static status_t ENETC_EMDIORead(uint8_t phyAddr, uint8_t regAddr, uint16_t *pData)
{
	return NETC_MDIORead(&s_emdio_handle, phyAddr, regAddr, pData);
}

static status_t ENETC_PHY_Init(phy_handle_t *phy_handle, const phy_config_t *config)
{
	status_t result = kStatus_Success;

	/* MDIO init */
	result = ENETC_MDIO_Init();
	if (result != kStatus_Success) {
		return result;
	}

	/* Initialize PHY */
	result = PHY_RTL8211F_Init(phy_handle, config);
	return result;
}

phy_rtl8211f_resource_t *get_netc_phy_res(void)
{
	static bool s_phy_res_init = false;

	if (!s_phy_res_init) {
		s_phy_rtl8211f_res.write = ENETC_EMDIOWrite;
		s_phy_rtl8211f_res.read  = ENETC_EMDIORead;
		s_phy_res_init = true;
	}
	return &s_phy_rtl8211f_res;
}

phy_operations_t *get_netc_phy_ops(void)
{
	static bool s_phy_ops_init = false;

	if (!s_phy_ops_init) {
		memcpy(&s_phy_rtl8211f_ops, &phyrtl8211f_ops, sizeof(phy_operations_t));
		s_phy_rtl8211f_ops.phyInit = ENETC_PHY_Init;
		s_phy_ops_init = true;
	}

	return &s_phy_rtl8211f_ops;
}

void BOARD_InitHardware(void)
{
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

	/* Select ETH signals to use */
	BOARD_EXPANDER_SetPinAsOutput(BOARD_PCA6416_I2C6_S3_ID, ETH2_SEL);
	BOARD_EXPANDER_SetPinAsOutput(BOARD_PCA6416_I2C6_S3_ID, ETH3_SEL);
	BOARD_EXPANDER_SetPinAsOutput(BOARD_PCA6416_I2C6_S3_ID, ETH4_SEL);

	BOARD_EXPANDER_SetPinToHigh(BOARD_PCA6416_I2C6_S3_ID, ETH2_SEL);
	BOARD_EXPANDER_SetPinToHigh(BOARD_PCA6416_I2C6_S3_ID, ETH3_SEL);
	BOARD_EXPANDER_SetPinToHigh(BOARD_PCA6416_I2C6_S3_ID, ETH4_SEL);

	/* PHY reset */
	BOARD_EXPANDER_SetPinAsOutput(BOARD_PCA6416_I2C3_S5_21_ID, ETH2_RST_B);
	BOARD_EXPANDER_SetPinAsOutput(BOARD_PCA6416_I2C3_S5_21_ID, ETH3_RST_B);
	BOARD_EXPANDER_SetPinAsOutput(BOARD_PCA6416_I2C3_S5_21_ID, ETH4_RST_B);

	BOARD_EXPANDER_SetPinToLow(BOARD_PCA6416_I2C3_S5_21_ID, ETH2_RST_B);
	SDK_DelayAtLeastUs(20000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
	BOARD_EXPANDER_SetPinToHigh(BOARD_PCA6416_I2C3_S5_21_ID, ETH2_RST_B);
	SDK_DelayAtLeastUs(100000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

	BOARD_EXPANDER_SetPinToLow(BOARD_PCA6416_I2C3_S5_21_ID, ETH3_RST_B);
	SDK_DelayAtLeastUs(20000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
	BOARD_EXPANDER_SetPinToHigh(BOARD_PCA6416_I2C3_S5_21_ID, ETH3_RST_B);
	SDK_DelayAtLeastUs(100000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

	BOARD_EXPANDER_SetPinToLow(BOARD_PCA6416_I2C3_S5_21_ID, ETH4_RST_B);
	SDK_DelayAtLeastUs(20000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
	BOARD_EXPANDER_SetPinToHigh(BOARD_PCA6416_I2C3_S5_21_ID, ETH4_RST_B);
	SDK_DelayAtLeastUs(100000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

	/*
	 * PCS(Physical Coding Sublayer) protocols on link0-5,
	 * xxxx xxxx xxxx xxx1: 1G SGMII
	 * xxxx xxxx xxxx xx1x: OC-SGMII(i.e.: OverClock 2.5 G SGMII)
	 */
	BLK_CTRL_NETCMIX->CFG_LINK_PCS_PROT_0 |= BLK_CTRL_NETCMIX_CFG_LINK_PCS_PROT_0_CFG_LINK_PCS_PROT_0(2U); /* OverClock 2.5 G SGMII */
	BLK_CTRL_NETCMIX->CFG_LINK_PCS_PROT_1 |= BLK_CTRL_NETCMIX_CFG_LINK_PCS_PROT_1_CFG_LINK_PCS_PROT_1(2U); /* OverClock 2.5 G SGMII */
	BLK_CTRL_NETCMIX->CFG_LINK_PCS_PROT_2 |= BLK_CTRL_NETCMIX_CFG_LINK_PCS_PROT_2_CFG_LINK_PCS_PROT_2(1U); /* 1G SGMII */
	BLK_CTRL_NETCMIX->CFG_LINK_PCS_PROT_3 |= BLK_CTRL_NETCMIX_CFG_LINK_PCS_PROT_3_CFG_LINK_PCS_PROT_3(1U); /* 1G SGMII */
	BLK_CTRL_NETCMIX->CFG_LINK_PCS_PROT_4 |= BLK_CTRL_NETCMIX_CFG_LINK_PCS_PROT_4_CFG_LINK_PCS_PROT_4(1U); /* 1G SGMII */
	BLK_CTRL_NETCMIX->CFG_LINK_PCS_PROT_5 |= BLK_CTRL_NETCMIX_CFG_LINK_PCS_PROT_5_CFG_LINK_PCS_PROT_5(1U); /* 1G SGMII */

	/*
	 * MII protocol for port0~5
	 * 0b0000..MII
	 * 0b0001..RMII
	 * 0b0010..RGMII
	 * 0b0011..SGMII
	 * 0b0100~0b1111..Reserved
	 */
	BLK_CTRL_NETCMIX->NETC_LINK_CFG0 |= BLK_CTRL_NETCMIX_NETC_LINK_CFG0_MII_PROT(0x3U); /* SGMII */
	BLK_CTRL_NETCMIX->NETC_LINK_CFG1 |= BLK_CTRL_NETCMIX_NETC_LINK_CFG1_MII_PROT(0x3U); /* SGMII */
	BLK_CTRL_NETCMIX->NETC_LINK_CFG2 |= BLK_CTRL_NETCMIX_NETC_LINK_CFG2_MII_PROT(0x2U); /* RGMII */
	BLK_CTRL_NETCMIX->NETC_LINK_CFG3 |= BLK_CTRL_NETCMIX_NETC_LINK_CFG3_MII_PROT(0x2U); /* RGMII */
	BLK_CTRL_NETCMIX->NETC_LINK_CFG4 |= BLK_CTRL_NETCMIX_NETC_LINK_CFG4_MII_PROT(0x2U); /* RGMII */
	BLK_CTRL_NETCMIX->NETC_LINK_CFG5 |= BLK_CTRL_NETCMIX_NETC_LINK_CFG5_MII_PROT(0x2U); /* RGMII */

	/*
	 * ETH2 selection: MAC2(switch port2) or MAC3(enetc1)
	 * 0b - MAC2 selected
	 * 1b - MAC3 selected
	 */
	BLK_CTRL_NETCMIX->EXT_PIN_CONTROL &= ~BLK_CTRL_NETCMIX_EXT_PIN_CONTROL_mac2_mac3_sel(1U);

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

	os_irq_register(MSGINTR2_IRQn, MSGINTR2_IRQHandler, NULL, portLOWEST_USABLE_INTERRUPT_PRIORITY - 1);
	os_irq_enable(MSGINTR2_IRQn);

	return;
}
