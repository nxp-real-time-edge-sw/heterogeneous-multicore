set(hmc_os_board_path "${hmc_root_path}/os/freertos/Core_M/boards/evkmimx8mm_cm4")

mcux_add_source(
    SOURCES app_board.c
            app_board.h
            app_srtm.c
            app_srtm.h
            board.c
            board.h
            clock_config.c
            clock_config.h
            pin_mux.c
            pin_mux.h
            remoteproc.h
            rpmsg_config.h
            rsc_table.c
            rsc_table.h
            srtm_config.h
)

mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES components/srtm/channels/srtm_rpmsg_endpoint.c
            components/srtm/services/srtm_uart_adapter.c
            components/srtm/services/srtm_uart_service.c
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}
    INCLUDES ${hmc_root_path}/include
             ${hmc_root_path}/os
             ${hmc_os_board_path}
             components/srtm/channels
             components/srtm/services
)

include(${SdkRootDirPath}/${hmc_root_path}/os/freertos/Core_M/common_freertos.cmake)

include(${SdkRootDirPath}/${hmc_libs_path}/log/lib_log.cmake)
include(${SdkRootDirPath}/${hmc_libs_path}/rpmsg/lib_rpmsg.cmake)

mcux_remove_armgcc_linker_script(
  BASE_PATH ${SdkRootDirPath}
  LINKER ${device_root}/i.MX/i.MX8MM/MIMX8MM6/gcc/MIMX8MM6xxxLZ_ram.ld
  TARGETS
    debug
    release
)

mcux_add_armgcc_linker_script(
  BASE_PATH ${SdkRootDirPath}
  LINKER ${hmc_os_board_path}/armgcc/MIMX8MM6xxxxx_cm4_ram.ld
  TARGETS
    debug
    release
)

mcux_add_macro(
  CC "-DMCUXPRESSO_SDK\
     -DFSL_RTOS_FREE_RTOS\
     -DPRINTF_ADVANCED_ENABLE=1\
     -DconfigTOTAL_HEAP_SIZE=0xa000\
     -DSRTM_DEBUG_MESSAGE_FUNC=DbgConsole_Printf\
     -DSRTM_DEBUG_VERBOSE_LEVEL=SRTM_DEBUG_VERBOSE_WARN"
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/rpmsg_uart_sharing_cm4.bin)
