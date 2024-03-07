/*
 * Copyright 2023-2024 NXP
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
product: Pins v12.0
processor: MIMX9352xxxxM
package_id: MIMX9352DVUXM
mcu_data: ksdk2_0
processor_version: 0.12.3
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

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
- options: {callFromInitBoot: 'true', coreID: cm33}
- pin_list:
  - {pin_num: F20, peripheral: LPUART2, signal: lpuart_rx, pin_signal: UART2_RXD, FSEL1: SlOW_SLEW_RATE, DSE: NO_DRIVE}
  - {pin_num: F21, peripheral: LPUART2, signal: lpuart_tx, pin_signal: UART2_TXD, PD: DISABLED, FSEL1: SlOW_SLEW_RATE}
  - {pin_num: J20, peripheral: LPUART5, signal: lpuart_rx, pin_signal: GPIO_IO01, FSEL1: SlOW_SLEW_RATE, DSE: NO_DRIVE}
  - {pin_num: J21, peripheral: LPUART5, signal: lpuart_tx, pin_signal: GPIO_IO00, PD: DISABLED, FSEL1: SlOW_SLEW_RATE}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPins(void) {                                /*!< Function assigned for the core: undefined[cm33] */
    IOMUXC_SetPinMux(IOMUXC_PAD_GPIO_IO01__LPUART5_RX, 0U);
    IOMUXC_SetPinMux(IOMUXC_PAD_GPIO_IO00__LPUART5_TX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_PAD_GPIO_IO01__LPUART5_RX,
                        IOMUXC_PAD_PD_MASK);
    IOMUXC_SetPinConfig(IOMUXC_PAD_GPIO_IO00__LPUART5_TX,
                        IOMUXC_PAD_DSE(15U));

    IOMUXC_SetPinMux(IOMUXC_PAD_UART2_RXD__LPUART2_RX, 0U);
    IOMUXC_SetPinMux(IOMUXC_PAD_UART2_TXD__LPUART2_TX, 0U);

    IOMUXC_SetPinConfig(IOMUXC_PAD_UART2_RXD__LPUART2_RX, 
                        IOMUXC_PAD_PD_MASK);
    IOMUXC_SetPinConfig(IOMUXC_PAD_UART2_TXD__LPUART2_TX, 
                        IOMUXC_PAD_DSE(15U));
}

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
