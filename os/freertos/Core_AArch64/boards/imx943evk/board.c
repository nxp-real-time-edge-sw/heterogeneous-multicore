/*
 * Copyright 2025 NXP
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
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
#include "fsl_lpi2c.h"
#endif
#if defined(BOARD_USE_PCA6416) && BOARD_USE_PCA6416
#include "fsl_pca6416a.h"
#endif
#if defined(BOARD_USE_PCA6408) && BOARD_USE_PCA6408
#include "fsl_pcal6408.h"
#endif
#if defined(BOARD_USE_PCA954X) && BOARD_USE_PCA954X
#include "fsl_pca954x.h"
#endif
#include "fsl_cache.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
static void BOARD_InitPCA954X(uint32_t i2cDevId, void *base, uint32_t clkFreq, uint8_t i2cAddr);
static status_t BOARD_PCA954X_SelectChan(uint32_t i2cDevId, uint32_t chan);
static status_t BOARD_PCA954X_DeselectMux(uint32_t i2cDevId, uint32_t chan);

static void BOARD_InitPCA6408(uint32_t i2cDevId, void *base, uint32_t clkFreq, uint8_t i2cAddr);
static status_t BOARD_PCA6408_SetPinToHigh(uint32_t i2cDevId, uint8_t pinIdx);
static status_t BOARD_PCA6408_SetPinToLow(uint32_t i2cDevId, uint8_t pinIdx);
static status_t BOARD_PCA6408_SetPinAsInput(uint32_t i2cDevId, uint8_t pinIdx);
static status_t BOARD_PCA6408_SetPinAsOutput(uint32_t i2cDevId, uint8_t pinIdx);

static void BOARD_InitPCA6416(uint32_t i2cDevId, void *base, uint32_t clkFreq, uint8_t i2cAddr);
static status_t BOARD_PCA6416_SetPinToHigh(uint32_t i2cDevId, uint8_t pinIdx);
static status_t BOARD_PCA6416_SetPinToLow(uint32_t i2cDevId, uint8_t pinIdx);
static status_t BOARD_PCA6416_SetPinAsInput(uint32_t i2cDevId, uint8_t pinIdx);
static status_t BOARD_PCA6416_SetPinAsOutput(uint32_t i2cDevId, uint8_t pinIdx);
#endif
/*******************************************************************************
 * Variables
 ******************************************************************************/
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
static LPI2C_Type * lpi2cBases[] = LPI2C_BASE_PTRS;
static clock_ip_name_t lpi2cClkId[] = LPI2C_CLOCKS;

static pca954x_handle_t g_pca954xHandle[BOARD_PCA9544_NUM + BOARD_PCA9548_NUM];
static pcal6408_handle_t g_pca6408Handle[BOARD_PCA6408_NUM];
static pca6416a_handle_t g_pca6416Handle[BOARD_PCA6416_NUM];

