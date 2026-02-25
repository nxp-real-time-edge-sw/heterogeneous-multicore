/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "app.h"
#include "os/stdio.h"
#include "os/irq.h"
#include "FreeRTOS.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "rtos_memory.h"
#include "sm_platform.h"

static bool shw_init_flag;

extern void LPSPI3_DriverIRQHandler(void);
extern void LPSPI8_DriverIRQHandler(void);
extern void CAN2_DriverIRQHandler(void);
extern void CAN4_DriverIRQHandler(void);
extern void LPI2C4_DriverIRQHandler(void);
extern void LPI2C6_DriverIRQHandler(void);

typedef void (*irq_func_t)(void *data);
static int irq_register_enable(uint32_t irq, irq_func_t func, void *data, uint32_t prio)
{
	int ret = 0;

	ret = os_irq_register(irq, func, data, prio);
	if (ret == 0)
		os_irq_enable(irq);

	return ret;
}

#ifdef LPSPI_TEST
void LPSPI3_IRQHandler(void *p)
{
	return LPSPI3_DriverIRQHandler();
}

void LPSPI8_IRQHandler(void *p)
{
	return LPSPI8_DriverIRQHandler();
}
#endif

#ifdef IRQ_STEER_EDMA
#ifdef IRQ_STEER_DBG
void IRQSTEER_Enable_All_IRQ(void)
{
	int i = 0;

	for (i = 0; i < INT_MAX_NUM; i++)
		IRQSTEER_EnableInterrupt(IRQSTEERA55_INST, (IRQn_Type)(IRQ_BEG_IDX + i));
}
#endif

void IRQSTEER_EDMA_Callback(uint8_t chan_id, uint8_t edma_id, uint8_t edma_ch)
{
	PRINTF("%s() is called! chan:%d edma_id:%d edma_ch:%d\r\n", __func__, chan_id, edma_id, edma_ch);
	EDMA_DriverIRQHandler(edma_id, edma_ch);
}

#define IRQ_CH_NUM (6)
static uint8_t s_irq_para[IRQ_CH_NUM] = {0, 1, 2, 3, 4, 5};
void IRQSTEER_IRQ_Handler(void *p)
{
	uint8_t ch = *(uint8_t *)p;

	PRINTF("%s() for CH%d is called!\r\n", __func__, ch);
	return IRQSTEER_Driver_IRQ_Handler(ch);
}
#endif  //IRQ_STEER_EDMA

#ifdef FLEXCAN_TEST
void CAN2_IRQHandler(void *p)
{
	return CAN2_DriverIRQHandler();
}

void CAN4_IRQHandler(void *p)
{
	return CAN4_DriverIRQHandler();
}
#endif

#ifdef LPI2C_TEST
void LPI2C4_IRQHandler(void *p)
{
	return LPI2C4_DriverIRQHandler();
}

void LPI2C6_IRQHandler(void *p)
{
	return LPI2C6_DriverIRQHandler();
}
#endif

void BOARD_InitHardware(void)
{
	uint32_t prio = portLOWEST_USABLE_INTERRUPT_PRIORITY - 1;

	if (shw_init_flag)
		return;
	shw_init_flag = true;

	/* Init board cpu and hardware. */
	BOARD_InitMemory();
	/* Enable GIC before register any interrupt handler*/
	GIC_Enable();

	SM_Platform_Init();
	board_pins_setup();
	board_clock_setup();
	BOARD_InitDebugConsole();

#ifdef LPSPI_TEST
	irq_register_enable(LPSPI3_IRQn, LPSPI3_IRQHandler, NULL, prio);
	irq_register_enable(LPSPI8_IRQn, LPSPI8_IRQHandler, NULL, prio);

	/* Route LPSPI8 of SoC to arduino interface */
	BOARD_EXPANDER_SetPinAsOutput(BOARD_PCA6416_I2C6_S3_ID, SPI8_SEL1);
	BOARD_EXPANDER_SetPinToLow(BOARD_PCA6416_I2C6_S3_ID, SPI8_SEL1);
	BOARD_EXPANDER_SetPinAsOutput(BOARD_PCA6416_I2C6_S3_ID, SPI8_SEL3);
	BOARD_EXPANDER_SetPinToLow(BOARD_PCA6416_I2C6_S3_ID, SPI8_SEL3);
#endif

#ifdef FLEXCAN_TEST
	irq_register_enable(CAN2_IRQn, CAN2_IRQHandler, NULL, prio);
	irq_register_enable(CAN4_IRQn, CAN4_IRQHandler, NULL, prio);

	BOARD_EXPANDER_SetPinAsOutput(BOARD_PCA6416_I2C6_S1_ID, CAN2_ENABLE);
	BOARD_EXPANDER_SetPinAsOutput(BOARD_PCA6416_I2C6_S1_ID, CAN2_STBY_B);
	BOARD_EXPANDER_SetPinAsOutput(BOARD_PCA6416_I2C6_S1_ID, CAN4_ENABLE);
	BOARD_EXPANDER_SetPinAsOutput(BOARD_PCA6416_I2C6_S1_ID, CAN4_STBY_B);

	/* Initialize TJA1463AT with normal mode */
	/* Clear CAN STBY_N and EN signal of TJA1463AT */
	BOARD_EXPANDER_SetPinToLow(BOARD_PCA6416_I2C6_S1_ID, CAN2_ENABLE);
	BOARD_EXPANDER_SetPinToLow(BOARD_PCA6416_I2C6_S1_ID, CAN2_STBY_B);
	BOARD_EXPANDER_SetPinToLow(BOARD_PCA6416_I2C6_S1_ID, CAN4_ENABLE);
	BOARD_EXPANDER_SetPinToLow(BOARD_PCA6416_I2C6_S1_ID, CAN4_STBY_B);
	SDK_DelayAtLeastUs(100U, SystemCoreClock);

	/* Setup CAN EN and STBY_N signal of TJA1436AT */
	BOARD_EXPANDER_SetPinToHigh(BOARD_PCA6416_I2C6_S1_ID, CAN2_ENABLE);
	BOARD_EXPANDER_SetPinToHigh(BOARD_PCA6416_I2C6_S1_ID, CAN2_STBY_B);
	BOARD_EXPANDER_SetPinToHigh(BOARD_PCA6416_I2C6_S1_ID, CAN4_ENABLE);
	BOARD_EXPANDER_SetPinToHigh(BOARD_PCA6416_I2C6_S1_ID, CAN4_STBY_B);
#endif

#ifdef LPI2C_TEST
	irq_register_enable(LPI2C4_IRQn, LPI2C4_IRQHandler, NULL, prio);
	irq_register_enable(LPI2C6_IRQn, LPI2C6_IRQHandler, NULL, prio);
#endif

#ifdef IRQ_STEER_EDMA
	for (int i = 0; i < IRQ_CH_NUM; i++)
		irq_register_enable(IRQSTEERM_CH0_IRQn + i, IRQSTEER_IRQ_Handler, s_irq_para + i, prio);

	IRQSTEER_Register_Callback(CH0, 3, EDMA2_INSTANCE, 3, IRQSTEER_EDMA_Callback);
	IRQSTEER_Register_Callback(CH2, 129, EDMA4_INSTANCE, kDma4RequestMuxCan2, IRQSTEER_EDMA_Callback);
	IRQSTEER_Register_Callback(CH3, 200, EDMA4_INSTANCE, kDma4RequestMuxCan4, IRQSTEER_EDMA_Callback);
#endif
}

uint64_t get_core_mpid(void)
{
	return (__get_MPIDR_EL1() & MPIDR_AFFINITY_MASK);
}
