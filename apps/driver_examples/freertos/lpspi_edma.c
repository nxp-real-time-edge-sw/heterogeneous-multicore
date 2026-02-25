/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2026 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_lpspi.h"
#include "lpspi.h"
#include "board.h"
#include "app.h"

#include "fsl_lpspi_edma.h"
#include "fsl_edma.h"

#define AT_INIT AT_NONCACHEABLE_SECTION_INIT
#define AT_SECT AT_NONCACHEABLE_SECTION

#define lpspi_slave_handle lpspi_slave_edma_handle_t
#define lpspi_master_handle lpspi_master_edma_handle_t

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define TRANSFER_SIZE		(512U) /*! Transfer dataSize.*/
#define TRANSFER_BAUDRATE	(500000U) /*! Transfer baudrate - 500k */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern uint32_t LPSPI_GetInstance(LPSPI_Type *base);
/*******************************************************************************
 * Variables
 ******************************************************************************/
AT_INIT(static uint8_t s_masterRxData[TRANSFER_SIZE]) = {0};
AT_INIT(static uint8_t s_masterTxData[TRANSFER_SIZE]) = {0};
AT_INIT(static uint8_t s_slaveRxData[TRANSFER_SIZE]) = {0};
AT_INIT(static uint8_t s_slaveTxData[TRANSFER_SIZE]) = {0};

static edma_handle_t s_masterRxHandle;
static edma_handle_t s_masterTxHandle;
static edma_handle_t s_slaveRxHandle;
static edma_handle_t s_slaveTxHandle;

AT_SECT(static lpspi_slave_handle s_slave_handle);
AT_SECT(static lpspi_master_handle s_master_handle);

static volatile bool s_slave_done = false;
static volatile bool s_master_done = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
static void LPSPI_SlaveUserCallback(LPSPI_Type *base, lpspi_slave_handle *handle, status_t status, void *userData)
{
	if (status == kStatus_Success)
		PRINTF("LPSPI slave completed callback. Success!\r\n");
	else if (status == kStatus_LPSPI_Error)
		PRINTF("LPSPI slave completed callback. Error!\r\n");
	else
		__NOP();

	s_slave_done = true;
}

static void LPSPI_MasterUserCallback(LPSPI_Type *base, lpspi_master_handle *handle, status_t status, void *userData)
{
	if (status == kStatus_Success)
		PRINTF("LPSPI master completed callback. Success!\r\n");
	else if (status == kStatus_LPSPI_Error)
		PRINTF("LPSPI master completed callback. Error!\r\n");
	else
		__NOP();

	s_master_done = true;
}

void LPSPI_SetupSlaveEDMA(lpspi_slave_handle *p)
{
	EDMA_CreateHandle(&s_slaveRxHandle, LPSPI_SLAVE_DMA_BASEADDR, LPSPI_SLAVE_DMA_RX_CHANNEL);
	EDMA_CreateHandle(&s_slaveTxHandle, LPSPI_SLAVE_DMA_BASEADDR, LPSPI_SLAVE_DMA_TX_CHANNEL);

#if defined(FSL_FEATURE_EDMA_HAS_CHANNEL_MUX) && FSL_FEATURE_EDMA_HAS_CHANNEL_MUX
	EDMA_SetChannelMux(LPSPI_SLAVE_DMA_BASEADDR, LPSPI_SLAVE_DMA_TX_CHANNEL, LPSPI_SLAVE_DMA_TX_REQ);
	EDMA_SetChannelMux(LPSPI_SLAVE_DMA_BASEADDR, LPSPI_SLAVE_DMA_RX_CHANNEL, LPSPI_SLAVE_DMA_RX_REQ);
#endif

	LPSPI_SlaveTransferCreateHandleEDMA(LPSPI_SLAVE_BASEADDR, p, LPSPI_SlaveUserCallback, NULL, &s_slaveRxHandle, &s_slaveTxHandle);
}

void LPSPI_SetupMasterEDMA(lpspi_master_handle *p)
{
	EDMA_CreateHandle(&s_masterRxHandle, LPSPI_MASTER_DMA_BASEADDR, LPSPI_MASTER_DMA_RX_CHANNEL);
	EDMA_CreateHandle(&s_masterTxHandle, LPSPI_MASTER_DMA_BASEADDR, LPSPI_MASTER_DMA_TX_CHANNEL);

#if defined(FSL_FEATURE_EDMA_HAS_CHANNEL_MUX) && FSL_FEATURE_EDMA_HAS_CHANNEL_MUX
	EDMA_SetChannelMux(LPSPI_MASTER_DMA_BASEADDR, LPSPI_MASTER_DMA_TX_CHANNEL, LPSPI_MASTER_DMA_TX_REQ);
	EDMA_SetChannelMux(LPSPI_MASTER_DMA_BASEADDR, LPSPI_MASTER_DMA_RX_CHANNEL, LPSPI_MASTER_DMA_RX_REQ);
#endif

	LPSPI_MasterTransferCreateHandleEDMA(LPSPI_MASTER_BASEADDR, p, LPSPI_MasterUserCallback, NULL, &s_masterRxHandle, &s_masterTxHandle);
}

/*!
 * @brief Main function
 */
