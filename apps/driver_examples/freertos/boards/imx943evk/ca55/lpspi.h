/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _LPSPI_APP_H_
#define _LPSPI_APP_H_
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*${macro:start}*/
/* Master related */

#define ENABLE_LPSPI_EDMA

#define LPSPI_MASTER_BASEADDR (LPSPI8)
#define LPSPI_MASTER_IRQN     (LPSPI8_IRQn)

#ifdef ENABLE_LPSPI_EDMA
#define LPSPI_MASTER_DMA_BASE         (EDMA4_BASE)
#define LPSPI_MASTER_DMA_RX_CHANNEL   1U
#define LPSPI_MASTER_DMA_TX_CHANNEL   2U
#define LPSPI_MASTER_DMA_RX_REQ       kDma4RequestMuxLpspi8Rx
#define LPSPI_MASTER_DMA_TX_REQ       kDma4RequestMuxLpspi8Tx
#define LPSPI_MASTER_DMA_BASEADDR     ((DMA_Type *)(LPSPI_MASTER_DMA_BASE))
#endif

#define LPSPI_MASTER_PCS_FOR_INIT     (kLPSPI_Pcs0)
#define LPSPI_MASTER_PCS_FOR_TRANSFER (kLPSPI_MasterPcs0)

#define LPSPI_MASTER_CLK_ID (kCLOCK_Lpspi8)

/* Slave related */
#define LPSPI_SLAVE_BASEADDR (LPSPI3)
#define LPSPI_SLAVE_IRQN     (LPSPI3_IRQn)

#ifdef ENABLE_LPSPI_EDMA
#define LPSPI_SLAVE_DMA_BASE          (EDMA2_BASE)
#define LPSPI_SLAVE_DMA_RX_CHANNEL    3U
#define LPSPI_SLAVE_DMA_TX_CHANNEL    4U
#define LPSPI_SLAVE_DMA_RX_REQ        kDma2RequestMuxLpspi3Rx
#define LPSPI_SLAVE_DMA_TX_REQ        kDma2RequestMuxLpspi3Tx
#define LPSPI_SLAVE_DMA_BASEADDR      ((DMA_Type *)(LPSPI_SLAVE_DMA_BASE))
#endif

#define LPSPI_SLAVE_PCS_FOR_INIT     (kLPSPI_Pcs0)
#define LPSPI_SLAVE_PCS_FOR_TRANSFER (kLPSPI_SlavePcs0)

#define LPSPI_SLAVE_CLK_ID (kCLOCK_Lpspi3)

#define LPSPI_MASTER_CLOCK_FREQ CLOCK_GetRate(LPSPI_MASTER_CLK_ID)
#define LPSPI_SLAVE_CLOCK_FREQ  CLOCK_GetRate(LPSPI_SLAVE_CLK_ID)

/*${macro:end}*/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
int lpspi_edma_test(void);
/*${prototype:end}*/

static inline void LPSPI_ShowPinCfg()
{
	PRINTF("\r\nFor i.MX943EVK, Pin is:\r\n");
	PRINTF(" Peripheral(LPSPI3)      connect to    Controller(LPSPI8) \r\n");
	PRINTF(" Pin Name   Board Location     Pin Name    Board Location \r\n");
	PRINTF(" SOUT       J47-8                SIN       J43-10 \r\n");
	PRINTF(" SIN        J47-10               SOUT      J43-8 \r\n");
	PRINTF(" SCK        J47-12               SCK       J43-12 \r\n");
	PRINTF(" PCS0       J47-6                PCS0      J43-6 \r\n");
}

#endif
