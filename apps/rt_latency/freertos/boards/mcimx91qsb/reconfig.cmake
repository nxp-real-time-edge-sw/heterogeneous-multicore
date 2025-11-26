set(hmc_os_board_path "${hmc_root_path}/os/freertos/Core_AArch64/boards/${board}")

if(CMAKE_BUILD_TYPE STREQUAL "ddr_debug")
    mcux_add_macro(
        CC "-DCONFIG_CPU_LOAD_STATS=1"
    )
endif()

mcux_add_macro(
    CC "-DFSL_RTOS_FREE_RTOS\
        -DPRINTF_ADVANCED_ENABLE=1"
)

mcux_add_source(
    SOURCES app_board.h
            app_mmu.h
            clock_config.c
            clock_config.h
            pin_mux.c
            pin_mux.h
            rt_latency_board.c
            rt_latency_board.h
)

mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES ${hmc_os_board_path}/mmu.c
            ${hmc_os_board_path}/board.c
            ${hmc_app_path}/../common/rt_latency.c
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
include(${SdkRootDirPath}/${hmc_root_path}/os/freertos/driver_counter_tpm.cmake)

mcux_add_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}/${hmc_os_board_path}/armgcc_aarch64
    LINKER MIMX9111xxxxx_ca55_ddr_ram.ld
    TARGETS
      ddr_debug
      ddr_release
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
