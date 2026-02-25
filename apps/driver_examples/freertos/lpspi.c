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

#define lpspi_slave_handle lpspi_slave_handle_t
#define lpspi_master_handle lpspi_master_handle_t

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
static uint8_t s_masterRxData[TRANSFER_SIZE] = {0};
static uint8_t s_masterTxData[TRANSFER_SIZE] = {0};
static uint8_t s_slaveRxData[TRANSFER_SIZE] = {0};
static uint8_t s_slaveTxData[TRANSFER_SIZE] = {0};

static lpspi_slave_handle s_slave_handle;
static lpspi_master_handle s_master_handle;

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

/*!
 * @brief Main function
 */
int lpspi_base_test(void)
{
	lpspi_master_config_t masterConfig;
	lpspi_slave_config_t slaveConfig;
	lpspi_transfer_t masterXfer;
	lpspi_transfer_t slaveXfer;
	uint32_t errorCount;
	uint32_t i = 0;

	PRINTF("\r\n%d. LPSPI transfer example\r\n", get_test_idx());
	PRINTF("This example use one lpspi instance as master and another as slave on one board.\r\n");

	LPSPI_ShowPinCfg();

	PRINTF("\r\nthe connection is: \r\n");
	PRINTF("LPSPI_master -- LPSPI_slave \r\n");
	PRINTF("   CLK      --    CLK  \r\n");
	PRINTF("   PCS      --    PCS  \r\n");
	PRINTF("   SOUT     --    SIN  \r\n");
	PRINTF("   SIN      --    SOUT \r\n");

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
	LPSPI_SlaveTransferCreateHandle(LPSPI_SLAVE_BASEADDR, &s_slave_handle, LPSPI_SlaveUserCallback, NULL);

	/*Set slave transfer ready to receive/send data*/
	slaveXfer.txData = s_slaveTxData;
	slaveXfer.rxData = s_slaveRxData;
	slaveXfer.dataSize = TRANSFER_SIZE;
	slaveXfer.configFlags = LPSPI_SLAVE_PCS_FOR_TRANSFER | kLPSPI_SlaveByteSwap;

	s_slave_done = false;
	LPSPI_SlaveTransferNonBlocking(LPSPI_SLAVE_BASEADDR, &s_slave_handle, &slaveXfer);

	/*Set up master transfer*/
	LPSPI_MasterTransferCreateHandle(LPSPI_MASTER_BASEADDR, &s_master_handle, LPSPI_MasterUserCallback, NULL);

	/*Start master transfer*/
	masterXfer.txData = s_masterTxData;
	masterXfer.rxData = s_masterRxData;
	masterXfer.dataSize = TRANSFER_SIZE;
	masterXfer.configFlags = LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterPcsContinuous | kLPSPI_SlaveByteSwap;

	s_master_done = false;
	LPSPI_MasterTransferNonBlocking(LPSPI_MASTER_BASEADDR, &s_master_handle, &masterXfer);

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
		PRINTF("LPSPI transfer test is Passed!\r\n");
	else
		PRINTF("LPSPI transfer test is Failed!\r\n");

	LPSPI_Deinit(LPSPI_MASTER_BASEADDR);
	LPSPI_Deinit(LPSPI_SLAVE_BASEADDR);

	return 0;
}

int lpspi_test(void)
{
	lpspi_base_test();

#ifdef ENABLE_LPSPI_EDMA
	lpspi_edma_test();
#endif

	return 0;
}
