set(hmc_os_board_path "${hmc_root_path}/os/freertos/Core_AArch64/boards/evkmimx8mm")

if(CMAKE_BUILD_TYPE STREQUAL "ddr_debug")
    mcux_add_macro(
        CC "-DCONFIG_CPU_LOAD_STATS=1"
    )
endif()

mcux_add_macro(
    CC "-DFSL_RTOS_FREE_RTOS\
        -DPRINTF_ADVANCED_ENABLE=1\
        -DPRINTF_FLOAT_ENABLE=1"
)

mcux_add_source(
    SOURCES app_mmu.h
            app_virtio_config.h
            gen_sw_mbox_config.h
            virtio_board.c
            virtio_board.h
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
include(${SdkRootDirPath}/${hmc_libs_path}/log/lib_log.cmake)

include(${SdkRootDirPath}/${hmc_root_path}/virtio/mailbox/generic_software_mailbox.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/virtio/plat_ports/plat_ports_ca.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/virtio/devices/lib_virtio_trans.cmake)

mcux_remove_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}
    LINKER ${device_root}/i.MX/i.MX8MM/MIMX8MM6/gcc/MIMX8MM6xxxLZ_ddr_ram.ld
    TARGETS
        ddr_debug
        ddr_release
)

mcux_add_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}
    LINKER ${hmc_os_board_path}/armgcc_aarch64/MIMX8MM6xxxxx_ca53_ddr_ram.ld
    TARGETS
        ddr_debug
        ddr_release
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
