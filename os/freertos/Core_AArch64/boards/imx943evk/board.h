/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#if __has_include("app_board.h")
#include "app_board.h"
#endif
#include "fsl_clock.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME        "MIMX943-EVK"
#define MANUFACTURER_NAME "NXP"
#define BOARD_DOMAIN_ID   (0U)

/* The UART to use for debug messages. */
#define BOARD_DEBUG_UART_TYPE     kSerialPort_Uart
#define BOARD_DEBUG_UART_BAUDRATE (115200U)

#if !defined(CONFIG_RAM_CONSOLE) && !defined(BOARD_DEBUG_UART_INSTANCE)
#define BOARD_DEBUG_UART_INSTANCE (8U) /* Use LPUART8 if not specified */
#endif

#if (BOARD_DEBUG_UART_INSTANCE == 1) /* LPUART1 which is used for uboot and Linux by default */
#define BOARD_DEBUG_UART_BASEADDR LPUART1_BASE
#define BOARD_DEBUG_UART_CLK_FREQ CLOCK_GetRate(kCLOCK_Lpuart1)
#define BOARD_UART_IRQ            LPUART1_IRQn
#define BOARD_UART_IRQ_HANDLER    LPUART1_IRQHandler
#elif (BOARD_DEBUG_UART_INSTANCE == 8) /* LPUART8 which is used for M33-1 by default*/
#define BOARD_DEBUG_UART_BASEADDR LPUART8_BASE
#define BOARD_DEBUG_UART_CLK_FREQ CLOCK_GetRate(kCLOCK_Lpuart8)
#define BOARD_UART_IRQ            LPUART8_IRQn
#define BOARD_UART_IRQ_HANDLER    LPUART8_IRQHandler
#endif

#define BOARD_CODEC_I2C_BASEADDR LPI2C3
#define BOARD_CODEC_I2C_INSTANCE 3U
#define BOARD_CODEC_I2C_CLOCK_ROOT kCLOCK_Lpi2c3
#define BOARD_CODEC_I2C_CLOCK_FREQ CLOCK_GetRate(BOARD_CODEC_I2C_CLOCK_ROOT)

/* PCA6408 */
#define BOARD_PCAL6408_I2C1            LPI2C1
#define BOARD_PCAL6408_I2C1_ADDR       (0x20U)
#define BOARD_PCAL6408_I2C1_CLOCK_ROOT kCLOCK_Lpi2c1
#define BOARD_PCAL6408_I2C1_CLOCK_FREQ CLOCK_GetRate(BOARD_PCAL6408_I2C1_CLOCK_ROOT)

#define BOARD_PCAL6408_I2C3            LPI2C3
#define BOARD_PCAL6408_I2C3_ADDR       (0x20U)
#define BOARD_PCAL6408_I2C3_CLOCK_ROOT kCLOCK_Lpi2c3
#define BOARD_PCAL6408_I2C3_CLOCK_FREQ CLOCK_GetRate(BOARD_PCAL6408_I2C3_CLOCK_ROOT)

#define BOARD_PCAL6408_I2C4            LPI2C4
#define BOARD_PCAL6408_I2C4_ADDR       (0x21U)
#define BOARD_PCAL6408_I2C4_CLOCK_ROOT kCLOCK_Lpi2c4
#define BOARD_PCAL6408_I2C4_CLOCK_FREQ CLOCK_GetRate(BOARD_PCAL6408_I2C4_CLOCK_ROOT)

/* output for BOARD_PCA6408_I2C4 */
#define BOARD_PCAL6408_SLOT_SAI3_SEL     (7U)

#define BOARD_PCAL6408_I2C5            LPI2C5
#define BOARD_PCAL6408_I2C5_ADDR       (0x21U)
#define BOARD_PCAL6408_I2C5_CLOCK_ROOT kCLOCK_Lpi2c5
#define BOARD_PCAL6408_I2C5_CLOCK_FREQ CLOCK_GetRate(BOARD_PCAL6408_I2C5_CLOCK_ROOT)

