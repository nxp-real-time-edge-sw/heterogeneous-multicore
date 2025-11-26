/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#if __has_include("app_board.h")
#include "app_board.h"
#endif
#include "fsl_clock.h"
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
#include "fsl_lpi2c.h"
#endif
#if defined(BOARD_USE_ADP5585) && BOARD_USE_ADP5585
#include "fsl_adp5585.h"
#endif
#if defined(BOARD_USE_PCAL6524) && BOARD_USE_PCAL6524
#include "fsl_pcal6524.h"
#endif
#if defined(BOARD_USE_PCAL6408) && BOARD_USE_PCAL6408
#include "fsl_pcal6408.h"
#endif
#if defined(BOARD_USE_PCA6416A) && BOARD_USE_PCA6416A
#include "fsl_pca6416a.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME        "IMX95LP4X-EVK"
#define MANUFACTURER_NAME "NXP"
#define BOARD_DOMAIN_ID   (0U)

/* The UART to use for debug messages. */
#define BOARD_DEBUG_UART_TYPE     kSerialPort_Uart
#define BOARD_DEBUG_UART_BAUDRATE (115200U)

#if !defined(CONFIG_RAM_CONSOLE) && !defined(BOARD_DEBUG_UART_INSTANCE)
#define BOARD_DEBUG_UART_INSTANCE (3U) /* Use LPUART3 if not specified */
#endif

#if (BOARD_DEBUG_UART_INSTANCE == 1) /* LPUART1 which is used for uboot and Linux by default */
#define BOARD_DEBUG_UART_BASEADDR LPUART1_BASE
#define BOARD_DEBUG_UART_CLK_FREQ HAL_ClockGetRate(hal_clock_lpuart1)
#define BOARD_UART_IRQ            LPUART1_IRQn
#define BOARD_UART_IRQ_HANDLER    LPUART1_IRQHandler
#elif (BOARD_DEBUG_UART_INSTANCE == 3) /* LPUART3 which is used for M7 by default*/
#define BOARD_DEBUG_UART_BASEADDR LPUART3_BASE
#define BOARD_DEBUG_UART_CLK_FREQ HAL_ClockGetRate(hal_clock_lpuart3)
#define BOARD_UART_IRQ            LPUART3_IRQn
#define BOARD_UART_IRQ_HANDLER    LPUART3_IRQHandler
#endif

/* ADP5585 */
#define BOARD_ADP5585_I2C            LPI2C2
#define BOARD_ADP5585_I2C_ADDR       (0x34U)
#define BOARD_ADP5585_I2C_CLOCK_ROOT hal_clock_lpi2c2
#define BOARD_ADP5585_I2C_CLOCK_FREQ HAL_ClockGetRate(BOARD_ADP5585_I2C_CLOCK_ROOT)
/* hal api and hal arguments */
#define BOARD_ADP5585_I2C_CLOCK_ROOT_HAL hal_clock_lpi2c2
#define BOARD_ADP5585_I2C_CLOCK_FREQ_HAL HAL_ClockGetRate(BOARD_ADP5585_I2C_CLOCK_ROOT_HAL)

#define BOARD_ADP5585_PDM_MQS_SEL (2U)
#define BOARD_ADP5585_EXP_SEL     (4U)
#define BOARD_ADP5585_CAN_STBY    (8U)

/* PCAL6524 */
#define BOARD_PCAL6524_I2C            LPI2C2
#define BOARD_PCAL6524_I2C_ADDR       (0x22U)
#define BOARD_PCAL6524_I2C_CLOCK_ROOT hal_clock_lpi2c2
#define BOARD_PCAL6524_I2C_CLOCK_FREQ HAL_ClockGetRate(BOARD_PCAL6524_I2C_CLOCK_ROOT)

#define BOARD_PCAL6524_EXP_SEL        (0x0U)
#define BOARD_PCAL6524_ENET1_RST_B    (0x4U)
#define BOARD_PCAL6524_ENET2_RST_B    (0x5U)
#define BOARD_PCAL6524_CAN_STBY       (0x8U + 0x6U)
#define BOARD_PCAL6524_MIPI_CSI_SEL   (0x10U + 0x0U)
#define BOARD_PCAL6524_MIPI_CSI_RST   (0x10U + 0x1U)
#define BOARD_PCAL6524_MIPI_CSI_EN    (0x10U + 0x2U)
#define BOARD_PCAL6524_PDM_MQS_SEL    (0x10U + 0x4U)

