SET(hmc_os_board_path "${hmc_root_path}/os/freertos/Core_M/boards/mcimx93evk")

if(CMAKE_BUILD_TYPE STREQUAL "debug")
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
    SOURCES app_board.h
            app_FreeRTOSConfig.h
            app_virtio_config.h
            board.c
            board.h
            clock_config.c
            clock_config.h
            pin_mux.c
            pin_mux.h
            rsc_table.c
            rsc_table.h
            virtio_board.c
            virtio_board.h
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}
    INCLUDES ${hmc_root_path}/include
             ${hmc_root_path}/os
             ${hmc_os_board_path}
)

include(${SdkRootDirPath}/${hmc_root_path}/os/freertos/Core_M/common_freertos.cmake)
include(${SdkRootDirPath}/${hmc_libs_path}/log/lib_log.cmake)

include(${SdkRootDirPath}/${hmc_root_path}/virtio/mailbox/mu_mailbox.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/virtio/plat_ports/plat_ports_cm.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/virtio/devices/lib_virtio_net.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/net/lib_net_switch.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/net/lib_port_enet_freertos.cmake)
include(${SdkRootDirPath}/${hmc_root_path}/net/lib_switch_hal_freertos.cmake)

mcux_remove_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}
    LINKER ${device_root}/i.MX/i.MX93/MIMX9352/gcc/MIMX9352xxxxM_ram.ld
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

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