/* output for BOARD_PCA6408_I2C5 */
#define BOARD_PCAL6408_AQR_PWR_EN        (0U)
#define BOARD_PCAL6408_AQR113C_RST_B_3V3 (1U)
#define BOARD_PCAL6408_ENET1_RST_B       (2U)
#define BOARD_PCAL6408_ETH_CLK_EN        (3U)

/* PCA6416A (U157) */
#define BOARD_PCA6416A_I2C            LPI2C3
#define BOARD_PCA6416A_I2C_ADDR       (0x21U)
#define BOARD_PCA6416A_I2C_CLOCK_ROOT kCLOCK_Lpi2c3
#define BOARD_PCA6416A_I2C_CLOCK_FREQ CLOCK_GetRate(BOARD_PCA6416A_I2C_CLOCK_ROOT)

/* P0_0 of PCA6416APW */
#define BOARD_PCA6416A_CAN1_ERROR_B         0U
/* P0_1 of PCA6416APW */
#define BOARD_PCA6416A_CAN1_EN         1U
/* P0_2 of PCA6416APW */
#define BOARD_PCA6416A_CAN1_STBY_B     2U
/* P0_3 of PCA6416APW */
#define BOARD_PCA6416A_CAN2_ERROR_B         3U
/* P0_4 of PCA6416APW */
#define BOARD_PCA6416A_CAN2_EN         4U
/* P0_5 of PCA6416APW */
#define BOARD_PCA6416A_CAN2_STBY_B     5U
/* P0_6 of PCA6416APW */
#define BOARD_PCA6416A_CAN3_ERROR_B         6U
/* P0_7 of PCA6416APW */
#define BOARD_PCA6416A_CAN3_EN         7U
/* P1_0 of PCA6416APW */
#define BOARD_PCA6416A_CAN3_STBY_B     8U
/* P1_1 of PCA6416APW */
#define BOARD_PCA6416A_CAN4_ERROR_B         9U
/* P1_2 of PCA6416APW */
#define BOARD_PCA6416A_CAN4_EN         10U
/* P1_3 of PCA6416APW */
#define BOARD_PCA6416A_CAN4_STBY_B     11U
/* P1_4 of PCA6416APW */
#define BOARD_PCA6416A_CAN5_ERROR_B         12U
/* P1_5 of PCA6416APW */
#define BOARD_PCA6416A_CAN5_EN         13U
/* P1_6 of PCA6416APW */
#define BOARD_PCA6416A_CAN5_STBY_B     14U
/* P1_7 of PCA6416APW */
#define BOARD_PCA6416A_CAN_SW_WAKE     15U

#define BOARD_PCA9544_NUM 1
#define BOARD_PCA9548_NUM 1
#define BOARD_PCA9544_I2C6_HANDLE_IDX 0
#define BOARD_PCA9548_I2C3_HANDLE_IDX 1

#define BOARD_PCA6408_NUM 2
#define BOARD_PCA6408_I2C6_S2_HANDLE_IDX 0
#define BOARD_PCA6408_I2C3_S7_HANDLE_IDX 1

#define BOARD_PCA6416_NUM 5
#define BOARD_PCA6416_I2C6_S1_HANDLE_IDX 0
#define BOARD_PCA6416_I2C6_S3_HANDLE_IDX 1
#define BOARD_PCA6416_I2C3_S5_20_HANDLE_IDX 2
#define BOARD_PCA6416_I2C3_S5_21_HANDLE_IDX 3
#define BOARD_PCA6416_I2C3_S6_HANDLE_IDX 4

#define BOARD_PCA9544_I2C6_ADDR (0x77U)
#define BOARD_PCA9548_I2C3_ADDR (0x77U)
#define BOARD_PCA6416_I2C6_S1_ADDR (0x21)
#define BOARD_PCA6408_I2C6_S2_ADDR (0x20)
#define BOARD_PCA6416_I2C6_S3_ADDR (0x20)
#define BOARD_PCA6416_I2C3_S5_20_ADDR (0x20)
#define BOARD_PCA6416_I2C3_S5_21_ADDR (0x21)
#define BOARD_PCA6416_I2C3_S6_ADDR (0x20)
#define BOARD_PCA6408_I2C3_S7_ADDR (0x20)
#define BOARD_24LC32A_I2C7_ADDR (0x50)

