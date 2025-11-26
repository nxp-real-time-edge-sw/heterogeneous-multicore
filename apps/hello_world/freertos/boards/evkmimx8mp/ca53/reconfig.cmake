set(hmc_os_board_path "${hmc_root_path}/os/freertos/Core_AArch64/boards/evkmimx8mp")

if (NOT DEFINED RTOS_ID)
    message("warning: RTOS_ID is not defined, set to 0 by default")
    set(RTOS_ID 0)
endif()

if (NOT DEFINED CONSOLE)
    message("warning: CONSOLE is not defined, set to UART4 by default")
    set(CONSOLE "UART4")
endif()

message("building RTOS${RTOS_ID} with ${CONSOLE} Console ...")

mcux_add_macro(
    CC "-DRTOSID=${RTOS_ID}"
)

if(${CONSOLE} STREQUAL "RAM_CONSOLE")
    mcux_remove_macro(
      CC "-DSDK_DEBUGCONSOLE=1"
    )
    mcux_add_macro(
      CC "-DSDK_DEBUGCONSOLE=2\
          -DCONFIG_RAM_CONSOLE"
    )
elseif(${CONSOLE} STREQUAL "UART2")
    mcux_add_macro(
      CC "-DSDK_DEBUGCONSOLE=1\
          -DBOARD_DEBUG_UART_INSTANCE=2"
    )
elseif(${CONSOLE} STREQUAL "UART4")
    mcux_add_macro(
      CC "-DSDK_DEBUGCONSOLE=1\
          -DBOARD_DEBUG_UART_INSTANCE=4"
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
            app_mmu.h
            clock_config.c
            clock_config.h
            hardware_init.c
            pin_mux.c
            pin_mux.h
)

mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES ${hmc_os_board_path}/board.c
            ${hmc_os_board_path}/mmu.c
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
set(DDR_LD_FILE "MIMX8ML8xxxxx_ca53_ddr_ram_RTOS${RTOS_ID}.ld")

mcux_remove_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}
    LINKER ${device_root}/i.MX/i.MX8MP/MIMX8ML8/gcc/MIMX8ML8xxxLZ_ddr_ram.ld
    TARGETS
      ddr_debug
      ddr_release
)

mcux_add_armgcc_linker_script(
    BASE_PATH ${APPLICATION_BINARY_DIR}
    LINKER ${DDR_LD_FILE}
    TARGETS
      ddr_debug
      ddr_release
)

mcux_add_custom_command(
    TARGETS ddr_release ddr_debug
    TOOLCHAINS armgcc
    BUILD_EVENT PRE_BUILD
    BUILD_COMMAND ${CMAKE_C_COMPILER} -E -P -x assembler-with-cpp -I${SdkRootDirPath}/${hmc_os_board_path} -DRTOSID=${RTOS_ID} -o ${DDR_LD_FILE}  ${SdkRootDirPath}/${hmc_os_board_path}/armgcc_aarch64/MIMX8ML8xxxxx_ca53_ddr_ram.ld.S
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