i2c_device_table_t g_i2cDevTbl[] = {
    [BOARD_PCA9544_I2C6_ID & I2C_DEVICE_ARRAY_IDX_MASK] = {
        .handle = &g_pca954xHandle[BOARD_PCA9544_I2C6_HANDLE_IDX],
        .parent_id = BOARD_UNKNOWN_ID,
    },
   [BOARD_PCA9548_I2C3_ID & I2C_DEVICE_ARRAY_IDX_MASK] = {
        .handle = &g_pca954xHandle[BOARD_PCA9548_I2C3_HANDLE_IDX],
        .parent_id = BOARD_UNKNOWN_ID,
    },
   [BOARD_PCA6416_I2C6_S1_ID & I2C_DEVICE_ARRAY_IDX_MASK] = {
        .handle = &g_pca6416Handle[BOARD_PCA6416_I2C6_S1_HANDLE_IDX],
        .parent_id = BOARD_PCA9544_I2C6_ID,
    },
   [BOARD_PCA6408_I2C6_S2_ID & I2C_DEVICE_ARRAY_IDX_MASK] = {
        .handle = &g_pca6408Handle[BOARD_PCA6408_I2C6_S2_HANDLE_IDX],
        .parent_id = BOARD_PCA9544_I2C6_ID,
    },
   [BOARD_PCA6416_I2C6_S3_ID & I2C_DEVICE_ARRAY_IDX_MASK] = {
        .handle = &g_pca6416Handle[BOARD_PCA6416_I2C6_S3_HANDLE_IDX],
        .parent_id = BOARD_PCA9544_I2C6_ID,
    },
   [BOARD_PCA6416_I2C3_S5_20_ID & I2C_DEVICE_ARRAY_IDX_MASK] = {
        .handle = &g_pca6416Handle[BOARD_PCA6416_I2C3_S5_20_HANDLE_IDX],
        .parent_id = BOARD_PCA9548_I2C3_ID,
    },
   [BOARD_PCA6416_I2C3_S5_21_ID & I2C_DEVICE_ARRAY_IDX_MASK] = {
        .handle = &g_pca6416Handle[BOARD_PCA6416_I2C3_S5_21_HANDLE_IDX],
        .parent_id = BOARD_PCA9548_I2C3_ID,
    },
    [BOARD_PCA6416_I2C3_S6_ID & I2C_DEVICE_ARRAY_IDX_MASK] = {
	 .handle = &g_pca6416Handle[BOARD_PCA6416_I2C3_S6_HANDLE_IDX],
	 .parent_id = BOARD_PCA9548_I2C3_ID,
     },
    [BOARD_PCA6408_I2C3_S7_ID & I2C_DEVICE_ARRAY_IDX_MASK] = {
	 .handle = &g_pca6408Handle[BOARD_PCA6408_I2C3_S7_HANDLE_IDX],
	 .parent_id = BOARD_PCA9548_I2C3_ID,
     },

};

static pca954x_t g_pca954xData[] = {
    [PCA9544_TYPE] = {
        .id = PCA9544_ID,
        .init = BOARD_InitPCA954X,
	.select = BOARD_PCA954X_SelectChan,
	.deselect = BOARD_PCA954X_DeselectMux,
    },
    [PCA9548_TYPE] = {
        .id = PCA9548_ID,
        .init = BOARD_InitPCA954X,
	.select = BOARD_PCA954X_SelectChan,
	.deselect = BOARD_PCA954X_DeselectMux,
    },
};

static pca64xx_t g_pca64xxData[] = {
    [PCA6408_TYPE - PCA6408_TYPE] = {
        .init = BOARD_InitPCA6408,
	.set_pin_to_high = BOARD_PCA6408_SetPinToHigh,
	.set_pin_to_low = BOARD_PCA6408_SetPinToLow,
	.set_pin_as_input = BOARD_PCA6408_SetPinAsInput,
	.set_pin_as_output = BOARD_PCA6408_SetPinAsOutput,
	.select = BOARD_MUX_Select,
	.deselect = BOARD_MUX_Deselect,
    },
    [PCA6416_TYPE - PCA6408_TYPE] = {
        .init = BOARD_InitPCA6416,
	.select = BOARD_PCA954X_SelectChan,
	.deselect = BOARD_PCA954X_DeselectMux,
	.set_pin_to_high = BOARD_PCA6416_SetPinToHigh,
	.set_pin_to_low = BOARD_PCA6416_SetPinToLow,
	.set_pin_as_input = BOARD_PCA6416_SetPinAsInput,
	.set_pin_as_output = BOARD_PCA6416_SetPinAsOutput,
	.select = BOARD_MUX_Select,
	.deselect = BOARD_MUX_Deselect,
    },
};
#endif
/*******************************************************************************
 * Code
 ******************************************************************************/