#define I2C_INSTANCE_IDX_SHIFT 24U
#define I2C_INSTANCE_IDX_MASK (0xFU << I2C_INSTANCE_IDX_SHIFT)
#define I2C_DEVICE_ADDR_SHIFT 16U
#define I2C_DEVICE_ADDR_MASK (0xFFU << I2C_DEVICE_ADDR_SHIFT)
#define I2C_CHAN_IDX_SHIFT 8U
#define I2C_CHAN_IDX_MASK (0xFFU << I2C_CHAN_IDX_SHIFT)
#define I2C_DEVICE_TYPE_SHIFT 4U
#define I2C_DEVICE_TYPE_MASK (0xFU << I2C_DEVICE_TYPE_SHIFT)
#define I2C_DEVICE_ARRAY_IDX_SHIFT 0x0U
#define I2C_DEVICE_ARRAY_IDX_MASK (0xFU << I2C_DEVICE_ARRAY_IDX_SHIFT)

typedef struct
{
   uint32_t parent_id; /* It indicates whether there is parent device for the i2c device */
   uint32_t chan; /* The chan is valid when parent_id is not equal to BOARD_UNKNOWN_ID */
   void *handle;
   void *data;
} i2c_device_table_t;

/* i2c channel id */
enum i2c_chan_id {
    BOARD_S0_CHAN_IDX,
    BOARD_S1_CHAN_IDX,
    BOARD_S2_CHAN_IDX,
    BOARD_S3_CHAN_IDX,
    BOARD_S4_CHAN_IDX,
    BOARD_S5_CHAN_IDX,
    BOARD_S6_CHAN_IDX,
    BOARD_S7_CHAN_IDX,
    BOARD_S8_CHAN_IDX,
    BOARD_S9_CHAN_IDX,
    BOARD_S10_CHAN_IDX,
    BOARD_S11_CHAN_IDX,
    BOARD_S12_CHAN_IDX,
    BOARD_S13_CHAN_IDX,
    BOARD_S14_CHAN_IDX,
    BOARD_S15_CHAN_IDX,
    BOARD_UNKNWON_CHAN_IDX,
};

enum i2c_device_type {
    PCA9544_TYPE,
    PCA9548_TYPE,
    PCA954X_TYPE,
    PCA6408_TYPE = PCA954X_TYPE, /* io expander */
    PCA6416_TYPE, /* io expander */
    PCA64XX_TYPE,
    UNKNOWN_TYPE,
};

enum i2c_instance_idx {
    LPI2C0_INST_IDX,
    LPI2C1_INST_IDX,
    LPI2C2_INST_IDX,
    LPI2C3_INST_IDX,
    LPI2C4_INST_IDX,
    LPI2C5_INST_IDX,
    LPI2C6_INST_IDX,
    LPI2C7_INST_IDX,
    LPI2C8_INST_IDX,
};

