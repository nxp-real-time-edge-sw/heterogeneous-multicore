/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_cache.h"
#include "fsl_clock.h"
#include "fsl_iomuxc.h"

/*******************************************************************************
 * Code
 ******************************************************************************/
/* Get UART Clock Id. */
clock_ip_name_t BOARD_GetUartClkId(uint32_t uartInstIdx)
{
    clock_ip_name_t uart_clk_id[] = LPUART_CLOCKS;

    return uart_clk_id[uartInstIdx];
}

/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
    /* clang-format off */
    clk_t clk = {
        .clkId = kCLOCK_IpInvalid,
        .clkRoundOpt = SCMI_CLOCK_ROUND_AUTO,
        .rate = 24000000UL,
    };
    /* clang-format on */

    if (BOARD_DEBUG_CONSOLE_TYPE == BOARD_DEBUG_UART_TYPE)
    {
        clk.clkId = BOARD_GetUartClkId(BOARD_DEBUG_CONSOLE_PORT);
    }

    CLOCK_SetRate(&clk);
    CLOCK_EnableClock(clk.clkId);
    DbgConsole_Init(BOARD_DEBUG_CONSOLE_PORT, BOARD_DEBUG_CONSOLE_BAUDRATE, BOARD_DEBUG_CONSOLE_TYPE,
                    CLOCK_GetRate(clk.clkId));
}

/*
 * Change macro definitions as follows when choose cache policy as non-cacheable:
 * #define CACHE_MODE_NONCACHEABLE (1U)
 * #define CACHE_MODE_WRITE_THROUGH (0U)
 * #define CACHE_MODE_WRITE_BACK (0U)
 *
 *
 * Change macro definitions as follows when choose cache policy as Write-Through:
 * #define CACHE_MODE_NONCACHEABLE (0U)
 * #define CACHE_MODE_WRITE_THROUGH (1U)
 * #define CACHE_MODE_WRITE_BACK (0U)
 *
 *
 * Change macro definitions as follows when choose cache policy as Write-Back:
 * #define CACHE_MODE_NONCACHEABLE (0U)
 * #define CACHE_MODE_WRITE_THROUGH (0U)
 * #define CACHE_MODE_WRITE_BACK (1U)
 */
#ifndef CACHE_MODE_NONCACHEABLE
#define CACHE_MODE_NONCACHEABLE  (1U)
#endif
#ifndef CACHE_MODE_WRITE_THROUGH
#define CACHE_MODE_WRITE_THROUGH (0U)
#endif
#ifndef CACHE_MODE_WRITE_BACK
#define CACHE_MODE_WRITE_BACK    (0U)
#endif

void BOARD_ConfigMPU(void)
{
    uint8_t attr;

    /* Disable code cache(ICache) and system cache(DCache) */
    XCACHE_DisableCache(SOC_CORE_ICACHE_CTRL);
    XCACHE_DisableCache(SOC_CORE_DCACHE_CTRL);

    /* NOTE: All TCRAM is non-cacheable regardless of MPU setting. */

    /*
     * default cache policy(default memory access behavior) after enable mpu on cortex-m33(according to RM of
     * cortex-m33): 0x00000000-0x1FFFFFFF Normal memory, Non-shareable, Write-Through, not Write Allocate
     * 0x20000000-0x3FFFFFFF Normal memory, Non-shareable, Write-Back, Write Allocate
     * 0x40000000-0x5FFFFFFF Device, Shareable
     * 0x60000000-0x7FFFFFFF Normal memory, Non-shareable, Write-Back, Write Allocate
     * 0x80000000-0x9FFFFFFF Normal memory, Non-shareable, Write-Through, not Write Allocate
     * 0xA0000000-0xDFFFFFFF Device, Shareable
     * 0xE0000000-0xE003FFFF Device, Shareable
     * 0xE0040000-0xE0043FFF Device, Shareable
     * 0xE0044000-0xE00EFFFF Device, Shareable
     * 0xF0000000-0xFFFFFFFF Device, Shareable
     */
    /* Disable MPU */
    ARM_MPU_Disable();

    /* Attr0: Device-nGnRnE */
    ARM_MPU_SetMemAttr(0U, ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRnE));

    /* Attr1: Normal memory, Outer non-cacheable, Inner non-cacheable */
    ARM_MPU_SetMemAttr(1U, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));

    /* Attr2: Normal memory, Inner write-through transient, read allocate. Inner write-through transient, read allocate
     */
    attr = ARM_MPU_ATTR_MEMORY_(0U, 0U, 1U, 0U);
    ARM_MPU_SetMemAttr(2U, ARM_MPU_ATTR(attr, attr));

    /* Attr3: Normal memory, Outer write-back transient, read/write allocate. Inner write-back transient, read/write
     * allocate */
    attr = ARM_MPU_ATTR_MEMORY_(0U, 1U, 1U, 1U);
    ARM_MPU_SetMemAttr(3U, ARM_MPU_ATTR(attr, attr));

        /* Attr0: device. */
    ARM_MPU_SetMemAttr(4U, ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRnE));

