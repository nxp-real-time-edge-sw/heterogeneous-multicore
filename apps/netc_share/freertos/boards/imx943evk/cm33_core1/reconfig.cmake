SET(hmc_os_board_path "${hmc_root_path}/os/freertos/Core_M/boards/${board}")

mcux_add_configuration(
    CC "-DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL=1"
)
mcux_add_macro(
    CC "-DSCMI_LMM_POWER_CHANGE_PROCESSED=1 \
       -DSRTM_NETC_SERVICE_USED=1 \
       -DNETC_PORT_USE_INCLUDES=1 \
       -DUSE_RTOS=1 \
       -DPRINTF_ADVANCED_ENABLE=1 \
       -DFSL_RTOS_FREE_RTOS"
)

mcux_add_linker_symbol(
    SYMBOLS "__stack_size__=2048\
             __heap_size__=25600"
)

mcux_add_source(
    SOURCES app.h
            app_FreeRTOSConfig.h
            app_srtm.c
            app_srtm.h
            board.c
            board.h
            clock_config.c
            clock_config.h
            hardware_init.c
            lwip_netc_port.h
            lwipopts.h
            pin_mux.c
            pin_mux.h
            rpmsg_config.h
            rsc_table.c
            rsc_table.h
            srtm_config.h
)

mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES middleware/multicore/remoteproc/remoteproc.h
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}
    INCLUDES ${hmc_root_path}/include
             ${hmc_root_path}/os
             ${hmc_os_board_path}
             middleware/multicore/remoteproc
)

include(${SdkRootDirPath}/${hmc_root_path}/os/freertos/Core_M/common_freertos.cmake)

mcux_add_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS debug release
    LINKER ${device_root}/i.MX/i.MX943/MIMX94398/gcc/MIMX94398_cm33_core1_ram.ld
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
