/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _LPI2C_APP_H_
#define _LPI2C_APP_H_
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*${macro:start}*/
#define ENABLE_LPI2C_EDMA

#define LPI2C_MASTER_CLOCK_ROOT		(kCLOCK_Lpi2c4)
#define LPI2C_MASTER_CLOCK_FREQUENCY    CLOCK_GetRate(LPI2C_MASTER_CLOCK_ROOT)
#define LPI2C_MASTER_BASE		(LPI2C4)

#define LPI2C_SLAVE_CLOCK_ROOT		(kCLOCK_Lpi2c6)
#define LPI2C_SLAVE_CLOCK_FREQUENCY     CLOCK_GetRate(LPI2C_MASTER_CLOCK_ROOT)
#define LPI2C_SLAVE_BASE		(LPI2C6)

#ifdef ENABLE_LPI2C_EDMA
#define LPI2C_MASTER_DMA                (EDMA4)
#define LPI2C_MASTER_DMA_BASE           (DMA_Type *)(EDMA4_BASE)
#define LPI2C_MASTER_DMA_TX_CHANNEL     1U
#define LPI2C_MASTER_DMA_RX_CHANNEL     2U
#define LPI2C_MASTER_DMA_RX_REQ         kDma4RequestMuxLpi2c4Rx
#define LPI2C_MASTER_DMA_TX_REQ         kDma4RequestMuxLpi2c4Tx
#endif
/*${macro:end}*/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
int lpi2c_edma_test(void);
/*${prototype:end}*/

static inline void LPI2C_ShowPinCfg()
{
	PRINTF("\r\nFor i.MX943EVK, Pin is:\r\n");
	PRINTF(" SLAVE(LPI2C6)      connect to    MASTER(LPI2C4) \r\n");
	PRINTF(" Pin Name   Board Location     Pin Name    Board Location \r\n");
	PRINTF(" SDA        J13-3                SDA       J12-3 \r\n");
	PRINTF(" SCL        J13-5                SCL       J12-5 \r\n");
}

#endif
