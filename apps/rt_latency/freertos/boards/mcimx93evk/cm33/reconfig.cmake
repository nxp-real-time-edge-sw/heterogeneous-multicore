SET(hmc_os_board_path "${hmc_root_path}/os/freertos/Core_M/boards/mcimx93evk")

mcux_add_source(
    SOURCES app_board.h
            board.c
            board.h
            clock_config.c
            clock_config.h
            rt_latency_board.c
            rt_latency_board.h
            pin_mux.c
            pin_mux.h
            rsc_table.c
            rsc_table.h
)

mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES ${hmc_app_path}/../common/rt_latency.c
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}
    INCLUDES ${hmc_root_path}/include
             ${hmc_root_path}/os
             ${hmc_os_board_path}
             ${hmc_app_path}/../common/
)

include(${SdkRootDirPath}/${hmc_root_path}/os/freertos/Core_M/common_freertos.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/libs/log/lib_log.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/libs/stats/lib_stats.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/os/freertos/driver_counter_tpm.cmake)

mcux_remove_armgcc_linker_script(
  BASE_PATH ${SdkRootDirPath}
  LINKER ${device_root}/i.MX/i.MX8MM/MIMX8MM6/gcc/MIMX9352xxxxM_ram.ld
  TARGETS
    debug
    release
)

mcux_add_armgcc_linker_script(
  BASE_PATH ${SdkRootDirPath}
  LINKER ${hmc_os_board_path}/armgcc/MIMX9352_cm33_ram.ld
  TARGETS
    debug
    release
)

mcux_add_macro(
  CC "-DFSL_RTOS_FREE_RTOS\
     -DPRINTF_ADVANCED_ENABLE=1"
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