void BOARD_InitClock(void)
{
	clock_ip_name_t src_idx = kCLOCK_Ext;

	for (; src_idx < CLOCK_NUM_SRC; src_idx++) {
		CLOCK_GetRate(src_idx);
	}
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

void BOARD_I2C_DeviceInit(LPI2C_Type *base, uint32_t clkFreq)
{
    BOARD_LPI2C_Init(base, clkFreq);
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
#endif

#if defined(BOARD_USE_PCA954X) && BOARD_USE_PCA954X
static void BOARD_InitPCA954X(uint32_t i2cDevId, void *base, uint32_t clkFreq, uint8_t i2cAddr)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    void *handle = g_i2cDevTbl[i2cDevArrayIdx].handle;
    pca954x_t *pca954xData = g_i2cDevTbl[i2cDevArrayIdx].data;
    uint8_t id = pca954xData->id;

    BOARD_I2C_DeviceInit(base, clkFreq);

    pca954x_config_t config = {
        .id              = (enum pca954x_id)id,
        .i2cBase         = base,
        .i2cAddr         = i2cAddr,
        .I2C_SendFunc    = BOARD_I2C_DeviceSend,
        .I2C_ReceiveFunc = BOARD_I2C_DeviceReceive,
    };

    PCA954X_Init(handle, &config);
}

static status_t BOARD_PCA954X_SelectChan(uint32_t i2cDevId, uint32_t chan)
{
    uint8_t i2cDevArrayIdx = i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK;
    void *handle = g_i2cDevTbl[i2cDevArrayIdx].handle;

    return PCA954X_SelectChan(handle, chan);
}

static status_t BOARD_PCA954X_DeselectMux(uint32_t i2cDevId, uint32_t chan)
{
    uint8_t i2cDevArrayIdx = i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK;
    void *handle = g_i2cDevTbl[i2cDevArrayIdx].handle;

    return PCA954X_DeselectMux(handle, chan);
}

#endif /* BOARD_USE_PCA954X. */


#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
void BOARD_InitI2cDevice(uint32_t i2cDevId)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    uint8_t i2cInstIdx = (i2cDevId & I2C_INSTANCE_IDX_MASK) >> I2C_INSTANCE_IDX_SHIFT;
    LPI2C_Type *base = lpi2cBases[i2cInstIdx];
    clock_ip_name_t clkId = lpi2cClkId[i2cInstIdx];
    uint32_t clkFreq = CLOCK_GetRate(clkId);
    uint8_t i2cAddr = (i2cDevId & I2C_DEVICE_ADDR_MASK) >> I2C_DEVICE_ADDR_SHIFT;
    uint8_t deviceType = (i2cDevId & I2C_DEVICE_TYPE_MASK) >> I2C_DEVICE_TYPE_SHIFT;

    if (deviceType < PCA954X_TYPE)
    {
        pca954x_t *pca954xData = NULL;

        g_i2cDevTbl[i2cDevArrayIdx].data = &g_pca954xData[deviceType];
	pca954xData = g_i2cDevTbl[i2cDevArrayIdx].data;
	pca954xData->init(i2cDevId, base, clkFreq, i2cAddr);
    }
    else if (deviceType < PCA64XX_TYPE)
    {
        pca64xx_t *pca64xxData = NULL;

        g_i2cDevTbl[i2cDevArrayIdx].data = &g_pca64xxData[deviceType - PCA6408_TYPE];
	pca64xxData = g_i2cDevTbl[i2cDevArrayIdx].data;
	pca64xxData->init(i2cDevId, base, clkFreq, i2cAddr);
    }
    else
    {
	assert(false);
    }
}

status_t BOARD_MUX_Select(uint32_t i2cDevId, uint32_t chan)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    uint8_t deviceType = (i2cDevId & I2C_DEVICE_TYPE_MASK) >> I2C_DEVICE_TYPE_SHIFT;
    pca954x_t *pca954xData = NULL;

    (void)deviceType;
    assert(deviceType < PCA954X_TYPE);

    if (g_i2cDevTbl[i2cDevArrayIdx].data == NULL)
    {
        BOARD_InitI2cDevice(i2cDevId);
    }

    pca954xData = g_i2cDevTbl[i2cDevArrayIdx].data;
    assert(pca954xData != NULL);

    return pca954xData->select(i2cDevId, chan);
}

status_t BOARD_MUX_Deselect(uint32_t i2cDevId, uint32_t chan)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    uint8_t deviceType = (i2cDevId & I2C_DEVICE_TYPE_MASK) >> I2C_DEVICE_TYPE_SHIFT;
    pca954x_t *pca954xData = NULL;

    (void)deviceType;
    assert(deviceType < PCA954X_TYPE);

    if (g_i2cDevTbl[i2cDevArrayIdx].data == NULL)
    {
        BOARD_InitI2cDevice(i2cDevId);
    }

    pca954xData = g_i2cDevTbl[i2cDevArrayIdx].data;
    assert(pca954xData != NULL);

    return pca954xData->deselect(i2cDevId, chan);
}

