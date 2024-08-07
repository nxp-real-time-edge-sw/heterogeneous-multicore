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

SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})
SET(ProjRootPath "${ProjDirPath}/../../../../../../")
SET(IncludePath "${ProjRootPath}/include")
SET(SdkDirPath "${ProjRootPath}/../mcux-sdk")
SET(RtosPath "${ProjRootPath}/../FreeRTOS-Kernel")
SET(OsPath "${ProjRootPath}/os")
SET(OsBoardPath "${ProjRootPath}/os/freertos/Core_M/boards/evkmimx8mm_cm4")
SET(BoardPath "${SdkDirPath}/boards/evkmimx8mm")
SET(AppPath "${ProjRootPath}/apps/hello_world")

SET(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})
SET(LIBRARY_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})

project(hello_world)

set(MCUX_SDK_PROJECT_NAME hello_world_cm4_UART4.elf)

include(${OsBoardPath}/armgcc/flags.cmake)

include(${ProjDirPath}/config.cmake)

add_executable(${MCUX_SDK_PROJECT_NAME}
    "${AppPath}/freertos/main.c"
    "${BoardPath}/board.c"
    "${BoardPath}/clock_config.c"
    "${ProjDirPath}/../pin_mux.c"
    "${ProjDirPath}/../hello_world_board.c"
    "${ProjDirPath}/../rsc_table.c"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${BoardPath}
    ${IncludePath}
    ${ProjDirPath}/..
    ${OsPath}
    ${OsBoardPath}
    ${SdkDirPath}/devices/MIMX8MM6
)

set(CMAKE_MODULE_PATH
    ${OsPath}/freertos/Core_M
    ${PlatPortsPath}
    ${MailboxPath}
    ${NetPath}
    ${RtosPath}
    ${SdkDirPath}
)

#Include Entry cmake component
include(all_devices)

# include modules
include(common_freertos)

include(middleware_freertos-kernel_heap_4)

include(driver_clock)

include(driver_mu)

include(middleware_freertos-kernel_MIMX8MM6)

include(driver_common)

include(driver_rdc)

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

include(utilities_misc_utilities_MIMX8MM6)

include(device_system)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE m)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE c)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE gcc)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE nosys)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY}
-Obinary ${EXECUTABLE_OUTPUT_PATH}/${MCUX_SDK_PROJECT_NAME} ${EXECUTABLE_OUTPUT_PATH}/hello_world_cm4_UART4.bin)
