/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_clock.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "pin_mux.h"
#include "board.h"
#include "mmu.h"
#include "uart.h"
#include "hal_pinctrl.h"
#include "fsl_cache.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void BOARD_InitClock(void)
{
	hal_clk_id_e src_idx = hal_clock_ext;

	for (; src_idx < HAL_CLOCK_PLATFORM_SOURCE_NUM; src_idx++) {
		HAL_ClockGetRate(src_idx);
	}
}

void pin_mux_lpuart1(void)
{
	HAL_PinctrlSetPinMux(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_UART1_RXD__LPUART1_RX, 0U);
	HAL_PinctrlSetPinMux(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_UART1_TXD__LPUART1_TX, 0U);

	HAL_PinctrlSetPinCfg(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_UART1_RXD__LPUART1_RX,
			HAL_PINCTRL_PLATFORM_IOMUXC_PAD_PD_MASK);
	HAL_PinctrlSetPinCfg(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_UART1_TXD__LPUART1_TX,
			HAL_PINCTRL_PLATFORM_IOMUXC_PAD_DSE(15U));
}

void pin_mux_lpuart3(void)
{
	HAL_PinctrlSetPinMux(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_GPIO_IO15__LPUART3_RX, 0U);
	HAL_PinctrlSetPinMux(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_GPIO_IO14__LPUART3_TX, 0U);

	HAL_PinctrlSetPinCfg(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_GPIO_IO15__LPUART3_RX,
			HAL_PINCTRL_PLATFORM_IOMUXC_PAD_PD_MASK);
	HAL_PinctrlSetPinCfg(HAL_PINCTRL_PLATFORM_IOMUXC_PAD_GPIO_IO14__LPUART3_TX,
			HAL_PINCTRL_PLATFORM_IOMUXC_PAD_DSE(15U));
}

void clock_config_lpuart1(void)
{
	/* clang-format off */
	hal_clk_t hal_clk = {
		.clk_id = hal_clock_lpuart1,
		.clk_round_opt = hal_clk_round_auto,
		.rate = 24000000UL,
	};
	/* clang-format on */

	HAL_ClockSetRate(&hal_clk);
	HAL_ClockEnable(&hal_clk);
}

void clock_config_lpuart3(void)
{
	/* clang-format off */
	hal_clk_t hal_clk = {
		.clk_id = hal_clock_lpuart3,
		.clk_round_opt = hal_clk_round_auto,
		.rate = 24000000UL,
	};
	/* clang-format on */

	HAL_ClockSetRate(&hal_clk);
	HAL_ClockEnable(&hal_clk);
}

/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
#if (BOARD_DEBUG_UART_INSTANCE == 1)
	pin_mux_lpuart1();
	clock_config_lpuart1();
#elif (BOARD_DEBUG_UART_INSTANCE == 3)
	pin_mux_lpuart3();
	clock_config_lpuart3();
#endif
	uart_init();
}

/* Initialize MMU, configure memory attributes for each region */
void BOARD_InitMemory(void)
{
	MMU_init();
}

void BOARD_RdcInit(void)
{
}

#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
void BOARD_LPI2C_Init(LPI2C_Type *base, uint32_t clkSrc_Hz)
{
    lpi2c_master_config_t lpi2cConfig = {0};

    LPI2C_MasterGetDefaultConfig(&lpi2cConfig);
    LPI2C_MasterInit(base, &lpi2cConfig, clkSrc_Hz);
}

status_t BOARD_LPI2C_Send(LPI2C_Type *base,
                          uint8_t deviceAddress,
                          uint32_t subAddress,
                          uint8_t subAddressSize,
                          uint8_t *txBuff,
                          uint8_t txBuffSize,
                          uint32_t flags)
{
    lpi2c_master_transfer_t xfer;

    xfer.flags          = flags;
    xfer.slaveAddress   = deviceAddress;
    xfer.direction      = kLPI2C_Write;
    xfer.subaddress     = subAddress;
    xfer.subaddressSize = subAddressSize;
    xfer.data           = txBuff;
    xfer.dataSize       = txBuffSize;

    return LPI2C_MasterTransferBlocking(base, &xfer);
}

status_t BOARD_LPI2C_Receive(LPI2C_Type *base,
                             uint8_t deviceAddress,
                             uint32_t subAddress,
                             uint8_t subAddressSize,
                             uint8_t *rxBuff,
                             uint8_t rxBuffSize,
                             uint32_t flags)
{
    lpi2c_master_transfer_t xfer;

    xfer.flags          = flags;
    xfer.slaveAddress   = deviceAddress;
    xfer.direction      = kLPI2C_Read;
    xfer.subaddress     = subAddress;
    xfer.subaddressSize = subAddressSize;
    xfer.data           = rxBuff;
    xfer.dataSize       = rxBuffSize;

    return LPI2C_MasterTransferBlocking(base, &xfer);
}

