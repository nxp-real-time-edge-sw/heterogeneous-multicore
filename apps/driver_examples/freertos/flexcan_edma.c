/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2026 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "flexcan.h"
#include "board.h"
#include "app.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

static CAN_Type *s_flexcan[MAX_IDX] = {FLEX_CAN_ID1, FLEX_CAN_ID2};
static uint8_t s_dmaChannel[MAX_IDX] = {FLEXCAN_DMA_CHANNEL1, FLEXCAN_DMA_CHANNEL2};
static uint8_t s_dmaSource[MAX_IDX] = {FLEXCAN_DMA_SOURCE1, FLEXCAN_DMA_SOURCE2};
static Flexcan_Session_t s_flexcan_session[MAX_IDX];

AT_NONCACHEABLE_SECTION(flexcan_frame_t s_rxFrames[MAX_IDX][FLEX_RX_MESSAGE_NUM]);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief FlexCAN Call Back function
 */
static FLEXCAN_CALLBACK(flexcan_callback)
{
	uint8_t port = *(uint8_t*)userData;
	Flexcan_Session_t *ps = NULL;

	ps = s_flexcan_session + port;

	PRINTF("%s() is called! port:%d status:0x%X\r\n", __func__, port, status);

	/* Process FlexCAN Tx event. */
	if ((kStatus_FLEXCAN_TxIdle == status) && (ps->tx_mbuf_num == result))
		ps->txDone = true;
}

/*!
 * @brief FlexCAN DMA Call Back function
 */
static void flexcan_dma_callback(CAN_Type *base, flexcan_edma_handle_t *handle, status_t status, void *userData)
{
	uint8_t port = *(uint8_t*)userData;
	Flexcan_Session_t *ps = NULL;

	ps = s_flexcan_session + port;

	PRINTF("%s() is called! port:%d status:0x%X\r\n", __func__, port, status);

	/* Process FlexCAN Rx event. */
	if (kStatus_FLEXCAN_RxFifoIdle == status)
		ps->rxDone = true;
}

/*!
 * @brief Main function
 */
static uint32_t s_rxFilter[] = {FIFO_TYPE(0x321, 0, 0), FIFO_TYPE(0x321, 1, 0), FIFO_TYPE(0x123, 0, 0), FIFO_TYPE(0x123, 1, 0)};
static int flexcan_init_port(int port, bool loopback)
{
	flexcan_rx_fifo_config_t rxFifoConfig;
	flexcan_config_t flexcanConfig;
	edma_config_t edmaConfig;
	Flexcan_Session_t *ps = NULL;

	if ((port < 0) || (port >= MAX_IDX))
		return -1;

	ps = s_flexcan_session + port;

	if (!ps->pflexcan) {
		ps->pflexcan = s_flexcan[port];
		ps->can_id = port;
		ps->rx_mbuf_num = RX_MESSAGE_BUFFER_NUM + port;
		ps->tx_mbuf_num = TX_MESSAGE_BUFFER_NUM + port;
		ps->dmaChannel = s_dmaChannel[port];
		ps->dmaSource = s_dmaSource[port];

		if (port == CAN_IDX1) {
			ps->srcClockHz = FLEX_CAN_CLK_FREQ1;
			IRQSTEER_EnableInterrupt(IRQSTEERA55_INST, (IRQn_Type)(IRQ_BEG_IDX + 128 + ps->dmaChannel));  // CAN2
		} else {
			ps->srcClockHz = FLEX_CAN_CLK_FREQ2;
			IRQSTEER_EnableInterrupt(IRQSTEERA55_INST, (IRQn_Type)(IRQ_BEG_IDX + 160 + ps->dmaChannel));  // CAN4
		}
	}

	FLEXCAN_GetDefaultConfig(&flexcanConfig);

	/* Enable internal loopback. */
	flexcanConfig.enableLoopBack = loopback;

#ifdef USE_IMPROVED_TIMING_CONFIG
	flexcan_timing_config_t timing_config;
 
	memset(&timing_config, 0, sizeof(flexcan_timing_config_t));

	/* Update the improved timing configuration*/
	if (FLEXCAN_CalculateImprovedTimingValues(ps->pflexcan, flexcanConfig.bitRate, ps->srcClockHz, &timing_config))
		memcpy(&(flexcanConfig.timingConfig), &timing_config, sizeof(flexcan_timing_config_t));
	else
		PRINTF("No found Improved Timing Configuration. Just used default configuration\r\n\r\n");
#endif

	if (ps->srcClockHz > UINT32_MAX)
		return -1;

	FLEXCAN_Init(ps->pflexcan, &flexcanConfig, (uint32_t)ps->srcClockHz);

	EDMA_GetDefaultConfig(&edmaConfig);
	EDMA_Init(FLEX_CAN_DMA, &edmaConfig);

	/* Create EDMA handle. */
	EDMA_CreateHandle(&ps->rxEdmaHandle, FLEX_CAN_DMA, ps->dmaChannel);
#if defined(FSL_FEATURE_EDMA_HAS_CHANNEL_MUX) && FSL_FEATURE_EDMA_HAS_CHANNEL_MUX
	EDMA_SetChannelMux(FLEX_CAN_DMA, ps->dmaChannel, ps->dmaSource);
#endif

	/* Setup Tx Message Buffer. */
	FLEXCAN_SetTxMbConfig(ps->pflexcan, ps->tx_mbuf_num, true);

	/* Setup Rx FIFO. */
	rxFifoConfig.idFilterTable = s_rxFilter;
	rxFifoConfig.idFilterType = kFLEXCAN_RxFifoFilterTypeA;
	rxFifoConfig.idFilterNum = sizeof(s_rxFilter) / sizeof(s_rxFilter[0]);
	rxFifoConfig.priority = kFLEXCAN_RxFifoPrioHigh;
	FLEXCAN_SetRxFifoConfig(ps->pflexcan, &rxFifoConfig, true);

	/* Create FlexCAN handle structure and set call back function. */
	FLEXCAN_TransferCreateHandle(ps->pflexcan, &ps->handle, flexcan_callback, &ps->can_id);

	/* Create FlexCAN EDMA handle structure and set call back function. */
	FLEXCAN_TransferCreateHandleEDMA(ps->pflexcan, &ps->flexcanEdmaHandle, flexcan_dma_callback, &ps->can_id, &ps->rxEdmaHandle);
	/* Receive data through Rx FIFO. */
	ps->rxFifoXfer.frame = s_rxFrames[port];
	ps->rxFifoXfer.frameNum = FLEX_RX_MESSAGE_NUM;
	ps->rxDone = false;
	ps->txDone = false;

	return 0;
}