status_t BOARD_EXPANDER_SetPinToHigh(uint32_t i2cDevId, uint8_t pinIdx)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    uint32_t parentId = g_i2cDevTbl[i2cDevArrayIdx].parent_id;
    uint8_t deviceType = (i2cDevId & I2C_DEVICE_TYPE_MASK) >> I2C_DEVICE_TYPE_SHIFT;
    uint8_t chanIdx = (i2cDevId & I2C_CHAN_IDX_MASK) >> I2C_CHAN_IDX_SHIFT;
    pca64xx_t *pca64xxData = NULL;
    status_t status = kStatus_Fail;

    (void)deviceType;
    assert(deviceType >= PCA954X_TYPE);
    assert(deviceType < PCA64XX_TYPE);

    if (g_i2cDevTbl[i2cDevArrayIdx].data == NULL)
    {
        BOARD_InitI2cDevice(i2cDevId);
    }

    pca64xxData = g_i2cDevTbl[i2cDevArrayIdx].data;
    assert(pca64xxData != NULL);

    if (parentId != BOARD_UNKNOWN_ID && pca64xxData->select != NULL)
    {
        status = pca64xxData->select(parentId, chanIdx);
    }

    if (status == kStatus_Success)
    {
        status = pca64xxData->set_pin_as_output(i2cDevId, pinIdx);
        if (status == kStatus_Success)
        {
            status = pca64xxData->set_pin_to_high(i2cDevId, pinIdx);
        }
    }
    if (status == kStatus_Success && parentId != BOARD_UNKNOWN_ID && pca64xxData->select != NULL)
    {
        status = pca64xxData->deselect(parentId, chanIdx);
    }

    return status;
}

status_t BOARD_EXPANDER_SetPinToLow(uint32_t i2cDevId, uint8_t pinIdx)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    uint32_t parentId = g_i2cDevTbl[i2cDevArrayIdx].parent_id;
    uint8_t deviceType = (i2cDevId & I2C_DEVICE_TYPE_MASK) >> I2C_DEVICE_TYPE_SHIFT;
    uint8_t chanIdx = (i2cDevId & I2C_CHAN_IDX_MASK) >> I2C_CHAN_IDX_SHIFT;
    pca64xx_t *pca64xxData = NULL;
    status_t status = kStatus_Fail;

    (void)deviceType;
    assert(deviceType >= PCA954X_TYPE);
    assert(deviceType < PCA64XX_TYPE);

    if (g_i2cDevTbl[i2cDevArrayIdx].data == NULL)
    {
        BOARD_InitI2cDevice(i2cDevId);
    }

    pca64xxData = g_i2cDevTbl[i2cDevArrayIdx].data;
    assert(pca64xxData != NULL);

    if (parentId != BOARD_UNKNOWN_ID && pca64xxData->select != NULL)
    {
        status = pca64xxData->select(parentId, chanIdx);
    }
    if (status == kStatus_Success)
    {
        status = pca64xxData->set_pin_as_output(i2cDevId, pinIdx);
        if (status == kStatus_Success)
        {
            status = pca64xxData->set_pin_to_low(i2cDevId, pinIdx);
        }
    }

    if (status == kStatus_Success && parentId != BOARD_UNKNOWN_ID && pca64xxData->select != NULL)
    {
        status = pca64xxData->deselect(parentId, chanIdx);
    }
    return status;
}

status_t BOARD_EXPANDER_SetPinAsInput(uint32_t i2cDevId, uint8_t pinIdx)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    uint32_t parentId = g_i2cDevTbl[i2cDevArrayIdx].parent_id;
    uint8_t deviceType = (i2cDevId & I2C_DEVICE_TYPE_MASK) >> I2C_DEVICE_TYPE_SHIFT;
    uint8_t chanIdx = (i2cDevId & I2C_CHAN_IDX_MASK) >> I2C_CHAN_IDX_SHIFT;
    status_t status = kStatus_Fail;
    pca64xx_t *pca64xxData = NULL;

    (void)deviceType;
    assert(deviceType >= PCA954X_TYPE);
    assert(deviceType < PCA64XX_TYPE);

    if (g_i2cDevTbl[i2cDevArrayIdx].data == NULL)
    {
        BOARD_InitI2cDevice(i2cDevId);
    }

    pca64xxData = g_i2cDevTbl[i2cDevArrayIdx].data;

    assert(pca64xxData->set_pin_as_input != NULL);

    if (parentId != BOARD_UNKNOWN_ID && pca64xxData->select != NULL)
    {
        status = pca64xxData->select(parentId, chanIdx);
    }
    if (status == kStatus_Success)
    {
        status = pca64xxData->set_pin_as_input(i2cDevId, pinIdx);
    }

    if (status == kStatus_Success && parentId != BOARD_UNKNOWN_ID && pca64xxData->select != NULL)
    {
        status = pca64xxData->deselect(parentId, chanIdx);
    }
    return status;
}