status_t BOARD_Display_I2C_Send(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize)
{
    return BOARD_LPI2C_Send(LPI2C2, deviceAddress, subAddress, subAddressSize, (uint8_t *)txBuff,
                            txBuffSize, 0);
}

status_t BOARD_Display_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    return BOARD_LPI2C_Receive(LPI2C2, deviceAddress, subAddress, subAddressSize, rxBuff,
                               rxBuffSize, 0);
}

#if defined(BOARD_USE_ADP5585) && BOARD_USE_ADP5585
void BOARD_ADP5585_I2C_Init(void)
{
    BOARD_LPI2C_Init(BOARD_ADP5585_I2C, BOARD_ADP5585_I2C_CLOCK_FREQ);
}

status_t BOARD_ADP5585_I2C_Send(uint8_t deviceAddress,
                                uint32_t subAddress,
                                uint8_t subAddressSize,
                                const uint8_t *txBuff,
                                uint8_t txBuffSize,
                                uint32_t flags)
{
    return BOARD_LPI2C_Send(BOARD_ADP5585_I2C, deviceAddress, subAddress, subAddressSize, (uint8_t *)txBuff, txBuffSize,
                            flags);
}

status_t BOARD_ADP5585_I2C_Receive(uint8_t deviceAddress,
                                   uint32_t subAddress,
                                   uint8_t subAddressSize,
                                   uint8_t *rxBuff,
                                   uint8_t rxBuffSize,
                                   uint32_t flags)
{
    return BOARD_LPI2C_Receive(BOARD_ADP5585_I2C, deviceAddress, subAddress, subAddressSize, rxBuff, rxBuffSize, flags);
}

void BOARD_InitADP5585(adp5585_handle_t *handle)
{
    BOARD_ADP5585_I2C_Init();

    static const adp5585_config_t config = {
        .i2cAddr         = BOARD_ADP5585_I2C_ADDR,
        .I2C_SendFunc    = BOARD_ADP5585_I2C_Send,
        .I2C_ReceiveFunc = BOARD_ADP5585_I2C_Receive,
    };

    ADP5585_Init(handle, &config);
}

#endif /* BOARD_USE_ADP5585. */

#if defined(BOARD_USE_PCAL6524) && BOARD_USE_PCAL6524
void BOARD_PCAL6524_I2C_Init(void)
{
    BOARD_LPI2C_Init(BOARD_PCAL6524_I2C, BOARD_PCAL6524_I2C_CLOCK_FREQ);
}

status_t BOARD_PCAL6524_I2C_Send(uint8_t deviceAddress,
                                 uint32_t subAddress,
                                 uint8_t subAddressSize,
                                 const uint8_t *txBuff,
                                 uint8_t txBuffSize,
                                 uint32_t flags)
{
    return BOARD_LPI2C_Send(BOARD_PCAL6524_I2C, deviceAddress, subAddress, subAddressSize, (uint8_t *)txBuff,
                            txBuffSize, flags);
}

status_t BOARD_PCAL6524_I2C_Receive(uint8_t deviceAddress,
                                    uint32_t subAddress,
                                    uint8_t subAddressSize,
                                    uint8_t *rxBuff,
                                    uint8_t rxBuffSize,
                                    uint32_t flags)
{
    return BOARD_LPI2C_Receive(BOARD_PCAL6524_I2C, deviceAddress, subAddress, subAddressSize, rxBuff, rxBuffSize,
                               flags);
}

void BOARD_InitPCAL6524(pcal6524_handle_t *handle)
{
    BOARD_PCAL6524_I2C_Init();

    static const pcal6524_config_t config = {
        .i2cAddr         = BOARD_PCAL6524_I2C_ADDR,
        .I2C_SendFunc    = BOARD_PCAL6524_I2C_Send,
        .I2C_ReceiveFunc = BOARD_PCAL6524_I2C_Receive,
    };

    PCAL6524_Init(handle, &config);
}

#endif /* BOARD_USE_PCAL6524. */

#if defined(BOARD_USE_PCAL6408) && BOARD_USE_PCAL6408
void BOARD_PCAL6408_I2C_Init(void *base, uint32_t clkSrc_Hz)
{
    BOARD_LPI2C_Init(base, clkSrc_Hz);
}

status_t BOARD_PCAL6408_I2C_Send(void *base,
                                 uint8_t deviceAddress,
                                 uint32_t subAddress,
                                 uint8_t subAddressSize,
                                 const uint8_t *txBuff,
                                 uint8_t txBuffSize,
                                 uint32_t flags)
{
    return BOARD_LPI2C_Send(base, deviceAddress, subAddress, subAddressSize, (uint8_t *)txBuff,
                            txBuffSize, flags);
}

status_t BOARD_PCAL6408_I2C_Receive(void *base,
                                    uint8_t deviceAddress,
                                    uint32_t subAddress,
                                    uint8_t subAddressSize,
                                    uint8_t *rxBuff,
                                    uint8_t rxBuffSize,
                                    uint32_t flags)
{
    return BOARD_LPI2C_Receive(base, deviceAddress, subAddress, subAddressSize, rxBuff, rxBuffSize,
                               flags);
}

