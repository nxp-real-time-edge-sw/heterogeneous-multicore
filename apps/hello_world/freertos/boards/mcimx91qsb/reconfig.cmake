set(hmc_os_board_path "${hmc_root_path}/os/freertos/Core_AArch64/boards/${board}")

if (NOT DEFINED RTOS_ID)
    set(RTOS_ID 0)
elseif(RTOS_ID NOT STREQUAL "0")
    message(FATAL_ERROR "Don't set RTOS_ID or set it to 0 as only single Core available")
endif()

if (NOT DEFINED CONSOLE)
    message("warning: CONSOLE is not defined, set to UART1 by default")
    set(CONSOLE "UART1")
endif()

message("building RTOS${RTOS_ID} with ${CONSOLE} Console ...")

mcux_add_macro(
    CC "-DRTOSID=${RTOS_ID}"
)

if(${CONSOLE} STREQUAL "UART1")
    mcux_add_macro(
      CC "-DSDK_DEBUGCONSOLE=1\
          -DBOARD_DEBUG_UART_INSTANCE=1"
    )
else()
    message(FATAL_ERROR "Unsuported Console ${CONSOLE} ...")
endif()

mcux_add_macro(
    CC "-DFSL_RTOS_FREE_RTOS\
        -DPRINTF_ADVANCED_ENABLE=1"
)

mcux_add_source(
    SOURCES app.h
            app_board.h
            clock_config.c
            clock_config.h
            hardware_init.c
            pin_mux.c
            pin_mux.h
)

mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES ${hmc_os_board_path}/mmu.c
            ${hmc_os_board_path}/board.c
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}
    INCLUDES ${hmc_root_path}/include
             ${hmc_root_path}/os
             ${hmc_root_path}/os/freertos/Core_AArch64/
             ${hmc_os_board_path}
)

include(${SdkRootDirPath}/${hmc_root_path}/os/freertos/Core_AArch64/common_freertos.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/os/freertos/Core_AArch64/core/armv8a/common_freertos_core_armv8a.cmake)

# linker script created by using memory defined in rtos_memory.h
set(DDR_LD_FILE "MIMX9111xxxxx_ca55_ddr_ram.ld")

mcux_add_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}/${hmc_os_board_path}/armgcc_aarch64
    LINKER ${DDR_LD_FILE}
    TARGETS
      ddr_debug
      ddr_release
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
