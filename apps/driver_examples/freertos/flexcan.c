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
#ifdef USE_CANFD
/*
 *    DWORD_IN_MB    DLC    BYTES_IN_MB             Maximum MBs
 *    2              8      kFLEXCAN_8BperMB    32(1 RAM block)  64(2 RAM block)  96(3 RAM block)
 *    4              10     kFLEXCAN_16BperMB   21(1 RAM block)  42(2 RAM block)  63(3 RAM block)
 *    8              13     kFLEXCAN_32BperMB   12(1 RAM block)  24(2 RAM block)  36(3 RAM block)
 *    16             15     kFLEXCAN_64BperMB   7(1 RAM block)   14(2 RAM block)  21(3 RAM block)
 *
 * Dword in each message buffer, Length of data in bytes, Payload size must align,
 * and the Message Buffers are limited corresponding to each payload configuration:
 */
#define DWORD_IN_MB	(16)
#define DLC		(15)
#define BYTES_IN_MB	kFLEXCAN_64BperMB
#else
#define DLC		(8)
#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static CAN_Type *s_flexcan[MAX_IDX] = {FLEX_CAN_ID1, FLEX_CAN_ID2};
static Flexcan_Session_t s_flexcan_session[MAX_IDX];

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

	switch (status) {
		/* Process FlexCAN Rx event. */
		case kStatus_FLEXCAN_RxIdle:
			if (ps->rx_mbuf_num == result)
				ps->rxDone = true;
			break;

		/* Process FlexCAN Tx event. */
		case kStatus_FLEXCAN_TxIdle:
			if (ps->tx_mbuf_num == result)
				ps->txDone = true;
			break;

		default:
			break;
	}
}

static int flexcan_init_port(int port, bool loopback)
{
	flexcan_rx_mb_config_t mbConfig;
	flexcan_config_t flexcanConfig;
	Flexcan_Session_t *ps = NULL;

	if ((port < 0) || (port >= MAX_IDX))
		return -1;

	ps = s_flexcan_session + port;
	if (!ps->pflexcan) {
		ps->pflexcan = s_flexcan[port];
		ps->can_id = port;
		ps->rx_mbuf_num = RX_MESSAGE_BUFFER_NUM + port;
		ps->tx_mbuf_num = TX_MESSAGE_BUFFER_NUM + port;

		if (port == CAN_IDX1)
			ps->srcClockHz = FLEX_CAN_CLK_FREQ1;
		else
			ps->srcClockHz = FLEX_CAN_CLK_FREQ2;
	}

	FLEXCAN_GetDefaultConfig(&flexcanConfig);
	flexcanConfig.enableLoopBack = loopback;

#ifdef USE_IMPROVED_TIMING_CONFIG
	flexcan_timing_config_t timing_config;
	memset(&timing_config, 0, sizeof(flexcan_timing_config_t));

	/* Update the improved timing configuration*/
#ifdef USE_CANFD
	if (FLEXCAN_FDCalculateImprovedTimingValues(ps->pflexcan, flexcanConfig.bitRate, flexcanConfig.bitRateFD, ps->srcClockHz, &timing_config))
		memcpy(&flexcanConfig.timingConfig, &timing_config, sizeof(flexcan_timing_config_t));
	else
		PRINTF("No found Improved Timing Configuration. Just used default configuration\r\n\r\n");
#else
	if (FLEXCAN_CalculateImprovedTimingValues(ps->pflexcan, flexcanConfig.bitRate, ps->srcClockHz, &timing_config))
		memcpy(&flexcanConfig.timingConfig, &timing_config, sizeof(flexcan_timing_config_t));
	else
		PRINTF("No found Improved Timing Configuration. Just used default configuration\r\n\r\n");
#endif
#endif

#ifdef USE_CANFD
	FLEXCAN_FDInit(ps->pflexcan, &flexcanConfig, ps->srcClockHz, BYTES_IN_MB, true);
#else
	FLEXCAN_Init(ps->pflexcan, &flexcanConfig, ps->srcClockHz);
#endif

	/* Setup Rx Message Buffer. */
	mbConfig.format = kFLEXCAN_FrameFormatStandard;
	mbConfig.type   = kFLEXCAN_FrameTypeData;
	mbConfig.id = FLEXCAN_ID_STD(0x123);
#ifdef USE_CANFD
	FLEXCAN_SetFDRxMbConfig(ps->pflexcan, ps->rx_mbuf_num, &mbConfig, true);
#else
	FLEXCAN_SetRxMbConfig(ps->pflexcan, ps->rx_mbuf_num, &mbConfig, true);
#endif

/* Setup Tx Message Buffer. */
#ifdef USE_CANFD
	FLEXCAN_SetFDTxMbConfig(ps->pflexcan, ps->tx_mbuf_num, true);
#else
	FLEXCAN_SetTxMbConfig(ps->pflexcan, ps->tx_mbuf_num, true);
#endif

	/* Create FlexCAN handle structure and set call back function. */
	FLEXCAN_TransferCreateHandle(ps->pflexcan, &ps->handle, flexcan_callback, &ps->can_id);

	/* Start receive data through Rx Message Buffer. */
	ps->rxXfer.mbIdx = (uint8_t)ps->rx_mbuf_num;

	/* Prepare Tx Frame for sending. */
	ps->txFrame.format = (uint8_t)kFLEXCAN_FrameFormatStandard;
	ps->txFrame.type   = (uint8_t)kFLEXCAN_FrameTypeData;
	ps->txFrame.id = FLEXCAN_ID_STD(0x123);
	ps->txFrame.length = (uint8_t)DLC;
#ifdef USE_CANFD
	ps->txFrame.brs = 1U;
	ps->txFrame.edl = 1U;
#endif

#ifdef USE_CANFD
	for (int i = 0; i < DWORD_IN_MB; i++) {
		ps->txFrame.dataWord[i] = (uint32_t)(i + 0x10 * port);
		ps->rxFrame.dataWord[i] = 0;
	}
	ps->rxXfer.framefd = &ps->rxFrame;
	ps->txXfer.framefd = &ps->txFrame;
#else
	ps->txFrame.dataWord0 = CAN_WORD0_DATA_BYTE_0(0x11) | CAN_WORD0_DATA_BYTE_1(0x22) | CAN_WORD0_DATA_BYTE_2(0x33) | CAN_WORD0_DATA_BYTE_3(0x44);
	ps->txFrame.dataWord1 = CAN_WORD1_DATA_BYTE_4(0x55) | CAN_WORD1_DATA_BYTE_5(0x66) | CAN_WORD1_DATA_BYTE_6(0x77) | CAN_WORD1_DATA_BYTE_7(0x88);
	ps->rxXfer.frame = &ps->rxFrame;
	ps->txXfer.frame = &ps->txFrame;
#endif

	/* Send data through Tx Message Buffer. */
	ps->txXfer.mbIdx = (uint8_t)ps->tx_mbuf_num;
	ps->rxDone = false;
	ps->txDone = false;

	return 0;
}

