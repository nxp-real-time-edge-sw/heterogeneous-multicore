# SOC DEVICE SETTING
SET(MCUX_DEVICE "MIMX8MM6_ca53")

# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 3.12.0)

# THE VERSION NUMBER
SET (Tutorial_VERSION_MAJOR 1)
SET (Tutorial_VERSION_MINOR 0)

# ENABLE DRIVER CACHE CONTROL
ADD_DEFINITIONS(-DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1)

# ENABLE ASM
ENABLE_LANGUAGE(ASM)

SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)

SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})
SET(ProjRootPath "${ProjDirPath}/../../../../../../")
SET(SdkDirPath "${ProjRootPath}/../mcux-sdk")
SET(MiddlewarePath "${ProjRootPath}/../middleware")
SET(RtosPath "${ProjRootPath}/../FreeRTOS-Kernel")
SET(BoardPath "${ProjRootPath}/os/freertos/Core_AArch64/boards/evkmimx8mm")
SET(OsPath "${ProjRootPath}/os")
SET(IncludePath "${ProjRootPath}/include")
SET(AppPath "${ProjRootPath}/apps/soem_servo_rt1180")
SET(OsPath "${ProjRootPath}/os")
SET(LibsPath "${ProjRootPath}/libs")

SET(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})
SET(LIBRARY_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})

project(soem_servo_ca53)

set(MCUX_SDK_PROJECT_NAME soem_servo_ca53.elf)

include(${BoardPath}/armgcc_aarch64/flags.cmake)
add_executable(${MCUX_SDK_PROJECT_NAME}
    "${AppPath}/freertos/main.c"
	"${AppPath}/freertos/axis_nc.c"
	"${AppPath}/freertos/cia402.c"
	"${AppPath}/freertos/servo.c"
    "${BoardPath}/board.c"
    "${BoardPath}/mmu.c"
    "${ProjDirPath}/../clock_config.c"
    "${ProjDirPath}/../pin_mux.c"
    "${ProjDirPath}/../app_enet.h"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${IncludePath}
    ${ProjDirPath}/..
    ${OsPath}
    ${BoardPath}
    ${SdkDirPath}/devices/MIMX8MM6
)

set(CMAKE_MODULE_PATH
    ${OsPath}/freertos/Core_AArch64
    ${OsPath}/freertos/Core_AArch64/core/armv8a
    ${OsPath}/freertos/
    ${LibsPath}/log
    ${MiddlewarePath}/soem
    ${RtosPath}
    ${SdkDirPath}
)

# include entry for cmake components

include(all_devices)

# include modules

include(device_system_ca53)

include(common_freertos)
include(common_freertos_core_armv8a)

include(component_iuart_adapter)
include(component_lists)
include(component_serial_manager)
include(component_serial_manager_uart)

include(driver_cache_armv8a)
include(driver_clock)
include(driver_common)
include(driver_iuart)
include(driver_rdc)
include(driver_counter_gpt)
include(driver_gpt)

include(lib_log)

include(driver_enet)
include(driver_phy-common)
include(driver_phy-device-ar8031)

include(middleware_freertos-kernel_extension)
include(middleware_freertos-kernel_heap_4)
include(middleware_freertos-kernel_aarch64)

include(middleware_soem_freertos)

include(utility_assert)
include(utility_debug_console)
include(utilities_misc_utilities_aarch64)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE m)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE c)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE gcc)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE nosys)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY}
-Obinary ${EXECUTABLE_OUTPUT_PATH}/${MCUX_SDK_PROJECT_NAME} ${EXECUTABLE_OUTPUT_PATH}/soem_servo_ca53.bin)