enum expander_pin_idx {
    P0_0,
    P0 = P0_0,
    SLOT_PCIE2_PWREN = P0_0,
    CAN4_ENABLE = P0_0,
    ETHD_RST_B = P0_0,
    SD1_SEL = P0_0,
    USB1_12V_ACK = P0,
    CAN1_ERROR_B = P0,
    P0_1,
    P1 = P0_1,
    SLOT_PCIE2_RESET_B = P0_1,
    CAN4_STBY_B = P0_1,
    CAN4_ERROR_B = P0_1,
    CAN1_ENABLE = P0_1,
    SD2_SEL = P0_1,
    M2_PCIE1_ALERT_IO = P1,
    CAN2_ERROR_B = P1,
    P0_2,
    P2 = P0_2,
    M2_PWREN = P0_2,
    ETH2_INT_B = P0_2,
    ETH2_RST_B = P0_2,
    CAN1_STBY_B = P0_2,
    ETH2_SEL = P0_2,
    SLOT_PCIE2_PRSNT = P2,
    CAN5_ERROR_B = P2,
    P0_3,
    P3 = P0_3,
    M2_PCIE1_RST_B = P0_3,
    ETH3_RST_B = P0_3,
    ETH3_INT_B = P0_3,
    CAN2_ENABLE = P0_3,
    ETH3_SEL = P0_3,
    SLOT_PCIE2_INT_WAKE_B = P3,
    ETH0_INT_B = P3,
    P0_4,
    P4 = P0_4,
    M2_SD3_RST_B = P0_4,
    ETH4_RST_B = P0_4,
    ETH4_INT_B = P0_4,
    CAN2_STBY_B = P0_4,
    ETH4_SEL = P0_4,
    PTN5110_ALERT_B = P4,
    ETH1_INT_B = P4,
    P0_5,
    P5 = P0_5,
    M2_NDIS1_B = P0_5,
    SGMII5_RST_B = P0_5,
    SGMII5_INT = P0_5,
    CAN5_ENABLE = P0_5,
    SPI8_SEL1 = P0_5,
    USB1_WAT_FLG_B = P5,
    P0_6,
    P6 = P0_6,
    M2_NDIS2_B = P0_6,
    SGMII6_RST_B = P0_6,
    SGMII6_INT = P0_6,
    CAN5_STBY_B = P0_6,
    SPI8_SEL3 = P0_6,
    PTN5150_INT_B = P6,
    P0_7,
    P7 = P0_7,
    USB1_12V_EN_EXP = P0_7,
    ETH4CD_RESET_B = P0_7,
    LVDS_TS_INT_B = P0_7,
    CAN_SW_WAKE = P0_7,
    UART_M_FT_SEL = P0_7,
    USB2_FLT_B = P7,
    P1_0,
    LVDS_RESET_B = P1_0,
    ETH4CD_INT_B = P1_0,
    ETHD_REFCLK_EN = P1_0,
    ADC_SEL = P1_0,
    P1_1,
    LVDS_EN = P1_1,
    ETH4CD_WOL_INT_B = P1_1,
    ETHD_REFCLK_A0 = P1_1,
    GPIO20_SEL = P1_1,
    P1_2,
    M2_WL_WAKE_IN = P1_2,
    SPI8_SEL2 = P1_2,
    ETH4CD_CARD_DET_B = P1_2,
    ETHD_REFCLK_A1 = P1_2,
    GPIO23_SEL = P1_2,
    P1_3,
    M2_BT_WAKE_IN = P1_3,
    MQS_MIC_SEL = P1_3,
    ETH4CD_INH_B = P1_3,
    ECAT_MII_SEL = P1_3,
    SAI3_SEL = P1_3,
    P1_4,
    M2_USB_SEL = P1_4,
    AUD_PWREN = P1_4,
    ETH4CD_WAKE_INOUT = P1_4,
    CAN_PDM_SEL = P1_4,
    P1_5,
    SD_CARD_ONOFF = P1_5,
    AUD_INT_B = P1_5,
    USDHC3_SEL = P1_5,
    P1_6,
    SEMC_IO1 = P1_6,
    AUD_HP_JD = P1_6,
    CAN2_SEL = P1_6,
    P1_7,
    SEMC_IO2 = P1_7,
    MQS_J_D = P1_7,
    CAN5_SEL = P1_7,
};

/*
 *  i2c instance index                                                     i2c channel index                       i2c device array index
 *           ^                                                                     ^                                         ^
 *           |                                                                     |                                         |
 * ---------------------                                       ---------------------------------------               +-------+-------+
 * |                   |                                       |                                     |               |               |
 * --------------------+---------------------------------------+----------------------------------------------------------------------
 * | 27 | 26 | 25 | 24 | 23 | 22 | 21 | 20 | 19 | 18 | 17 | 16 | 15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 * --------------------+---------------------------------------+----------------------------------------------------------------------
 *                     |                                       |                                     |               |
 *                     -----------------------------------------                                     --------+--------
 *                                         |                                                                 | 
 *                                         v                                                                 v
 *                                 i2c device address                                              i2c device type
 */