static int flexcan_test_port(bool loopback)
{
	Flexcan_Session_t *ps_rx = NULL;
	Flexcan_Session_t *ps_tx = NULL;
	int rx_port = CAN_IDX2;
	int tx_port = CAN_IDX1;

	ps_rx = s_flexcan_session + rx_port;
	if (loopback) {
		tx_port = rx_port;
		ps_tx = ps_rx;
	} else {
		ps_tx = s_flexcan_session + tx_port;
	}

#ifdef USE_CANFD
	FLEXCAN_TransferFDReceiveNonBlocking(ps_rx->pflexcan, &ps_rx->handle, &ps_rx->rxXfer);
#else
	FLEXCAN_TransferReceiveNonBlocking(ps_rx->pflexcan, &ps_rx->handle, &ps_rx->rxXfer);
#endif

	PRINTF("\r\ntx_port:%d Send message from MB%d to MB%d\r\n", tx_port, ps_tx->tx_mbuf_num, ps_rx->rx_mbuf_num);

#ifdef USE_CANFD
	FLEXCAN_TransferFDSendNonBlocking(ps_tx->pflexcan, &ps_tx->handle, &ps_tx->txXfer);
#else
	FLEXCAN_TransferSendNonBlocking(ps_tx->pflexcan, &ps_tx->handle, &ps_tx->txXfer);
#endif

	/* Waiting for Rx Message finish */
	while (!ps_rx->rxDone || !ps_tx->txDone);

	PRINTF("\r\nrx_port:%d Received message from MB%d\r\n", rx_port, ps_rx->rx_mbuf_num);

#ifdef USE_CANFD
	ps_rx->rx_ok = 1;
	for (int i = 0; i < DWORD_IN_MB; i++) {
		PRINTF("rx_port:%d rx_word%02d = 0x%02X tx_word%02d = 0x%02X\r\n", rx_port, i, ps_rx->rxFrame.dataWord[i], i, ps_tx->txFrame.dataWord[i]);
		if (ps_rx->rxFrame.dataWord[i] != ps_tx->txFrame.dataWord[i])
			ps_rx->rx_ok = 0;
	}
#else
	PRINTF("rx_port:%d rx word0 = 0x%X, word1 = 0x%X\r\n", rx_port, ps_rx->rxFrame.dataWord0, ps_rx->rxFrame.dataWord1);
	PRINTF("tx_port:%d tx word0 = 0x%X, word1 = 0x%X\r\n", tx_port, ps_tx->txFrame.dataWord0, ps_tx->txFrame.dataWord1);
	if ((ps_rx->rxFrame.dataWord0 == ps_tx->txFrame.dataWord0) && (ps_rx->rxFrame.dataWord1 == ps_tx->txFrame.dataWord1))
		ps_rx->rx_ok = 1;
#endif

	PRINTF("\r\n");
	if (ps_rx->rx_ok)
		PRINTF("FLEXCAN transfer test is Passed!\r\n");
	else
		PRINTF("FLEXCAN transfer test is Failed!\r\n");

	return 0;
}

/*!
 * @brief Main function
 */
int flexcan_base_test(void)
{
	bool loopback = false;
	uint8_t i = 0;

	PRINTF("\r\n%d. FlexCAN transfer example\r\n", get_test_idx());

	FLEXCAN_ShowPinCfg();

	for (i = 0; i < MAX_IDX; i++)
		flexcan_init_port(i, loopback);

	flexcan_test_port(loopback);

	return 0;
}

int flexcan_test(void)
{
	flexcan_base_test();

#ifdef ENABLE_FLEXCAN_EDMA
	flexcan_edma_test();
#endif

	return 0;
}