void BOARD_InitPCAL6408_I2C1(pcal6408_handle_t *handle)
{
    BOARD_PCAL6408_I2C_Init(BOARD_PCAL6408_I2C1, BOARD_PCAL6408_I2C1_CLOCK_FREQ);

    static const pcal6408_config_t config = {
        .i2cBase         = BOARD_PCAL6408_I2C1,
        .i2cAddr         = BOARD_PCAL6408_I2C1_ADDR,
        .I2C_SendFunc    = BOARD_PCAL6408_I2C_Send,
        .I2C_ReceiveFunc = BOARD_PCAL6408_I2C_Receive,
    };

    PCAL6408_Init(handle, &config);
}

void BOARD_InitPCAL6408_I2C3(pcal6408_handle_t *handle)
{
    BOARD_PCAL6408_I2C_Init(BOARD_PCAL6408_I2C3, BOARD_PCAL6408_I2C3_CLOCK_FREQ);

    static const pcal6408_config_t config = {
        .i2cBase         = BOARD_PCAL6408_I2C3,
        .i2cAddr         = BOARD_PCAL6408_I2C3_ADDR,
        .I2C_SendFunc    = BOARD_PCAL6408_I2C_Send,
        .I2C_ReceiveFunc = BOARD_PCAL6408_I2C_Receive,
    };

    PCAL6408_Init(handle, &config);
}

void BOARD_InitPCAL6408_I2C4(pcal6408_handle_t *handle)
{
    BOARD_PCAL6408_I2C_Init(BOARD_PCAL6408_I2C4, BOARD_PCAL6408_I2C4_CLOCK_FREQ);

    static const pcal6408_config_t config = {
        .i2cBase         = BOARD_PCAL6408_I2C4,
        .i2cAddr         = BOARD_PCAL6408_I2C4_ADDR,
        .I2C_SendFunc    = BOARD_PCAL6408_I2C_Send,
        .I2C_ReceiveFunc = BOARD_PCAL6408_I2C_Receive,
    };

    PCAL6408_Init(handle, &config);
}

void BOARD_InitPCAL6408_I2C5(pcal6408_handle_t *handle)
{
    BOARD_PCAL6408_I2C_Init(BOARD_PCAL6408_I2C5, BOARD_PCAL6408_I2C5_CLOCK_FREQ);

    static const pcal6408_config_t config = {
        .i2cBase         = BOARD_PCAL6408_I2C5,
        .i2cAddr         = BOARD_PCAL6408_I2C5_ADDR,
        .I2C_SendFunc    = BOARD_PCAL6408_I2C_Send,
        .I2C_ReceiveFunc = BOARD_PCAL6408_I2C_Receive,
    };

    PCAL6408_Init(handle, &config);
}
#endif /* BOARD_USE_PCAL6408. */

#if defined(BOARD_USE_PCA6416A) && BOARD_USE_PCA6416A
void BOARD_PCA6416A_I2C_Init(void)
{
    BOARD_LPI2C_Init(BOARD_PCA6416A_I2C, BOARD_PCA6416A_I2C_CLOCK_FREQ);
}

status_t BOARD_PCA6416A_I2C_Send(void *base,
                                 uint8_t deviceAddress,
                                 uint32_t subAddress,
                                 uint8_t subAddressSize,
                                 const uint8_t *txBuff,
                                 uint8_t txBuffSize,
                                 uint32_t flags)
{
    return BOARD_LPI2C_Send(base, deviceAddress, subAddress, subAddressSize, (uint8_t *)txBuff,
                            txBuffSize, flags);
}

status_t BOARD_PCA6416A_I2C_Receive(void *base,
                                    uint8_t deviceAddress,
                                    uint32_t subAddress,
                                    uint8_t subAddressSize,
                                    uint8_t *rxBuff,
                                    uint8_t rxBuffSize,
                                    uint32_t flags)
{
    return BOARD_LPI2C_Receive(base, deviceAddress, subAddress, subAddressSize, rxBuff, rxBuffSize,
                               flags);
}

void BOARD_InitPCA6416A(pca6416a_handle_t *handle)
{
    BOARD_PCA6416A_I2C_Init();

    static const pca6416a_config_t config = {
        .i2cBase         = BOARD_PCA6416A_I2C,
        .i2cAddr         = BOARD_PCA6416A_I2C_ADDR,
        .I2C_SendFunc    = BOARD_PCA6416A_I2C_Send,
        .I2C_ReceiveFunc = BOARD_PCA6416A_I2C_Receive,
    };

    PCA6416A_Init(handle, &config);
}
#endif /* BOARD_USE_PCA6416A. */
#endif /* SDK_I2C_BASED_COMPONENT_USED */
