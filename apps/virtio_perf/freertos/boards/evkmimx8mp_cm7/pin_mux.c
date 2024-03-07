/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
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
}