int lpspi_edma_test(void)
{
	lpspi_master_config_t masterConfig;
	lpspi_slave_config_t slaveConfig;
	lpspi_transfer_t masterXfer;
	lpspi_transfer_t slaveXfer;
	edma_config_t edmaConfig;
	uint32_t errorCount;
	uint32_t i = 0;

	PRINTF("\r\n%d. LPSPI EDMA transfer example\r\n", get_test_idx());
	PRINTF("This example use one lpspi instance as master and another as slave on one board.\r\n");

	LPSPI_ShowPinCfg();

	PRINTF("\r\nthe connection is: \r\n");
	PRINTF("LPSPI_master -- LPSPI_slave \r\n");
	PRINTF("   CLK      --    CLK  \r\n");
	PRINTF("   PCS      --    PCS  \r\n");
	PRINTF("   SOUT     --    SIN  \r\n");
	PRINTF("   SIN      --    SOUT \r\n");

#ifdef IRQ_STEER_EDMA
	IRQSTEER_EnableInterrupt(IRQSTEERA55_INST, (IRQn_Type)(IRQ_BEG_IDX + 129));
	IRQSTEER_EnableInterrupt(IRQSTEERA55_INST, (IRQn_Type)(IRQ_BEG_IDX + 3));
#endif

	EDMA_GetDefaultConfig(&edmaConfig);
	EDMA_Init(LPSPI_MASTER_DMA_BASEADDR, &edmaConfig);
#if (LPSPI_SLAVE_DMA_BASE != LPSPI_MASTER_DMA_BASE)
	EDMA_Init(LPSPI_SLAVE_DMA_BASEADDR, &edmaConfig);
#endif

	/*Set up the transfer data*/
	for (i = 0; i < TRANSFER_SIZE; i++) {
		s_masterTxData[i] = i % 256;
		s_masterRxData[i] = 0;

		s_slaveTxData[i] = ~s_masterTxData[i];
		s_slaveRxData[i] = 0;
	}

	/*Master config*/
	LPSPI_MasterGetDefaultConfig(&masterConfig);

	masterConfig.baudRate = TRANSFER_BAUDRATE;
	masterConfig.whichPcs = LPSPI_MASTER_PCS_FOR_INIT;

	LPSPI_MasterInit(LPSPI_MASTER_BASEADDR, &masterConfig, LPSPI_MASTER_CLOCK_FREQ);

	/*Slave config*/
	LPSPI_SlaveGetDefaultConfig(&slaveConfig);
	slaveConfig.whichPcs = LPSPI_SLAVE_PCS_FOR_INIT;

	LPSPI_SlaveInit(LPSPI_SLAVE_BASEADDR, &slaveConfig);

	/*Set up slave first */
	LPSPI_SetupSlaveEDMA(&s_slave_handle);

	/*Set slave transfer ready to receive/send data*/
	slaveXfer.txData = s_slaveTxData;
	slaveXfer.rxData = s_slaveRxData;
	slaveXfer.dataSize = TRANSFER_SIZE;
	slaveXfer.configFlags = LPSPI_SLAVE_PCS_FOR_TRANSFER | kLPSPI_SlaveByteSwap;

	s_slave_done = false;
	LPSPI_SlaveTransferEDMA(LPSPI_SLAVE_BASEADDR, &s_slave_handle, &slaveXfer);

	/*Set up master transfer*/
	LPSPI_SetupMasterEDMA(&s_master_handle);

	/*Start master transfer*/
	masterXfer.txData = s_masterTxData;
	masterXfer.rxData = s_masterRxData;
	masterXfer.dataSize = TRANSFER_SIZE;
	masterXfer.configFlags = LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterPcsContinuous | kLPSPI_SlaveByteSwap;

	s_master_done = false;
	LPSPI_MasterTransferEDMA(LPSPI_MASTER_BASEADDR, &s_master_handle, &masterXfer);

	/*Wait slave received all data. Even though the LPSPI_MasterTransferBlocking is a blocking transfer, but slave may
	 * need a little bit of time to read the data of last frames from buffer.*/
	PRINTF("Waiting LPSPI slave transfer complete... \r\n");
	while (!s_slave_done || !s_master_done);

	errorCount = 0;
	for (i = 0; i < TRANSFER_SIZE; i++) {
		if (s_masterTxData[i] != s_slaveRxData[i])
			errorCount++;

		if (s_slaveTxData[i] != s_masterRxData[i])
			errorCount++;

		if (i < 8) {
			PRINTF("data-%d: MOSI 0x%02X==0x%02X\r\n", i, s_masterTxData[i], s_slaveRxData[i]);
			PRINTF("data-%d: MISO 0x%02X==0x%02X\r\n", i, s_slaveTxData[i], s_masterRxData[i]);
		}
	}

	PRINTF("\r\n");
	if (errorCount == 0)
		PRINTF("LPSPI EDMA transfer test is Passed!\r\n");
	else
		PRINTF("LPSPI EDMA transfer test is Failed!\r\n");

	LPSPI_Deinit(LPSPI_MASTER_BASEADDR);
	LPSPI_Deinit(LPSPI_SLAVE_BASEADDR);

	return 0;
}
