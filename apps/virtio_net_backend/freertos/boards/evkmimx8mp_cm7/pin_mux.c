/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "pin_mux.h"

static void pin_mux_enet(void)
{
	IOMUXC_SetPinMux(IOMUXC_SAI1_RXD0_GPIO4_IO02, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_RXD0_GPIO4_IO02, 0U);
	IOMUXC_SetPinMux(IOMUXC_SAI1_RXD2_ENET1_MDC, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_RXD2_ENET1_MDC,
			IOMUXC_SW_PAD_CTL_PAD_DSE(1U));
	IOMUXC_SetPinMux(IOMUXC_SAI1_RXD3_ENET1_MDIO, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_RXD3_ENET1_MDIO,
			IOMUXC_SW_PAD_CTL_PAD_DSE(1U));
	IOMUXC_SetPinMux(IOMUXC_SAI1_RXD4_ENET1_RGMII_RD0, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_RXD4_ENET1_RGMII_RD0,
			IOMUXC_SW_PAD_CTL_PAD_DSE(3U) |
			IOMUXC_SW_PAD_CTL_PAD_FSEL_MASK |
			IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_SAI1_RXD5_ENET1_RGMII_RD1, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_RXD5_ENET1_RGMII_RD1,
			IOMUXC_SW_PAD_CTL_PAD_DSE(3U) |
			IOMUXC_SW_PAD_CTL_PAD_FSEL_MASK |
			IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_SAI1_RXD6_ENET1_RGMII_RD2, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_RXD6_ENET1_RGMII_RD2,
			IOMUXC_SW_PAD_CTL_PAD_DSE(3U) |
			IOMUXC_SW_PAD_CTL_PAD_FSEL_MASK |
			IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_SAI1_RXD7_ENET1_RGMII_RD3, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_RXD7_ENET1_RGMII_RD3,
			IOMUXC_SW_PAD_CTL_PAD_DSE(3U) |
			IOMUXC_SW_PAD_CTL_PAD_FSEL_MASK |
			IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_SAI1_TXC_ENET1_RGMII_RXC, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_TXC_ENET1_RGMII_RXC,
			IOMUXC_SW_PAD_CTL_PAD_DSE(3U) |
			IOMUXC_SW_PAD_CTL_PAD_FSEL_MASK |
			IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_SAI1_TXD0_ENET1_RGMII_TD0, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_TXD0_ENET1_RGMII_TD0,
			IOMUXC_SW_PAD_CTL_PAD_DSE(3U) |
			IOMUXC_SW_PAD_CTL_PAD_FSEL_MASK);
	IOMUXC_SetPinMux(IOMUXC_SAI1_TXD1_ENET1_RGMII_TD1, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_TXD1_ENET1_RGMII_TD1,
			IOMUXC_SW_PAD_CTL_PAD_DSE(3U) |
			IOMUXC_SW_PAD_CTL_PAD_FSEL_MASK);
	IOMUXC_SetPinMux(IOMUXC_SAI1_TXD2_ENET1_RGMII_TD2, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_TXD2_ENET1_RGMII_TD2,
			IOMUXC_SW_PAD_CTL_PAD_DSE(3U) |
			IOMUXC_SW_PAD_CTL_PAD_FSEL_MASK);
	IOMUXC_SetPinMux(IOMUXC_SAI1_TXD3_ENET1_RGMII_TD3, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_TXD3_ENET1_RGMII_TD3,
			IOMUXC_SW_PAD_CTL_PAD_DSE(3U) |
			IOMUXC_SW_PAD_CTL_PAD_FSEL_MASK);
	IOMUXC_SetPinMux(IOMUXC_SAI1_TXD4_ENET1_RGMII_TX_CTL, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_TXD4_ENET1_RGMII_TX_CTL,
			IOMUXC_SW_PAD_CTL_PAD_DSE(3U) |
			IOMUXC_SW_PAD_CTL_PAD_FSEL_MASK);
	IOMUXC_SetPinMux(IOMUXC_SAI1_TXD5_ENET1_RGMII_TXC, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_TXD5_ENET1_RGMII_TXC,
			IOMUXC_SW_PAD_CTL_PAD_DSE(3U) |
			IOMUXC_SW_PAD_CTL_PAD_FSEL_MASK);
	IOMUXC_SetPinMux(IOMUXC_SAI1_TXFS_ENET1_RGMII_RX_CTL, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SAI1_TXFS_ENET1_RGMII_RX_CTL,
			IOMUXC_SW_PAD_CTL_PAD_DSE(3U) |
			IOMUXC_SW_PAD_CTL_PAD_FSEL_MASK |
			IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
}


/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void)
{
	BOARD_InitPins();
}

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: cm7}
- pin_list:
  - {pin_num: AJ5, peripheral: UART4, signal: uart_rx, pin_signal: UART4_RXD, PE: Enabled, HYS: CMOS, PUE: Weak_Pull_Up, FSEL: Slow, DSE: X1}
  - {pin_num: AH5, peripheral: UART4, signal: uart_tx, pin_signal: UART4_TXD, PE: Enabled, HYS: CMOS, PUE: Weak_Pull_Up, FSEL: Slow, DSE: X1}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPins(void) {                                /*!< Function assigned for the core: Cortex-M7F[m7] */
	IOMUXC_SetPinMux(IOMUXC_UART4_RXD_UART4_RX, 0U);
	IOMUXC_SetPinConfig(IOMUXC_UART4_RXD_UART4_RX,
			IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
			IOMUXC_SW_PAD_CTL_PAD_PE_MASK);
	IOMUXC_SetPinMux(IOMUXC_UART4_TXD_UART4_TX, 0U);
	IOMUXC_SetPinConfig(IOMUXC_UART4_TXD_UART4_TX,
			IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
			IOMUXC_SW_PAD_CTL_PAD_PE_MASK);
	pin_mux_enet();
}
