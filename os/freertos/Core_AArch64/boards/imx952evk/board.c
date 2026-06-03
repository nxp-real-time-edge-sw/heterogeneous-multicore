/*
 * Copyright 2026 NXP
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
#include "fsl_cache.h"
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
#include "fsl_lpi2c.h"
#endif
#if defined(BOARD_USE_PCA6416A) && BOARD_USE_PCA6416A
#include "fsl_pca6416a.h"
#endif /* BOARD_USE_PCA6416A */
#if BOARD_ENABLE_ELE
#include "fsl_ele_base_api.h"
#endif
#include "fsl_rgpio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* For i2c release bus */
typedef struct _i2c_rls_info
{
   clock_ip_name_t clk_id;
   RGPIO_Type * scl_gpio_base;
   RGPIO_Type * sda_gpio_base;
   uint32_t scl_pin_idx;
   uint32_t sda_pin_idx;
} i2c_rls_info_t;
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void BOARD_InitClock(void)
{
    clock_ip_name_t src_idx = kCLOCK_ext;
    for (; src_idx < CLOCK_NUM_SRC; src_idx++)
    {
        CLOCK_GetRate(src_idx);
    }
}

void pin_mux_lpuart1(void)
{
	IOMUXC_SetPinMux(IOMUXC_PAD_UART1_RXD__LPUART1_RX, 0U);
	IOMUXC_SetPinMux(IOMUXC_PAD_UART1_TXD__LPUART1_TX, 0U);

	IOMUXC_SetPinConfig(IOMUXC_PAD_UART1_RXD__LPUART1_RX,
			IOMUXC_PAD_DSE(15U) |
			IOMUXC_PAD_FSEL1(2U) |
			IOMUXC_PAD_PD_MASK);
	IOMUXC_SetPinConfig(IOMUXC_PAD_UART1_TXD__LPUART1_TX,
			IOMUXC_PAD_DSE(15U) |
			IOMUXC_PAD_FSEL1(2U) |
			IOMUXC_PAD_PD_MASK);
}

void pin_mux_lpuart3(void)
{
	IOMUXC_SetPinMux(IOMUXC_PAD_GPIO_IO15__LPUART3_RX, 0U);
	IOMUXC_SetPinMux(IOMUXC_PAD_GPIO_IO14__LPUART3_TX, 0U);

	IOMUXC_SetPinConfig(IOMUXC_PAD_GPIO_IO15__LPUART3_RX,
			IOMUXC_PAD_DSE(15U) |
			IOMUXC_PAD_FSEL1(2U) |
			IOMUXC_PAD_PD_MASK);
	IOMUXC_SetPinConfig(IOMUXC_PAD_GPIO_IO14__LPUART3_TX,
			IOMUXC_PAD_DSE(15U) |
			IOMUXC_PAD_FSEL1(2U) |
			IOMUXC_PAD_PD_MASK);
}

void clock_config_lpuart1(void)
{
    /* clang-format off */
    clk_t clk = {
        .clkId = kCLOCK_lpuart1,
		.pclkId = kCLOCK_osc24m,
		.rate = 24000000UL,
	.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
    };
    /* clang-format on */

    CLOCK_SetRate(&clk);
    CLOCK_EnableClock(clk.clkId);
}