/* PCAL6408 */
#define BOARD_PCAL6408_I2C1            LPI2C1
#define BOARD_PCAL6408_I2C1_ADDR       (0x20U)
#define BOARD_PCAL6408_I2C1_CLOCK_ROOT hal_clock_lpi2c1
#define BOARD_PCAL6408_I2C1_CLOCK_FREQ HAL_ClockGetRate(BOARD_PCAL6408_I2C1_CLOCK_ROOT)

#define BOARD_PCAL6408_I2C3            LPI2C3
#define BOARD_PCAL6408_I2C3_ADDR       (0x20U)
#define BOARD_PCAL6408_I2C3_CLOCK_ROOT hal_clock_lpi2c3
#define BOARD_PCAL6408_I2C3_CLOCK_FREQ HAL_ClockGetRate(BOARD_PCAL6408_I2C3_CLOCK_ROOT)

/* output for BOARD_PCAL6408_I2C3 */
#define BOARD_PCAL6408_CSI1_SYNC         (0U)
#define BOARD_PCAL6408_CSI1_PWDN         (1U)
#define BOARD_PCAL6408_CSI1_RST_B        (2U)
#define BOARD_PCAL6408_ETH_CLK_EN        (3U)

#define BOARD_PCAL6408_I2C4            LPI2C4
#define BOARD_PCAL6408_I2C4_ADDR       (0x21U)
#define BOARD_PCAL6408_I2C4_CLOCK_ROOT hal_clock_lpi2c4
#define BOARD_PCAL6408_I2C4_CLOCK_FREQ HAL_ClockGetRate(BOARD_PCAL6408_I2C4_CLOCK_ROOT)

/* output for BOARD_PCAL6408_I2C4 */
#define BOARD_PCAL6408_SLOT_SAI3_SEL     (7U)

#define BOARD_PCAL6408_I2C5            LPI2C5
#define BOARD_PCAL6408_I2C5_ADDR       (0x21U)
#define BOARD_PCAL6408_I2C5_CLOCK_ROOT hal_clock_lpi2c5
#define BOARD_PCAL6408_I2C5_CLOCK_FREQ HAL_ClockGetRate(BOARD_PCAL6408_I2C5_CLOCK_ROOT)

/* output for BOARD_PCAL6408_I2C5 */
#define BOARD_PCAL6408_AQR_PWR_EN        (0U)
#define BOARD_PCAL6408_AQR113C_RST_B_3V3 (1U)
#define BOARD_PCAL6408_ENET1_RST_B       (2U)
#define BOARD_PCAL6408_ETH_CLK_EN        (3U)

/* PCA6416A (U27) */
#define BOARD_PCA6416A_I2C            LPI2C6
#define BOARD_PCA6416A_I2C_ADDR       (0x21U)
#define BOARD_PCA6416A_I2C_CLOCK_ROOT hal_clock_lpi2c6
#define BOARD_PCA6416A_I2C_CLOCK_FREQ HAL_ClockGetRate(BOARD_PCA6416A_I2C_CLOCK_ROOT)

#define BOARD_PCA6416A_CAN1_STBY_B     5U
#define BOARD_PCA6416A_CAN1_EN         6U
#define BOARD_PCA6416A_CAN1_WAKE       7U

#define BOARD_PCA6416A_PDM_CAN_SEL     (8U + 2U)

/* hal api and hal arguments */
#define BOARD_PCAL6524_I2C_CLOCK_ROOT_HAL hal_clock_lpi2c2
#define BOARD_PCAL6524_I2C_CLOCK_FREQ_HAL HAL_ClockGetRate(BOARD_PCAL6524_I2C_CLOCK_ROOT_HAL)
#define BOARD_PCAL6524_I2C_CLOCK_GATE kCLOCK_Lpi2c2

#define BOARD_PCAL6524_EXT1_PWREN  (8U + 5U)
#define BOARD_PCAL6524_ENET1_NRST  (8U + 7U)
#define BOARD_PCAL6524_ENET2_NRST  (16U + 0U)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/

void BOARD_InitClock(void);
void BOARD_InitDebugConsole(void);
void BOARD_InitMemory(void);
void BOARD_RdcInit(void);
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
void BOARD_LPI2C_Init(LPI2C_Type *base, uint32_t clkSrc_Hz);
status_t BOARD_LPI2C_Send(LPI2C_Type *base,
                          uint8_t deviceAddress,
                          uint32_t subAddress,
                          uint8_t subaddressSize,
                          uint8_t *txBuff,
                          uint8_t txBuffSize,
                          uint32_t flags);
