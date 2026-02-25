/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_BOARD_H_
#define _APP_BOARD_H_

#include "fsl_debug_console.h"
#include "hal_clock.h"
#include "clock_config.h"
#include "fsl_power.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define IRQ_STEER_EDMA
#undef IRQ_STEER_DBG

#define CPU_CORTEX_A55
#define CPU_CORE_NAME	"Cortex-A55"

#ifdef IRQ_STEER_EDMA
#include "fsl_irqsteer.h"
#include "fsl_irqsteer_soc.h"
#include "fsl_edma_soc.h"

#define INT_MAX_NUM (FSL_FEATURE_IRQSTEER_EXT_INT_MAX_NUM)  // 960
#define IRQ_BEG_IDX (FSL_FEATURE_IRQSTEER_IRQ_START_INDEX)  // 224

#define IRQSTEERM_CH0_IRQn IRQSTEERM33_CH0_IRQn
#define IRQSTEERM_CH1_IRQn IRQSTEERM33_CH1_IRQn
#define IRQSTEERM_CH2_IRQn IRQSTEERM33_CH2_IRQn
#define IRQSTEERM_CH3_IRQn IRQSTEERM33_CH3_IRQn
#define IRQSTEERM_CH4_IRQn IRQSTEERM33_CH4_IRQn
#define IRQSTEERM_CH5_IRQn IRQSTEERM33_CH5_IRQn

#define CH0 (IRQSTEERM_CH0_IRQn - IRQSTEERM_CH0_IRQn)
#define CH1 (IRQSTEERM_CH1_IRQn - IRQSTEERM_CH0_IRQn)
#define CH2 (IRQSTEERM_CH2_IRQn - IRQSTEERM_CH0_IRQn)
#define CH3 (IRQSTEERM_CH3_IRQn - IRQSTEERM_CH0_IRQn)
#define CH4 (IRQSTEERM_CH4_IRQn - IRQSTEERM_CH0_IRQn)
#define CH5 (IRQSTEERM_CH5_IRQn - IRQSTEERM_CH0_IRQn)

int IRQSTEER_Register_Callback(uint8_t ch, uint16_t irq, uint8_t edma_id, uint8_t edma_ch, void *pfunc);

extern void IRQSTEER_Driver_IRQ_Handler(uint8_t ch);
extern void EDMA_DriverIRQHandler(uint32_t instance, uint32_t channel);
#endif

#endif /* _APP_BOARD_H_ */
