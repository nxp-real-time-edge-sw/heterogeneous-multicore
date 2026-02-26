mcux_set_variable(hmc_os_board_path "${hmc_root_path}/os/freertos/Core_AArch64/boards/${board}")

mcux_add_configuration(
    CC "-DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL=1 \
        -DFSL_ETH_ENABLE_CACHE_CONTROL=1 \
        -DSDK_I2C_BASED_COMPONENT_USED=1 \
        -DBOARD_USE_PCAL6524=1 \
        -DBOARD_USE_PCAL6408=1 \
        -DNETC_PORT_USE_INCLUDES=1 \
        -DUSE_RTOS=1 \
        -DPRINTF_ADVANCED_ENABLE=1 \
        -DNETC_MSGINTR_PRIORITY=\"(portLOWEST_USABLE_INTERRUPT_PRIORITY-1)\" \
        -DSDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1800000000 \
        -DLWIP_PING_USE_ENETC=0 \
        -DFSL_RTOS_FREE_RTOS"
)

mcux_add_linker_symbol(
    SYMBOLS "__stack_size__=2048\
             __heap_size__=25600"
)
mcux_add_source(
    SOURCES app_board.h
            app_enet.h
            app_mmu.h
            clock_config.c
            clock_config.h
            hardware_init.c
            lwipopts.h
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
             ${hmc_app_path}/../common/
)

include(${SdkRootDirPath}/${hmc_root_path}/os/freertos/Core_AArch64/common_freertos.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/os/freertos/Core_AArch64/core/armv8a/common_freertos_core_armv8a.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/libs/log/lib_log.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/libs/stats/lib_stats.cmake)

mcux_add_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}
    LINKER ${hmc_os_board_path}/armgcc_aarch64/MIMX9596xxxxx_ca55_ddr_ram.ld
    TARGETS
      ddr_debug
      ddr_release
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
