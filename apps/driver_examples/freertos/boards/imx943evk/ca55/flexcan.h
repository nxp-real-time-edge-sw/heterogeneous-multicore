/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FLEXCAN_APP_H_
#define _FLEXCAN_APP_H_

#include "fsl_flexcan.h"
#include "fsl_flexcan_edma.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*${macro:start}*/
/* Master related */

#define ENABLE_FLEXCAN_EDMA

#define USE_CANFD (1)

#define FLEX_CAN_ID1     CAN2
#define FLEX_CAN_ID2     CAN4

#define CAN_IDX1         (0)
#define CAN_IDX2         (1)
#define MAX_IDX          (2)

#define FLEXCAN_CLOCK_ROOT1   (kCLOCK_Can2)
#define FLEXCAN_CLOCK_ROOT2   (kCLOCK_Can4)

#define RX_MESSAGE_BUFFER_NUM (0)
#define TX_MESSAGE_BUFFER_NUM (9)

#define FLEX_CAN_CLK_FREQ1 (CLOCK_GetRate(FLEXCAN_CLOCK_ROOT1))
#define FLEX_CAN_CLK_FREQ2 (CLOCK_GetRate(FLEXCAN_CLOCK_ROOT2))

/* Set USE_IMPROVED_TIMING_CONFIG macro to use api to calculates the improved CAN / CAN FD timing values. */
#define USE_IMPROVED_TIMING_CONFIG (1U)

#ifdef ENABLE_FLEXCAN_EDMA
#define FLEX_CAN_DMA         (EDMA4)

#define FLEXCAN_DMA_CHANNEL1 (kDma4RequestMuxCan2)
#define FLEXCAN_DMA_SOURCE1  (kDma4RequestMuxCan2)

#define FLEXCAN_DMA_CHANNEL2 (kDma4RequestMuxCan4)
#define FLEXCAN_DMA_SOURCE2  (kDma4RequestMuxCan4)

#define FLEX_RX_MESSAGE_NUM  (4)
#define FIFO_TYPE            FLEXCAN_RX_FIFO_STD_FILTER_TYPE_A
#endif
/*${macro:end}*/

typedef struct _Flexcan_Session_t
{
	uint8_t can_id;
	uint8_t rx_ok;
	uint8_t rx_mbuf_num;
	uint8_t tx_mbuf_num;
	CAN_Type *pflexcan;
	flexcan_handle_t handle;
	flexcan_mb_transfer_t txXfer, rxXfer;
	volatile bool txDone, rxDone;
	uint64_t srcClockHz;
#ifdef USE_CANFD
	flexcan_fd_frame_t txFrame, rxFrame;
#else
	flexcan_frame_t txFrame, rxFrame;
#endif

#ifdef ENABLE_FLEXCAN_EDMA
	flexcan_edma_handle_t flexcanEdmaHandle;
	edma_handle_t rxEdmaHandle;
	flexcan_fifo_transfer_t rxFifoXfer;
	flexcan_frame_t txFrames[FLEX_RX_MESSAGE_NUM];
	uint8_t dmaChannel;
	uint8_t dmaSource;
#endif
} Flexcan_Session_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
int flexcan_edma_test(void);
/*${prototype:end}*/

static inline void FLEXCAN_ShowPinCfg()
{
	PRINTF("\r\nFor i.MX943EVK, Pin is:\r\n");
	PRINTF(" CAN2        connect to          CAN4 \r\n");
	PRINTF(" Pin Name   Board Location     Pin Name    Board Location \r\n");
	PRINTF(" CAN_H        J18-2                CAN_H       J19-2 \r\n");
	PRINTF(" CAN_L        J18-3                CAN_L       J19-3 \r\n");
}

#endif
