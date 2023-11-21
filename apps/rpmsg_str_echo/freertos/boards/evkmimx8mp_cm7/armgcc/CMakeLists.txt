# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 3.10.0)

# THE VERSION NUMBER
SET (Tutorial_VERSION_MAJOR 1)
SET (Tutorial_VERSION_MINOR 0)

# ENABLE ASM
ENABLE_LANGUAGE(ASM)

SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)

SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

# CURRENT DIRECTORY
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})

SET(ProjRootPath "${ProjDirPath}/../../../../../../")
SET(SdkDirPath "${ProjRootPath}/../mcux-sdk")
SET(RtosPath "${ProjRootPath}/../FreeRTOS-Kernel")
SET(OsPath "${ProjRootPath}/os")
SET(OsBoardPath "${ProjRootPath}/os/freertos/Core_M/boards/evkmimx8mp_cm7")
SET(BoardPath "${SdkDirPath}/boards/evkmimx8mp")
SET(LibsPath "${ProjRootPath}/libs")
SET(IncludePath "${ProjRootPath}/include")
SET(AppPath "${ProjRootPath}/apps/rpmsg_str_echo")

SET(MulticorePath "${ProjRootPath}/../middleware/multicore")

SET(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})
SET(LIBRARY_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})

project(rpmsg_str_echo_cm7)

set(MCUX_BUILD_TYPES debug release ddr_debug ddr_release flash_debug flash_release)

set(MCUX_SDK_PROJECT_NAME rpmsg_str_echo_cm7.elf)

include(${OsBoardPath}/armgcc/flags.cmake)

include(${ProjDirPath}/config.cmake)

add_executable(${MCUX_SDK_PROJECT_NAME}
    "${AppPath}/freertos/main.c"
    "${BoardPath}/board.c"
    "${BoardPath}/clock_config.c"
    "${ProjDirPath}/../app_board.c"
    "${ProjDirPath}/../pin_mux.c"
    "${ProjDirPath}/../rsc_table.c"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${IncludePath}
    ${ProjDirPath}/..
    ${OsPath}
    ${OsBoardPath}
    ${BoardPath}
    ${AppBoardPath}
    ${SdkDirPath}/devices/MIMX8ML8
)

set(CMAKE_MODULE_PATH
    ${OsPath}/freertos/Core_M
    ${MulticorePath}
    ${RtosPath}
    ${SdkDirPath}
    ${LibsPath}/rpmsg
    ${LibsPath}/log
)

# include entry for cmake components
include(all_devices)

# include modules
include(common_freertos)

include(middleware_multicore_rpmsg_lite_imx8mp_m7_freertos)

include(middleware_multicore_rpmsg_lite_MIMX8ML8)

include(middleware_multicore_rpmsg_lite_freertos)

include(middleware_freertos-kernel_heap_4)

include(driver_clock)

include(driver_mu)

include(middleware_freertos-kernel_MIMX8ML8)

include(driver_common)

include(driver_rdc)

include(driver_audiomix)

include(device_CMSIS)

include(utility_debug_console)

include(component_iuart_adapter)

include(component_serial_manager)

include(component_lists)

include(component_serial_manager_uart)

include(driver_iuart)

include(device_startup)

include(utility_assert)

include(middleware_freertos-kernel_extension)

include(CMSIS_Include_core_cm)

include(utilities_misc_utilities_MIMX8ML8)

include(device_system)

include(lib_log)

include(lib_rpmsg)

IF(NOT DEFINED TARGET_LINK_SYSTEM_LIBRARIES)
    SET(TARGET_LINK_SYSTEM_LIBRARIES "-lm -lc -lgcc -lnosys")
ENDIF()

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE ${TARGET_LINK_SYSTEM_LIBRARIES})

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY}
-Obinary ${EXECUTABLE_OUTPUT_PATH}/${MCUX_SDK_PROJECT_NAME} ${EXECUTABLE_OUTPUT_PATH}/rpmsg_str_echo_cm7.bin)