#if CACHE_MODE_NONCACHEABLE
    /* NOTE: DDR is used as shared memory for A/M core communication, set it to non-cacheable. */
    /* Region 0: [0x80000000, 0xDFFFFFFF](DRAM), outer shareable, read/write, any privileged, executable. Attr 1
     * (non-cacheable). */
    ARM_MPU_SetRegion(0U, ARM_MPU_RBAR(0x80000000, ARM_MPU_SH_OUTER, 0U, 1U, 0U), ARM_MPU_RLAR(0xDFFFFFFF, 1U));
#elif CACHE_MODE_WRITE_THROUGH
    /* Region 0: [0x80000000, 0xDFFFFFFF](DRAM), outer shareable, read/write, any privileged, executable. Attr 2
     * (Normal memory, Inner write-through transient, read allocate. Inner write-through transient, read allocate). */
    ARM_MPU_SetRegion(0U, ARM_MPU_RBAR(0x80000000, ARM_MPU_SH_OUTER, 0U, 1U, 0U), ARM_MPU_RLAR(0xDFFFFFFF, 2U));
#elif CACHE_MODE_WRITE_BACK
    /* Region 0: [0x80000000, 0xDFFFFFFF](DRAM), outer shareable, read/write, any privileged, executable. Attr 3
     * (Normal memory, Outer write-back transient, read/write allocate. Inner write-back transient, read/write
     * allocate). */
    ARM_MPU_SetRegion(0U, ARM_MPU_RBAR(0x80000000, ARM_MPU_SH_OUTER, 0U, 1U, 0U), ARM_MPU_RLAR(0xDFFFFFFF, 3U));
#endif
    ARM_MPU_SetRegion(1U, ARM_MPU_RBAR(0x28000000, ARM_MPU_SH_OUTER, 0U, 1U, 0U), ARM_MPU_RLAR(0x3FFFFFFF, 4U));

    /* Enable MPU(The MPU is enabled during HardFault and NMI handlers; use default memory map when access the memory within region) */
    ARM_MPU_Enable(MPU_CTRL_HFNMIENA_Msk | MPU_CTRL_PRIVDEFENA_Msk);

    /* Enable ICache and DCache */
    XCACHE_EnableCache(SOC_CORE_ICACHE_CTRL);
    XCACHE_EnableCache(SOC_CORE_DCACHE_CTRL);
    /* flush pipeline */
    __DSB();
    __ISB();
}

void BOARD_InitLpuart8Pins(void) {                                /*!< Function assigned for the core: undefined[cm33] */
    IOMUXC_SetPinMux(IOMUXC_PAD_DAP_TCLK_SWCLK__LPUART8_RX, 0U);
    IOMUXC_SetPinMux(IOMUXC_PAD_DAP_TMS_SWDIO__LPUART8_TX, 0U);

    IOMUXC_SetPinConfig(IOMUXC_PAD_DAP_TCLK_SWCLK__LPUART8_RX,
                        IOMUXC_PAD_PD_MASK);
    IOMUXC_SetPinConfig(IOMUXC_PAD_DAP_TMS_SWDIO__LPUART8_TX,
                        IOMUXC_PAD_DSE(15U));
}

void BOARD_InitDebugConsolePins(void)
{
#if BOARD_DEBUG_UART_TYPE == kSerialPort_Uart
    BOARD_InitLpuart8Pins();
#else
#warn "Not initialize any pins for debug console with non uart port"
#endif
}