void clock_config_lpuart3(void)
{
    /* clang-format off */
    clk_t clk = {
        .clkId = kCLOCK_lpuart3,
		.pclkId = kCLOCK_osc24m,
		.rate = 24000000UL,
	.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
    };
    /* clang-format on */

    CLOCK_SetRate(&clk);
    CLOCK_EnableClock(clk.clkId);
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

    /*
     * lpi2cConfig.debugEnable = false;
     * lpi2cConfig.ignoreAck = false;
     * lpi2cConfig.pinConfig = kLPI2C_2PinOpenDrain;
     * lpi2cConfig.baudRate_Hz = 100000U;
     * lpi2cConfig.busIdleTimeout_ns = 0;
     * lpi2cConfig.pinLowTimeout_ns = 0;
     * lpi2cConfig.sdaGlitchFilterWidth_ns = 0;
     * lpi2cConfig.sclGlitchFilterWidth_ns = 0;
     */
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

status_t BOARD_I2C_DeviceSend( void *base,
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

status_t BOARD_I2C_DeviceReceive( void *base,
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
        .i2cBase         = BOARD_PCAL6524_I2C,
        .i2cAddr         = BOARD_PCAL6524_I2C_ADDR,
        .I2C_SendFunc    = BOARD_I2C_DeviceSend,
        .I2C_ReceiveFunc = BOARD_I2C_DeviceReceive,
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

#if defined(BOARD_USE_PI4IO6408) && BOARD_USE_PI4IO6408
void BOARD_PI4IO6408_I2C_Init(void *base, uint32_t clkSrc_Hz)
{
    BOARD_LPI2C_Init(base, clkSrc_Hz);
}

status_t BOARD_PI4IO6408_I2C_Send(void *base,
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

status_t BOARD_PI4IO6408_I2C_Receive(void *base,
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

void BOARD_InitPI4IO6408_I2C3(pi4io6408_handle_t *handle)
{
    BOARD_PI4IO6408_I2C_Init(BOARD_PI4IO6408_I2C3, BOARD_PI4IO6408_I2C3_CLOCK_FREQ);

    static const pi4io6408_config_t config = {
        .i2cBase         = BOARD_PI4IO6408_I2C3,
        .i2cAddr         = BOARD_PI4IO6408_I2C3_ADDR,
        .I2C_SendFunc    = BOARD_PI4IO6408_I2C_Send,
        .I2C_ReceiveFunc = BOARD_PI4IO6408_I2C_Receive,
    };

    PI4IO6408_Init(handle, &config);
}
#endif

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
static void i2c_release_bus_delay(void)
{
    uint32_t i = 0;
    for (i = 0; i < I2C_RELEASE_BUS_COUNT; i++)
    {
        __NOP();
    }
}

/*
 *@use_as_gpio: true - use pads as gpio function, false - use pads as i2c function
 */
status_t BOARD_SelectI2cPadsFunc(int32_t i2cInstIdx, bool use_as_gpio)
{
    status_t result = kStatus_Success;
    assert(i2cInstIdx < I2C_MAX_INSTANCE_NUM);

    switch (i2cInstIdx)
    {
        case 1:
            if (use_as_gpio)
            {
                IOMUXC_SetPinMux(I2C1_RELEASE_SCL_PAD_USE_AS_GPIO_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C1_RELEASE_SCL_PAD_USE_AS_GPIO_ARGS,
                                    IOMUXC_PAD_DSE(7U) |
                                    IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_PD_MASK);
                IOMUXC_SetPinMux(I2C1_RELEASE_SDA_PAD_USE_AS_GPIO_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C1_RELEASE_SDA_PAD_USE_AS_GPIO_ARGS,
                                    IOMUXC_PAD_DSE(7U) |
                                    IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_PD_MASK);
            }
            else
            {
                IOMUXC_SetPinMux(I2C1_RELEASE_SCL_PAD_USE_AS_SCL_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C1_RELEASE_SCL_PAD_USE_AS_SCL_ARGS,
                                    IOMUXC_PAD_DSE(15U) | IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_OD_MASK);
                IOMUXC_SetPinMux(I2C1_RELEASE_SDA_PAD_USE_AS_SDA_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C1_RELEASE_SDA_PAD_USE_AS_SDA_ARGS,
                                    IOMUXC_PAD_DSE(15U) | IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_OD_MASK);
            }
            break;
        case 2:
            if (use_as_gpio)
            {
                IOMUXC_SetPinMux(I2C2_RELEASE_SCL_PAD_USE_AS_GPIO_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C2_RELEASE_SCL_PAD_USE_AS_GPIO_ARGS,
                                    IOMUXC_PAD_DSE(7U) |
                                    IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_PD_MASK);
                IOMUXC_SetPinMux(I2C2_RELEASE_SDA_PAD_USE_AS_GPIO_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C2_RELEASE_SDA_PAD_USE_AS_GPIO_ARGS,
                                    IOMUXC_PAD_DSE(7U) |
                                    IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_PD_MASK);
            }
            else
            {
                IOMUXC_SetPinMux(I2C2_RELEASE_SCL_PAD_USE_AS_SCL_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C2_RELEASE_SCL_PAD_USE_AS_SCL_ARGS,
                                    IOMUXC_PAD_DSE(15U) | IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_OD_MASK);
                IOMUXC_SetPinMux(I2C2_RELEASE_SDA_PAD_USE_AS_SDA_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C2_RELEASE_SDA_PAD_USE_AS_SDA_ARGS,
                                    IOMUXC_PAD_DSE(15U) | IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_OD_MASK);
            }
            break;
       case 3:
            if (use_as_gpio)
            {
                IOMUXC_SetPinMux(I2C3_RELEASE_SCL_PAD_USE_AS_GPIO_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C3_RELEASE_SCL_PAD_USE_AS_GPIO_ARGS,
                                    IOMUXC_PAD_DSE(7U) |
                                    IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_PD_MASK);
                IOMUXC_SetPinMux(I2C3_RELEASE_SDA_PAD_USE_AS_GPIO_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C3_RELEASE_SDA_PAD_USE_AS_GPIO_ARGS,
                                    IOMUXC_PAD_DSE(7U) |
                                    IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_PD_MASK);
            }
            else
            {
                IOMUXC_SetPinMux(I2C3_RELEASE_SCL_PAD_USE_AS_SCL_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C3_RELEASE_SCL_PAD_USE_AS_SCL_ARGS,
                                    IOMUXC_PAD_DSE(15U) | IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_OD_MASK);
                IOMUXC_SetPinMux(I2C3_RELEASE_SDA_PAD_USE_AS_SDA_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C3_RELEASE_SDA_PAD_USE_AS_SDA_ARGS,
                                    IOMUXC_PAD_DSE(15U) | IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_OD_MASK);
            }
            break;
        case 4:
            if (use_as_gpio)
            {
                IOMUXC_SetPinMux(I2C4_RELEASE_SCL_PAD_USE_AS_GPIO_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C4_RELEASE_SCL_PAD_USE_AS_GPIO_ARGS,
                                    IOMUXC_PAD_DSE(7U) |
                                    IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_PD_MASK);
                IOMUXC_SetPinMux(I2C4_RELEASE_SDA_PAD_USE_AS_GPIO_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C4_RELEASE_SDA_PAD_USE_AS_GPIO_ARGS,
                                    IOMUXC_PAD_DSE(7U) |
                                    IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_PD_MASK);
            }
            else
            {
                IOMUXC_SetPinMux(I2C4_RELEASE_SCL_PAD_USE_AS_SCL_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C4_RELEASE_SCL_PAD_USE_AS_SCL_ARGS,
                                    IOMUXC_PAD_DSE(15U) | IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_OD_MASK);
                IOMUXC_SetPinMux(I2C4_RELEASE_SDA_PAD_USE_AS_SDA_ARGS, 0U);
                IOMUXC_SetPinConfig(I2C4_RELEASE_SDA_PAD_USE_AS_SDA_ARGS,
                                    IOMUXC_PAD_DSE(15U) | IOMUXC_PAD_FSEL1(2U) |
                                    IOMUXC_PAD_OD_MASK);
            }
            break;
        default:
            PRINTF("Please support this new instance %d in board.c\r\n", i2cInstIdx);
            result = kStatus_InvalidArgument;
            break;
    }
    return result;
}

/*
 * i2cInstIdx: i2c controler instance index
 */
status_t BOARD_I2C_ReleaseBus(int32_t i2cInstIdx)
{
    uint8_t i = 0;
    status_t result = kStatus_Success;
    rgpio_pin_config_t pin_config;
    i2c_rls_info_t i2c_infos[I2C_MAX_INSTANCE_NUM] = {
        { kCLOCK_invalid, 0, 0, 0, 0},
        { kCLOCK_invalid, I2C1_RELEASE_SCL_GPIO, I2C1_RELEASE_SDA_GPIO, I2C1_RELEASE_SCL_PIN, I2C1_RELEASE_SDA_PIN},
        { kCLOCK_invalid, I2C2_RELEASE_SCL_GPIO, I2C2_RELEASE_SDA_GPIO, I2C2_RELEASE_SCL_PIN, I2C2_RELEASE_SDA_PIN},
        { kCLOCK_invalid, I2C3_RELEASE_SCL_GPIO, I2C3_RELEASE_SDA_GPIO, I2C3_RELEASE_SCL_PIN, I2C3_RELEASE_SDA_PIN},
        { kCLOCK_invalid, I2C4_RELEASE_SCL_GPIO, I2C4_RELEASE_SDA_GPIO, I2C4_RELEASE_SCL_PIN, I2C4_RELEASE_SDA_PIN},
    };
    /* coverity[cert_int31_c_violation] */
    assert(i2cInstIdx < ARRAY_SIZE(i2c_infos));
    i2c_rls_info_t *i2c_info = &i2c_infos[i2cInstIdx];

    pin_config.pinDirection = kRGPIO_DigitalOutput;
    pin_config.outputLogic  = 1U;

    /* Select gpio function for pads that used for i2c */
    result = BOARD_SelectI2cPadsFunc(i2cInstIdx, true);
    if (result != kStatus_Success)
    {
      PRINTF("Failed to change pad function to gpio\r\n");
      return result;
    }
    CLOCK_EnableClock(i2c_info->clk_id);

    RGPIO_PinInit(i2c_info->scl_gpio_base, i2c_info->scl_pin_idx, &pin_config);
    RGPIO_PinInit(i2c_info->sda_gpio_base, i2c_info->sda_pin_idx, &pin_config);

    /* Drive SDA low first to simulate a start */
    RGPIO_PinWrite(i2c_info->sda_gpio_base, i2c_info->sda_pin_idx, 0U);
    i2c_release_bus_delay();

    /* Send 9 pulses on SCL and keep SDA high */
    for (i = 0; i < 9; i++)
    {
        RGPIO_PinWrite(i2c_info->scl_gpio_base, i2c_info->scl_pin_idx, 0U);
        i2c_release_bus_delay();

        RGPIO_PinWrite(i2c_info->sda_gpio_base, i2c_info->sda_pin_idx, 1U);
        i2c_release_bus_delay();

        RGPIO_PinWrite(i2c_info->scl_gpio_base, i2c_info->scl_pin_idx, 1U);
        i2c_release_bus_delay();
        i2c_release_bus_delay();
    }

    /* Send stop */
    RGPIO_PinWrite(i2c_info->scl_gpio_base, i2c_info->scl_pin_idx, 0U);
    i2c_release_bus_delay();

    RGPIO_PinWrite(i2c_info->sda_gpio_base, i2c_info->sda_pin_idx, 0U);
    i2c_release_bus_delay();

    RGPIO_PinWrite(i2c_info->scl_gpio_base, i2c_info->scl_pin_idx, 1U);
    i2c_release_bus_delay();

    RGPIO_PinWrite(i2c_info->sda_gpio_base, i2c_info->sda_pin_idx, 1U);
    i2c_release_bus_delay();
    /* On 8ulp i2c affect fusion_init, so add more delay */
    i2c_release_bus_delay();

    /* Select i2c function for pads that used for i2c */
    BOARD_SelectI2cPadsFunc(i2cInstIdx, false);

    return result;
}