status_t BOARD_EXPANDER_SetPinAsOutput(uint32_t i2cDevId, uint8_t pinIdx)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    uint32_t parentId = g_i2cDevTbl[i2cDevArrayIdx].parent_id;
    uint8_t deviceType = (i2cDevId & I2C_DEVICE_TYPE_MASK) >> I2C_DEVICE_TYPE_SHIFT;
    uint8_t chanIdx = (i2cDevId & I2C_CHAN_IDX_MASK) >> I2C_CHAN_IDX_SHIFT;
    status_t status = kStatus_Fail;
    pca64xx_t *pca64xxData = NULL;

    (void)deviceType;
    assert(deviceType >= PCA954X_TYPE);
    assert(deviceType < PCA64XX_TYPE);

    if (g_i2cDevTbl[i2cDevArrayIdx].data == NULL)
    {
        BOARD_InitI2cDevice(i2cDevId);
    }

    pca64xxData = g_i2cDevTbl[i2cDevArrayIdx].data;

    assert(pca64xxData->set_pin_as_input != NULL);

    if (parentId != BOARD_UNKNOWN_ID && pca64xxData->select != NULL)
    {
        status = pca64xxData->select(parentId, chanIdx);
    }
    if (status == kStatus_Success)
    {
        status = pca64xxData->set_pin_as_output(i2cDevId, pinIdx);
    }

    if (status == kStatus_Success && parentId != BOARD_UNKNOWN_ID && pca64xxData->select != NULL)
    {
        status = pca64xxData->deselect(parentId, chanIdx);
    }
    return status;
}
#endif

#if defined(BOARD_USE_PCA6408) && BOARD_USE_PCA6408
static void BOARD_InitPCA6408(uint32_t i2cDevId, void *base, uint32_t clkFreq, uint8_t i2cAddr)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    void *handle = g_i2cDevTbl[i2cDevArrayIdx].handle;

    BOARD_I2C_DeviceInit(base, clkFreq);

    pcal6408_config_t config = {
        .i2cBase         = base,
        .i2cAddr         = i2cAddr,
        .I2C_SendFunc    = BOARD_I2C_DeviceSend,
        .I2C_ReceiveFunc = BOARD_I2C_DeviceReceive,
    };

    PCAL6408_Init(handle, &config);
}

/*
 * Configures signal to 1
 */
static status_t BOARD_PCA6408_SetPinToHigh(uint32_t i2cDevId, uint8_t pinIdx)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    void *handle = g_i2cDevTbl[i2cDevArrayIdx].handle;

   return PCAL6408_SetPins(handle, (1U << pinIdx));
}

/*
 * Configures signal to 0
 */
static status_t BOARD_PCA6408_SetPinToLow(uint32_t i2cDevId, uint8_t pinIdx)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    void *handle = g_i2cDevTbl[i2cDevArrayIdx].handle;

   return PCAL6408_ClearPins(handle, (1U << pinIdx));
}

/*
 * Configures signal as input
 */
static status_t BOARD_PCA6408_SetPinAsInput(uint32_t i2cDevId, uint8_t pinIdx)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    void *handle = g_i2cDevTbl[i2cDevArrayIdx].handle;

   return PCAL6408_SetDirection(handle, (1U << pinIdx), kPCAL6408_Input);
}

/*
 * Configures signal as output
 */
static status_t BOARD_PCA6408_SetPinAsOutput(uint32_t i2cDevId, uint8_t pinIdx)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    void *handle = g_i2cDevTbl[i2cDevArrayIdx].handle;

   return PCAL6408_SetDirection(handle, (1U << pinIdx), kPCAL6408_Output);
}

#endif /* BOARD_USE_PCA6408. */

