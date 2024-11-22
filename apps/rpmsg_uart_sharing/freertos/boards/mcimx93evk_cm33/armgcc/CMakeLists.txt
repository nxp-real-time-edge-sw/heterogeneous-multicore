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

# FreeRTOS heap size 40KB
ADD_DEFINITIONS(-DconfigTOTAL_HEAP_SIZE=0xa000)

# UART non-blocking mode
ADD_DEFINITIONS(-DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING)

# CURRENT DIRECTORY
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})

SET(ProjRootPath "${ProjDirPath}/../../../../../../")
SET(SdkDirPath "${ProjRootPath}/../mcux-sdk")
SET(RtosPath "${ProjRootPath}/../FreeRTOS-Kernel")
SET(OsPath "${ProjRootPath}/os")
SET(OsBoardPath "${ProjRootPath}/os/freertos/Core_M/boards/mcimx93evk_cm33")
SET(LibsPath "${ProjRootPath}/libs")
SET(IncludePath "${ProjRootPath}/include")
SET(AppPath "${ProjRootPath}/apps/rpmsg_uart_sharing")
SET(MulticorePath "${ProjRootPath}/../middleware/multicore")

SET(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})
SET(LIBRARY_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})

project(rpmsg_uart_sharing)

set(MCUX_SDK_PROJECT_NAME rpmsg_uart_sharing.elf)

include(${OsBoardPath}/armgcc/flags.cmake)

include(${ProjDirPath}/config.cmake)

add_executable(${MCUX_SDK_PROJECT_NAME}
    "${AppPath}/freertos/main.c"
    "${ProjDirPath}/../app_board.c"
    "${ProjDirPath}/../app_srtm.c"
    "${ProjDirPath}/../board.c"
    "${ProjDirPath}/../clock_config.c"
    "${ProjDirPath}/../pin_mux.c"
    "${ProjDirPath}/../rsc_table.c"
    "${SdkDirPath}/components/srtm/channels/srtm_rpmsg_endpoint.c"
    "${SdkDirPath}/components/srtm/services/srtm_uart_adapter.c"
    "${SdkDirPath}/components/srtm/services/srtm_uart_service.c"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${IncludePath}
    ${ProjDirPath}/..
    ${OsPath}
    ${OsBoardPath}
    ${SdkDirPath}/devices/MIMX9352
    ${SdkDirPath}/components/srtm/channels
    ${SdkDirPath}/components/srtm/services
)

#path for entry cmake
set(CMAKE_MODULE_PATH
    ${OsPath}/freertos/Core_M
    ${MulticorePath}
    ${RtosPath}
    ${SdkDirPath}
)

#Include Entry cmake component
include(all_devices)

# include modules
include(common_freertos)

include(middleware_multicore_rpmsg_lite_MIMX9352)

include(middleware_multicore_rpmsg_lite_freertos)

include(driver_clock)

include(component_lpuart_adapter)

include(component_serial_manager_uart)

include(device_startup)

include(utility_assert)

include(driver_cache_xcache)

include(driver_memory)

include(driver_mu1)

include(driver_srtm_freertos)

include(driver_srtm_MIMX9352)

include(middleware_freertos-kernel_cm33_nonsecure_port)

include(middleware_freertos-kernel_extension)

include(middleware_multicore_rpmsg_lite_imx93_m33_freertos)

include(middleware_freertos-kernel_heap_4)

include(utilities_misc_utilities_MIMX8MM6)

include(device_system)

IF(NOT DEFINED TARGET_LINK_SYSTEM_LIBRARIES)
    SET(TARGET_LINK_SYSTEM_LIBRARIES "-lm -lc -lgcc -lnosys")
ENDIF()

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE ${TARGET_LINK_SYSTEM_LIBRARIES})

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY}
-Obinary ${EXECUTABLE_OUTPUT_PATH}/${MCUX_SDK_PROJECT_NAME} ${EXECUTABLE_OUTPUT_PATH}/rpmsg_uart_sharing.bin)