enum i2c_device_id {
    BOARD_PCA9544_I2C6_ID = (LPI2C6_INST_IDX << I2C_INSTANCE_IDX_SHIFT) | (BOARD_PCA9544_I2C6_ADDR << I2C_DEVICE_ADDR_SHIFT) | (BOARD_UNKNWON_CHAN_IDX << I2C_CHAN_IDX_SHIFT) | (PCA9544_TYPE << I2C_DEVICE_TYPE_SHIFT) | 0,
    BOARD_PCA9548_I2C3_ID = (LPI2C3_INST_IDX << I2C_INSTANCE_IDX_SHIFT) | (BOARD_PCA9548_I2C3_ADDR << I2C_DEVICE_ADDR_SHIFT) | (BOARD_UNKNWON_CHAN_IDX << I2C_CHAN_IDX_SHIFT) | (PCA9548_TYPE << I2C_DEVICE_TYPE_SHIFT) | 1,
    BOARD_PCA6416_I2C6_S1_ID = (LPI2C6_INST_IDX << I2C_INSTANCE_IDX_SHIFT) | (BOARD_PCA6416_I2C6_S1_ADDR << I2C_DEVICE_ADDR_SHIFT) | (BOARD_S1_CHAN_IDX << I2C_CHAN_IDX_SHIFT) | (PCA6416_TYPE << I2C_DEVICE_TYPE_SHIFT) | 2,
    BOARD_PCA6408_I2C6_S2_ID = (LPI2C6_INST_IDX << I2C_INSTANCE_IDX_SHIFT) | (BOARD_PCA6408_I2C6_S2_ADDR << I2C_DEVICE_ADDR_SHIFT) | (BOARD_S2_CHAN_IDX << I2C_CHAN_IDX_SHIFT) | (PCA6408_TYPE << I2C_DEVICE_TYPE_SHIFT) | 3,
    BOARD_PCA6416_I2C6_S3_ID = (LPI2C6_INST_IDX << I2C_INSTANCE_IDX_SHIFT) | (BOARD_PCA6416_I2C6_S3_ADDR << I2C_DEVICE_ADDR_SHIFT) | (BOARD_S3_CHAN_IDX << I2C_CHAN_IDX_SHIFT) | (PCA6416_TYPE << I2C_DEVICE_TYPE_SHIFT) | 4,
    BOARD_PCA6416_I2C3_S5_20_ID = (LPI2C3_INST_IDX << I2C_INSTANCE_IDX_SHIFT) | (BOARD_PCA6416_I2C3_S5_20_ADDR << I2C_DEVICE_ADDR_SHIFT) | (BOARD_S5_CHAN_IDX << I2C_CHAN_IDX_SHIFT) | (PCA6416_TYPE << I2C_DEVICE_TYPE_SHIFT) | 5,
    BOARD_PCA6416_I2C3_S5_21_ID = (LPI2C3_INST_IDX << I2C_INSTANCE_IDX_SHIFT) | (BOARD_PCA6416_I2C3_S5_21_ADDR << I2C_DEVICE_ADDR_SHIFT) | (BOARD_S5_CHAN_IDX << I2C_CHAN_IDX_SHIFT) | (PCA6416_TYPE << I2C_DEVICE_TYPE_SHIFT) | 6,
    BOARD_PCA6416_I2C3_S6_ID = (LPI2C3_INST_IDX << I2C_INSTANCE_IDX_SHIFT) | (BOARD_PCA6416_I2C3_S6_ADDR << I2C_DEVICE_ADDR_SHIFT) | (BOARD_S6_CHAN_IDX << I2C_CHAN_IDX_SHIFT) | (PCA6416_TYPE << I2C_DEVICE_TYPE_SHIFT) | 7,
    BOARD_PCA6408_I2C3_S7_ID = (LPI2C3_INST_IDX << I2C_INSTANCE_IDX_SHIFT) | (BOARD_PCA6408_I2C3_S7_ADDR << I2C_DEVICE_ADDR_SHIFT) | (BOARD_S7_CHAN_IDX << I2C_CHAN_IDX_SHIFT) | (PCA6416_TYPE << I2C_DEVICE_TYPE_SHIFT) | 8,
    BOARD_UNKNOWN_ID      = (BOARD_UNKNWON_CHAN_IDX << I2C_CHAN_IDX_SHIFT) | (UNKNOWN_TYPE << I2C_DEVICE_TYPE_SHIFT) | 10,
};