#if defined(BOARD_USE_PCA6416) && BOARD_USE_PCA6416
static void BOARD_InitPCA6416(uint32_t i2cDevId, void *base, uint32_t clkFreq, uint8_t i2cAddr)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    void *handle = g_i2cDevTbl[i2cDevArrayIdx].handle;

    BOARD_I2C_DeviceInit(base, clkFreq);

    pca6416a_config_t config = {
        .i2cBase         = base,
        .i2cAddr         = i2cAddr,
        .I2C_SendFunc    = BOARD_I2C_DeviceSend,
        .I2C_ReceiveFunc = BOARD_I2C_DeviceReceive,
    };

    PCA6416A_Init(handle, &config);
}

/*
 * Configures signal to 1
 */
static status_t BOARD_PCA6416_SetPinToHigh(uint32_t i2cDevId, uint8_t pinIdx)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    void *handle = g_i2cDevTbl[i2cDevArrayIdx].handle;

   return PCA6416A_SetPins(handle, (1U << pinIdx));
}

/*
 * Configures signal to 0
 */
static status_t BOARD_PCA6416_SetPinToLow(uint32_t i2cDevId, uint8_t pinIdx)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    void *handle = g_i2cDevTbl[i2cDevArrayIdx].handle;

   return PCA6416A_ClearPins(handle, (1U << pinIdx));
}

/*
 * Configures signal as input
 */
static status_t BOARD_PCA6416_SetPinAsInput(uint32_t i2cDevId, uint8_t pinIdx)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    void *handle = g_i2cDevTbl[i2cDevArrayIdx].handle;

   return PCA6416A_SetDirection(handle, (1U << pinIdx), kPCA6416A_Input);
}

/*
 * Configures signal as output
 */
static status_t BOARD_PCA6416_SetPinAsOutput(uint32_t i2cDevId, uint8_t pinIdx)
{
    uint8_t i2cDevArrayIdx = (i2cDevId & I2C_DEVICE_ARRAY_IDX_MASK) >> I2C_DEVICE_ARRAY_IDX_SHIFT;
    void *handle = g_i2cDevTbl[i2cDevArrayIdx].handle;

   return PCA6416A_SetDirection(handle, (1U << pinIdx), kPCA6416A_Output);
}
#endif /* BOARD_USE_PCA6416. */

void pin_mux_lpuart1(void)
{
	IOMUXC_SetPinMux(IOMUXC_PAD_UART1_RXD__LPUART1_RX, 0U);
	IOMUXC_SetPinMux(IOMUXC_PAD_UART1_TXD__LPUART1_TX, 0U);

	IOMUXC_SetPinConfig(IOMUXC_PAD_UART1_RXD__LPUART1_RX, IOMUXC_PAD_PD_MASK);
	IOMUXC_SetPinConfig(IOMUXC_PAD_UART1_TXD__LPUART1_TX, IOMUXC_PAD_DSE(15U));
}

void pin_mux_lpuart8(void)
{
	IOMUXC_SetPinMux(IOMUXC_PAD_DAP_TCLK_SWCLK__LPUART8_RX, 0U);
	IOMUXC_SetPinMux(IOMUXC_PAD_DAP_TMS_SWDIO__LPUART8_TX, 0U);

	IOMUXC_SetPinConfig(IOMUXC_PAD_DAP_TCLK_SWCLK__LPUART8_RX, IOMUXC_PAD_PD_MASK);
	IOMUXC_SetPinConfig(IOMUXC_PAD_DAP_TMS_SWDIO__LPUART8_TX, IOMUXC_PAD_DSE(15U));
}

void clock_config_lpuart1(void)
{
	/* clang-format off */
	clk_t clk = {
		.clkId = kCLOCK_Lpuart1,
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 24000000UL,
	};
	/* clang-format on */

	CLOCK_SetRate(&clk);
	CLOCK_EnableClock(clk.clkId);
}

void clock_config_lpuart8(void)
{
	/* clang-format off */
	clk_t clk = {
		.clkId = kCLOCK_Lpuart8,
		.clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
		.rate = 24000000UL,
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
#elif (BOARD_DEBUG_UART_INSTANCE == 8)
	pin_mux_lpuart8();
	clock_config_lpuart8();
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
