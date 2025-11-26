set(hmc_os_board_path "${hmc_root_path}/os/freertos/Core_AArch64/boards/${board}")

mcux_add_macro(
  CC "-DAARCH64=1"
)

if (NOT DEFINED RTOS_ID)
    message("warning: RTOS_ID is not defined, set to 0 by default")
    set(RTOS_ID 0)
endif()

if (NOT DEFINED CONSOLE)
    message("warning: CONSOLE is not defined, set to UART2 by default")
    set(CONSOLE "UART2")
endif()

if (NOT DEFINED IS_MASTER)
    message("warning: IS_MASTER is not defined, set to master by default")
    set(IS_MASTER "master")
endif()

message("building RTOS${RTOS_ID} ${IS_MASTER} with ${CONSOLE} Console ...")

mcux_add_macro(
    CC "-DRTOSID=${RTOS_ID}"
)

if(${IS_MASTER} STREQUAL "master")
    mcux_add_macro(
        C "-DGEN_SW_MBOX_MASTER=1"
    )
elseif(NOT ${IS_MASTER} STREQUAL "remote")
	message(FATAL_ERROR "Unsuported IS_MASTER=${IS_MASTER} ...")
endif()

set(app_name ${IS_MASTER})

if(${CONSOLE} STREQUAL "UART2")
    mcux_add_macro(
      CC "-DSDK_DEBUGCONSOLE=1\
          -DBOARD_DEBUG_UART_INSTANCE=2"
    )
elseif(${CONSOLE} STREQUAL "UART1")
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
    SOURCES app_board.c
            app_board.h
            clock_config.c
            clock_config.h
            pin_mux.c
            pin_mux.h
            rpmsg_config.h
            rpmsg_${app_name}/app_rpmsg.h
            rpmsg_${app_name}/gen_sw_mbox_config.h
)

mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES ${hmc_os_board_path}/board.c
            ${hmc_os_board_path}/mmu.c
            ${hmc_app_path}/main_${app_name}.c
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}
    INCLUDES ${hmc_root_path}/include
             ${hmc_root_path}/os
             ${hmc_root_path}/os/freertos/Core_AArch64/
             ${hmc_os_board_path}
             ${hmc_app_board_core_path}/rpmsg_${app_name}
)

include(${SdkRootDirPath}/${hmc_root_path}/os/freertos/Core_AArch64/common_freertos.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/os/freertos/Core_AArch64/core/armv8a/common_freertos_core_armv8a.cmake)

include(${SdkRootDirPath}/${hmc_libs_path}/log/lib_log.cmake)
include(${SdkRootDirPath}/${hmc_libs_path}/rpmsg/lib_rpmsg.cmake)
include(${SdkRootDirPath}/${hmc_libs_path}/gen_sw_mbox/lib_gen_sw_mbox.cmake)

# linker script created by using memory defined in rtos_memory.h
set(DDR_LD_FILE "MIMX9352xxxxx_ca55_ddr_ram_RTOS${RTOS_ID}.ld")

mcux_remove_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}
    LINKER ${device_root}/i.MX/i.MX93/MIMX9352/gcc/MIMX9352xxxxM_ram.ld
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
    BUILD_COMMAND ${CMAKE_C_COMPILER} -E -P -x assembler-with-cpp -I${SdkRootDirPath}/${hmc_os_board_path} -DRTOSID=${RTOS_ID} -o ${DDR_LD_FILE}  ${SdkRootDirPath}/${hmc_os_board_path}/armgcc_aarch64/MIMX9352xxxxx_ca55_ddr_ram.ld.S
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
