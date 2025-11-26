cmake_minimum_required(VERSION 3.22.0)

set(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})
get_filename_component(BOARD_ROOT_ABSOLUTE ${ProjDirPath}/boards REALPATH)
set(CUSTOM_BOARD_ROOT ${BOARD_ROOT_ABSOLUTE})

message(STATUS "CUSTOM_BOARD_ROOT absolute path: ${CUSTOM_BOARD_ROOT}")

include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

set(hmc_root_path "../../heterogeneous-multicore")
set(hmc_app_path ${hmc_root_path}/apps/netc_share/freertos)
set(hmc_app_board_path "${hmc_app_path}/boards/${board}")
set(hmc_app_board_core_path "${hmc_app_path}/boards/${board}/${core_id}")

project(netc_share LANGUAGES C CXX ASM PROJECT_BOARD_PORT_PATH ${hmc_app_board_path})

include(${SdkRootDirPath}/CMakeLists.txt)

mcux_add_source(
    SOURCES main.c
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}
    INCLUDES ${hmc_app_board_path}
    INCLUDES ${hmc_app_board_core_path}
)

include(${SdkRootDirPath}/${hmc_app_board_path}/reconfig.cmake OPTIONAL)
include(${SdkRootDirPath}/${hmc_app_board_core_path}/reconfig.cmake OPTIONAL)

mcux_add_configuration(
    TARGETS debug
            ddr_debug
    CC "-DDEBUG"
    CX "-DDEBUG"
)

mcux_add_configuration(
    TARGETS release
            ddr_release
    CC "-DNDEBUG"
    CX "-DNDEBUG"
)
