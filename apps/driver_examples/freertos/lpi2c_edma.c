/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2026 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "fsl_lpi2c_edma.h"
#include "fsl_edma.h"
#include "lpi2c.h"
#include "board.h"
#include "app.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LPI2C_MASTER ((LPI2C_Type *)LPI2C_MASTER_BASE)
#define LPI2C_SLAVE  ((LPI2C_Type *)LPI2C_SLAVE_BASE)

#define I2C_MASTER_SLAVE_ADDR_7BIT (0x7EU)
#define I2C_BAUDRATE               (100000) /* 100K */
#define I2C_DATA_LENGTH            (32) /* MAX is 256 */

#define AT_INIT AT_NONCACHEABLE_SECTION_INIT
#define AT_SECT AT_NONCACHEABLE_SECTION

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
AT_INIT(static uint8_t s_slave_buff[I2C_DATA_LENGTH]) = {0};
AT_INIT(static uint8_t s_master_buff[I2C_DATA_LENGTH]) = {0};

AT_SECT(static lpi2c_master_edma_handle_t s_master_handle);
static lpi2c_slave_handle_t s_slave_handle;

static edma_handle_t s_edmaTxHandle;
static edma_handle_t s_edmaRxHandle;

static volatile bool s_slave_done = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
static void lpi2c_slave_callback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *param)
{
	switch (xfer->event) {
		/* Setup the slave receive buffer */
		case kLPI2C_SlaveReceiveEvent:
			xfer->data = s_slave_buff;
			xfer->dataSize = I2C_DATA_LENGTH;
			break;

		/* The master has sent a stop transition on the bus */
		case kLPI2C_SlaveCompletionEvent:
			s_slave_done = true;
			break;

		/* Transmit event not handled in this demo */
		case kLPI2C_SlaveTransmitEvent:
		default:
			break;
	}
}

