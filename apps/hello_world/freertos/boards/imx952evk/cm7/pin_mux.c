/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"

static void BOARD_InitDEBUG_UARTPins(void)
{
    IOMUXC_SetPinMux(IOMUXC_PAD_GPIO_IO14__LPUART3_TX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_PAD_GPIO_IO14__LPUART3_TX,
                        IOMUXC_PAD_DSE(15U) |
                        IOMUXC_PAD_FSEL1(2U) |
                        IOMUXC_PAD_PD_MASK);
    IOMUXC_SetPinMux(IOMUXC_PAD_GPIO_IO15__LPUART3_RX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_PAD_GPIO_IO15__LPUART3_RX,
                        IOMUXC_PAD_DSE(15U) |
                        IOMUXC_PAD_FSEL1(2U) |
                        IOMUXC_PAD_PD_MASK);
}
void BOARD_InitBootPins(void)
{
    BOARD_InitDEBUG_UARTPins();
}