typedef void (*i2c_device_init_func_t)( uint32_t i2cDevId,
                                            void *base,
                                            uint32_t clkFreq,
                                            uint8_t i2cAddr);
typedef status_t (*i2c_device_select_chan_func_t)( uint32_t i2cDevId,
                                                uint32_t chan);
typedef status_t (*i2c_device_deselect_mux_func_t)( uint32_t i2cDevId,
                                                uint32_t chan);
typedef status_t (*i2c_device_set_pin_to_high_func_t)(uint32_t i2cDevId,
                                                uint8_t pinIdx);
typedef status_t (*i2c_device_set_pin_to_low_func_t)( uint32_t i2cDevId,
                                                uint8_t pinIdx);
typedef status_t (*i2c_device_set_pin_as_input_func_t)( uint32_t i2cDevId,
                                                uint8_t pinIdx);
typedef status_t (*i2c_device_set_pin_as_output_func_t)( uint32_t i2cDevId,
                                                uint8_t pinIdx);

typedef struct {
    uint8_t id;
    i2c_device_init_func_t init;
    i2c_device_select_chan_func_t select;
    i2c_device_deselect_mux_func_t deselect;
} pca954x_t;

typedef struct {
    i2c_device_init_func_t init;
    i2c_device_set_pin_to_high_func_t set_pin_to_high;
    i2c_device_set_pin_to_low_func_t set_pin_to_low;
    i2c_device_set_pin_as_input_func_t set_pin_as_input;
    i2c_device_set_pin_as_output_func_t set_pin_as_output;
    i2c_device_select_chan_func_t select;
    i2c_device_deselect_mux_func_t deselect;
} pca64xx_t;


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

void BOARD_InitI2cDevice(uint32_t i2cDevId);
/*
 * How to use the api?
 * e.g.
 * BOARD_MUX_Select(BOARD_PCA9544_I2C6_ID, BOARD_S0_CHAN_IDX);
 */
status_t BOARD_MUX_Select(uint32_t i2cDevId, uint32_t chan);
/*
 * How to use the api?
 * e.g.
 * BOARD_MUX_Deselect(BOARD_PCA9544_I2C6_ID, BOARD_S0_CHAN_IDX);
 */
status_t BOARD_MUX_Deselect(uint32_t i2cDevId, uint32_t chan);

/*
 * How to use the api?
 * e.g.
 * BOARD_EXPANDER_SetPinToHigh(BOARD_PCA6416_I2C6_S1_ID, P0_0);
 */
status_t BOARD_EXPANDER_SetPinToHigh(uint32_t i2cDevId, uint8_t pinIdx);
/*
 * How to use the api?
 * e.g.
 * BOARD_EXPANDER_SetPinToLow(BOARD_PCA6416_I2C6_S1_ID, P0_0);
 */
status_t BOARD_EXPANDER_SetPinToLow(uint32_t i2cDevId, uint8_t pinIdx);
/*
 * How to use the api?
 * e.g.
 * BOARD_EXPANDER_SetPinAsInput(BOARD_PCA6416_I2C6_S1_ID, P0_0);
 */
status_t BOARD_EXPANDER_SetPinAsInput(uint32_t i2cDevId, uint8_t pinIdx);
/*
 * How to use the api?
 * e.g.
 * BOARD_EXPANDER_SetPinAsOutput(BOARD_PCA6416_I2C6_S1_ID, P0_0);
 */
status_t BOARD_EXPANDER_SetPinAsOutput(uint32_t i2cDevId, uint8_t pinIdx);
#if defined(BOARD_USE_PCA954X) && BOARD_USE_PCA954X

#endif /* BOARD_USE_PCA954X */

#if defined(BOARD_USE_PCA6408) && BOARD_USE_PCA6408

#endif /* BOARD_USE_PCA6408 */

#if defined(BOARD_USE_PCA6408) && BOARD_USE_PCA6408

#endif /* BOARD_USE_PCA6416 */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
