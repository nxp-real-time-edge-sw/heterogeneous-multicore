/*
 * Copyright 2022-2023 NXP
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
product: Pins v10.0
processor: MIMX8MM6xxxLZ
package_id: MIMX8MM6DVTLZ
mcu_data: ksdk2_0
processor_version: 0.10.12
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "pin_mux.h"

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
  - {pin_num: F19, peripheral: UART4, signal: uart_rx, pin_signal: UART4_RXD, PE: Disabled, PUE: Enabled, DSE: X6_0}
  - {pin_num: F18, peripheral: UART4, signal: uart_tx, pin_signal: UART4_TXD, PE: Disabled, PUE: Enabled, DSE: X6_0}
  - {pin_num: D6, peripheral: UART3, signal: uart_rx, pin_signal: ECSPI1_SCLK, PUE: Enabled}
  - {pin_num: B7, peripheral: UART3, signal: uart_tx, pin_signal: ECSPI1_MOSI, PUE: Enabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPins(void) {                                /*!< Function assigned for the core: Cortex-M4[cm4] */
    IOMUXC_SetPinMux(IOMUXC_UART4_RXD_UART4_RX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART4_RXD_UART4_RX,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(6U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK);
    IOMUXC_SetPinMux(IOMUXC_UART4_TXD_UART4_TX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART4_TXD_UART4_TX,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(6U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK);

    IOMUXC_SetPinMux(IOMUXC_ECSPI1_MOSI_UART3_TX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ECSPI1_MOSI_UART3_TX,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(6U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PE_MASK);
    IOMUXC_SetPinMux(IOMUXC_ECSPI1_SCLK_UART3_RX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_ECSPI1_SCLK_UART3_RX,
                        IOMUXC_SW_PAD_CTL_PAD_DSE(6U) |
                        IOMUXC_SW_PAD_CTL_PAD_FSEL(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PE_MASK);
}

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/