#define CAN_BYTE(x, y, z) CAN_WORD##x##_DATA_BYTE_##y(z)
static int flexcan_test_port(bool loopback)
{
	flexcan_frame_t *rxFrame = NULL;
	flexcan_frame_t *txFrame = NULL;
	Flexcan_Session_t *ps_rx = NULL;
	Flexcan_Session_t *ps_tx = NULL;
	int rx_port = CAN_IDX2;
	int tx_port = CAN_IDX1;
	uint32_t data = 0;
	uint32_t i = 0;

	ps_rx = s_flexcan_session + rx_port;
	if (loopback) {
		tx_port = rx_port;
		ps_tx = ps_rx;
	} else {
		ps_tx = s_flexcan_session + tx_port;
	}

	FLEXCAN_TransferReceiveFifoEDMA(ps_rx->pflexcan, &ps_rx->flexcanEdmaHandle, &ps_rx->rxFifoXfer);

	for (i = 0; i < FLEX_RX_MESSAGE_NUM; i++) {
		txFrame = &ps_tx->txFrames[i];
		data = 0x11 * (i + 1);

		/* Send message through Tx Message Buffer. */
		txFrame->format = (uint8_t)kFLEXCAN_FrameFormatStandard;
		txFrame->type = (uint8_t)kFLEXCAN_FrameTypeData;
		txFrame->id = FLEXCAN_ID_STD(0x123);
		txFrame->length = 8U;
		txFrame->dataWord0 = CAN_BYTE(0, 0, data) | CAN_BYTE(0, 1, data) | CAN_BYTE(0, 2, data) | CAN_BYTE(0, 3, data);
		txFrame->dataWord1 = CAN_BYTE(1, 4, data) | CAN_BYTE(1, 5, data) | CAN_BYTE(1, 6, data) | CAN_BYTE(1, 7, data);

		ps_tx->txXfer.frame = txFrame;
		ps_tx->txXfer.mbIdx = (uint8_t)ps_tx->tx_mbuf_num;
		FLEXCAN_TransferSendNonBlocking(ps_tx->pflexcan, &ps_tx->handle, &ps_tx->txXfer);

		while (!ps_tx->txDone);
		ps_tx->txDone = false;

		PRINTF("tx_port:%d Send Msg%d to Rx FIFO: word0 = 0x%x, word1 = 0x%x.\r\n", tx_port, i + 1, txFrame->dataWord0, txFrame->dataWord1);
	}

	while (!ps_rx->rxDone);
	ps_rx->rxDone = false;

	rxFrame = ps_rx->rxFifoXfer.frame;
	ps_rx->rx_ok = 1;
	for (i = 0; i < FLEX_RX_MESSAGE_NUM; i++) {
		PRINTF("rx_port:%d Receive Msg%d from FIFO: word0 = 0x%x, word1 = 0x%x, ID Filter Hit%d.\r\n",
		rx_port, i + 1, rxFrame[i].dataWord0, rxFrame[i].dataWord1, rxFrame[i].idhit);

		if (rxFrame[i].dataWord0 != ps_tx->txFrames[i].dataWord0)
			ps_rx->rx_ok = 0;
		else if (rxFrame[i].dataWord1 != ps_tx->txFrames[i].dataWord1)
			ps_rx->rx_ok = 0;
	}

	PRINTF("\r\n");
	if (ps_rx->rx_ok)
		PRINTF("FLEXCAN EDMA transfer test is Passed!\r\n");
	else
		PRINTF("FLEXCAN EDMA transfer test is Failed!\r\n");

	return 0;
}

/*!
 * @brief Main function
 */
int flexcan_edma_test(void)
{
	bool loopback = false;
	uint8_t i = 0;

	PRINTF("\r\n%d. FlexCAN EDMA transfer example\r\n", get_test_idx());

	FLEXCAN_ShowPinCfg();

	for (i = 0; i < MAX_IDX; i++)
		flexcan_init_port(i, loopback);

	flexcan_test_port(loopback);

	return 0;
}
