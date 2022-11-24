/*
 * Copyright 2018-2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v9.0
processor: MIMX8MM6xxxLZ
package_id: MIMX8MM6DVTLZ
mcu_data: ksdk2_0
processor_version: 9.0.0

 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "pin_mux.h"

static void pin_mux_enet(void)
{
    IOMUXC_SetPinMux(IOMUXC_ENET_MDC_ENET1_MDC, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_MDC_ENET1_MDC,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(3U));
    IOMUXC_SetPinMux(IOMUXC_ENET_MDIO_ENET1_MDIO, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_MDIO_ENET1_MDIO,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(3U));

    IOMUXC_SetPinMux(IOMUXC_ENET_RD0_ENET1_RGMII_RD0, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_RD0_ENET1_RGMII_RD0,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS(1U));
    IOMUXC_SetPinMux(IOMUXC_ENET_RD1_ENET1_RGMII_RD1, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_RD1_ENET1_RGMII_RD1,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS(1U));
    IOMUXC_SetPinMux(IOMUXC_ENET_RD2_ENET1_RGMII_RD2, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_RD2_ENET1_RGMII_RD2,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS(1U));
    IOMUXC_SetPinMux(IOMUXC_ENET_RD3_ENET1_RGMII_RD3, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_RD3_ENET1_RGMII_RD3,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS(1U));

    IOMUXC_SetPinMux(IOMUXC_ENET_RX_CTL_ENET1_RGMII_RX_CTL, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_RX_CTL_ENET1_RGMII_RX_CTL,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS(1U));

    IOMUXC_SetPinMux(IOMUXC_ENET_RXC_ENET1_RGMII_RXC, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_RXC_ENET1_RGMII_RXC,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS(1U));

    IOMUXC_SetPinMux(IOMUXC_ENET_TD0_ENET1_RGMII_TD0, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_TD0_ENET1_RGMII_TD0,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(3U));
    IOMUXC_SetPinMux(IOMUXC_ENET_TD1_ENET1_RGMII_TD1, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_TD1_ENET1_RGMII_TD1,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(3U));
    IOMUXC_SetPinMux(IOMUXC_ENET_TD2_ENET1_RGMII_TD2, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_TD2_ENET1_RGMII_TD2,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(3U));
    IOMUXC_SetPinMux(IOMUXC_ENET_TD3_ENET1_RGMII_TD3, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_TD3_ENET1_RGMII_TD3,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(3U));

    IOMUXC_SetPinMux(IOMUXC_ENET_TX_CTL_ENET1_RGMII_TX_CTL, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_TX_CTL_ENET1_RGMII_TX_CTL,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(3U));

    IOMUXC_SetPinMux(IOMUXC_ENET_TXC_ENET1_RGMII_TXC, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ENET_TXC_ENET1_RGMII_TXC,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(3U));
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
- options: {callFromInitBoot: 'true', prefix: BOARD_, coreID: cm4}
- pin_list:
  - {pin_num: F19, peripheral: UART4, signal: uart_rx, pin_signal: UART4_RXD, PE: Disabled, PUE: Disabled, DSE: X6_0}
  - {pin_num: F18, peripheral: UART4, signal: uart_tx, pin_signal: UART4_TXD, PE: Disabled, PUE: Disabled, DSE: X6_0}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPins(void) {                                /*!< Function assigned for the core: Cortex-M4[m4] */
    IOMUXC_SetPinMux(IOMUXC_UART4_RXD_UART4_RX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART4_RXD_UART4_RX,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(6U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U));
    IOMUXC_SetPinMux(IOMUXC_UART4_TXD_UART4_TX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART4_TXD_UART4_TX,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(6U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U));
    pin_mux_enet();
}

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