int lpi2c_edma_test(void)
{
	lpi2c_master_transfer_t masterXfer = {0};
	lpi2c_master_config_t masterConfig;
	lpi2c_slave_config_t slaveConfig;
	edma_config_t edmaConfig;
	status_t reVal;
	uint32_t i = 0;
	int rx_ok = 0;

	PRINTF("\r\n%d. LPI2C EDMA transfer example\r\n", get_test_idx());
	PRINTF("This example use one lpi2c instance as controller and another as target on one board.\r\n");

	LPI2C_ShowPinCfg();

	PRINTF("\r\nthe connection is: \r\n");
	PRINTF("LPI2C_controller -- LPI2C_target   \r\n");
	PRINTF("   SDA      --    SDA  \r\n");
	PRINTF("   SCL      --    SCL \r\n");

#ifdef IRQ_STEER_EDMA
	IRQSTEER_EnableInterrupt(IRQSTEERA55_INST, (IRQn_Type)(IRQ_BEG_IDX + 129));
#endif

	/* EDMA init */
	EDMA_GetDefaultConfig(&edmaConfig);
	EDMA_Init(LPI2C_MASTER_DMA_BASE, &edmaConfig);

	LPI2C_SlaveGetDefaultConfig(&slaveConfig);

	/* Change the slave address */
	slaveConfig.address0 = I2C_MASTER_SLAVE_ADDR_7BIT;

	/* Initialize the LPI2C slave peripheral */
	LPI2C_SlaveInit(LPI2C_SLAVE, &slaveConfig, LPI2C_SLAVE_CLOCK_FREQUENCY);

	/* Create the LPI2C handle for the non-blocking transfer */
	LPI2C_SlaveTransferCreateHandle(LPI2C_SLAVE, &s_slave_handle, lpi2c_slave_callback, NULL);

	/* Start accepting I2C transfers on the LPI2C slave peripheral */
	reVal = LPI2C_SlaveTransferNonBlocking(LPI2C_SLAVE, &s_slave_handle, kLPI2C_SlaveReceiveEvent | kLPI2C_SlaveCompletionEvent);
	if (reVal != kStatus_Success) {
		PRINTF("LPI2C_SlaveTransferNonBlocking is failed! reVal:%d\r\n", reVal);
		return -1;
	}

	/* Set up i2c master to send data to slave */
	PRINTF("\r\nController will send data :");
	for (i = 0; i < I2C_DATA_LENGTH; i++) {
		s_master_buff[i] = i;
		s_slave_buff[i] = 0;

		if (i % 8 == 0)
			PRINTF("\r\n");
		PRINTF("0x%02X ", s_master_buff[i]);
	}
	PRINTF("\r\n");

	LPI2C_MasterGetDefaultConfig(&masterConfig);

	/* Change the default baudrate configuration */
	masterConfig.baudRate_Hz = I2C_BAUDRATE;

	/* Initialize the LPI2C master peripheral */
	LPI2C_MasterInit(LPI2C_MASTER, &masterConfig, LPI2C_MASTER_CLOCK_FREQUENCY);

	/* Create the EDMA channel handles */
	EDMA_CreateHandle(&s_edmaRxHandle, LPI2C_MASTER_DMA, LPI2C_MASTER_DMA_RX_CHANNEL);
	EDMA_CreateHandle(&s_edmaTxHandle, LPI2C_MASTER_DMA, LPI2C_MASTER_DMA_TX_CHANNEL);

#if defined(FSL_FEATURE_EDMA_HAS_CHANNEL_MUX) && FSL_FEATURE_EDMA_HAS_CHANNEL_MUX
	EDMA_SetChannelMux(LPI2C_MASTER_DMA_BASE, LPI2C_MASTER_DMA_RX_CHANNEL, LPI2C_MASTER_DMA_RX_REQ);
	EDMA_SetChannelMux(LPI2C_MASTER_DMA_BASE, LPI2C_MASTER_DMA_TX_CHANNEL, LPI2C_MASTER_DMA_TX_REQ);
#endif

	/* Create the LPI2C master DMA driver handle */
	LPI2C_MasterCreateEDMAHandle(LPI2C_MASTER, &s_master_handle, &s_edmaRxHandle, &s_edmaTxHandle, NULL, NULL);

	/* Setup the master transfer */
	masterXfer.slaveAddress = I2C_MASTER_SLAVE_ADDR_7BIT;
	masterXfer.direction = kLPI2C_Write;
	masterXfer.subaddress = 0;
	masterXfer.subaddressSize = 0;
	masterXfer.data = s_master_buff;
	masterXfer.dataSize = I2C_DATA_LENGTH;
	masterXfer.flags = kLPI2C_TransferDefaultFlag;

	/* Send master non-blocking data to slave */
	reVal = LPI2C_MasterTransferEDMA(LPI2C_MASTER, &s_master_handle, &masterXfer);
	if (reVal != kStatus_Success) {
		PRINTF("LPI2C_MasterTransferNonBlocking is failed! reVal:%d\r\n", reVal);
		return -2;
	}

	/* Wait for the transfer to complete. */
	while (!s_slave_done);

	/* Display the slave data */
	PRINTF("\r\nTarget received data :");
	for (i = 0; i < I2C_DATA_LENGTH; i++) {
		if (i % 8 == 0)
			PRINTF("\r\n");
		PRINTF("0x%02X ", s_slave_buff[i]);
	}
	PRINTF("\r\n");

	/* Transfer completed. Check the data. */
	rx_ok = 1;
	for (i = 0; i < I2C_DATA_LENGTH; i++) {
		if (s_slave_buff[i] != s_master_buff[i]) {
			PRINTF("\r\nError occurred! i:%d master_tx:%02X slave_rx:%02X\r\n", i, s_master_buff[i], s_slave_buff[i]);
			rx_ok = 0;
			break;
		}
	}

	PRINTF("\r\n");
	if (rx_ok)
		PRINTF("LPI2C EDMA transfer test is Passed!\r\n");
	else
		PRINTF("LPI2C EDMA transfer test is Failed!\r\n");

	return 0;
}