status_t BOARD_LPI2C_Receive(LPI2C_Type *base,
                             uint8_t deviceAddress,
                             uint32_t subAddress,
                             uint8_t subaddressSize,
                             uint8_t *rxBuff,
                             uint8_t rxBuffSize,
                             uint32_t flags);
status_t BOARD_Display_I2C_Send(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize);
status_t BOARD_Display_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);
#if defined(BOARD_USE_ADP5585) && BOARD_USE_ADP5585
void BOARD_ADP5585_I2C_Init(void);
status_t BOARD_ADP5585_I2C_Send(uint8_t deviceAddress,
                                uint32_t subAddress,
                                uint8_t subAddressSize,
                                const uint8_t *txBuff,
                                uint8_t txBuffSize,
                                uint32_t flags);
status_t BOARD_ADP5585_I2C_Receive(uint8_t deviceAddress,
                                   uint32_t subAddress,
                                   uint8_t subAddressSize,
                                   uint8_t *rxBuff,
                                   uint8_t rxBuffSize,
                                   uint32_t flags);

void BOARD_InitADP5585(adp5585_handle_t *handle);
#endif /* BOARD_USE_ADP5585 */

#if defined(BOARD_USE_PCAL6524) && BOARD_USE_PCAL6524
void BOARD_PCAL6524_I2C_Init(void);
status_t BOARD_PCAL6524_I2C_Send(uint8_t deviceAddress,
                                 uint32_t subAddress,
                                 uint8_t subAddressSize,
                                 const uint8_t *txBuff,
                                 uint8_t txBuffSize,
                                 uint32_t flags);
status_t BOARD_PCAL6524_I2C_Receive(uint8_t deviceAddress,
                                    uint32_t subAddress,
                                    uint8_t subAddressSize,
                                    uint8_t *rxBuff,
                                    uint8_t rxBuffSize,
                                    uint32_t flags);

void BOARD_InitPCAL6524(pcal6524_handle_t *handle);
#endif /* BOARD_USE_PCAL6524 */

#if defined(BOARD_USE_PCAL6408) && BOARD_USE_PCAL6408
void BOARD_PCAL6408_I2C_Init(void *base, uint32_t clkSrc_Hz);
status_t BOARD_PCAL6408_I2C_Send(void *base,
                                 uint8_t deviceAddress,
                                 uint32_t subAddress,
                                 uint8_t subAddressSize,
                                 const uint8_t *txBuff,
                                 uint8_t txBuffSize,
                                 uint32_t flags);
status_t BOARD_PCAL6408_I2C_Receive(void *base,
                                    uint8_t deviceAddress,
                                    uint32_t subAddress,
                                    uint8_t subAddressSize,
                                    uint8_t *rxBuff,
                                    uint8_t rxBuffSize,
                                    uint32_t flags);
void BOARD_InitPCAL6408_I2C1(pcal6408_handle_t *handle);
void BOARD_InitPCAL6408_I2C3(pcal6408_handle_t *handle);
void BOARD_InitPCAL6408_I2C4(pcal6408_handle_t *handle);
void BOARD_InitPCAL6408_I2C5(pcal6408_handle_t *handle);
#endif /* BOARD_USE_PCAL6408 */

#if defined(BOARD_USE_PCA6416A) && BOARD_USE_PCA6416A
void BOARD_PCA6416A_I2C_Init(void);
status_t BOARD_PCA6416A_I2C_Send(void *base,
                                 uint8_t deviceAddress,
                                 uint32_t subAddress,
                                 uint8_t subAddressSize,
                                 const uint8_t *txBuff,
                                 uint8_t txBuffSize,
                                 uint32_t flags);
status_t BOARD_PCA6416A_I2C_Receive(void *base,
                                    uint8_t deviceAddress,
                                    uint32_t subAddress,
                                    uint8_t subAddressSize,
                                    uint8_t *rxBuff,
                                    uint8_t rxBuffSize,
                                    uint32_t flags);
void BOARD_InitPCA6416A(pca6416a_handle_t *handle);
#endif /* BOARD_USE_PCA6416A */

#endif /* SDK_I2C_BASED_COMPONENT_USED */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
