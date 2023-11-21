# SOC DEVICE SETTING
SET(MCUX_DEVICE "MIMX8MM6_ca53")

# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 3.1.1)

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
SET(RtosPath "${ProjRootPath}/../FreeRTOS-Kernel")
SET(BoardPath "${ProjRootPath}/os/freertos/Core_AArch64/boards/evkmimx8mm")
SET(OsPath "${ProjRootPath}/os")
SET(IncludePath "${ProjRootPath}/include")
SET(AppPath "${ProjRootPath}/apps/hello_world")

SET(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})
SET(LIBRARY_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})

project(hello_world_ca53)

message("building RTOS${RTOS_ID} with ${CONSOLE} Console ...")
if(${CONSOLE} STREQUAL "RAM_CONSOLE")
    SET(DEBUG_CONSOLE_CONFIG "-DSDK_DEBUGCONSOLE=2 -DCONFIG_RAM_CONSOLE")
elseif(${CONSOLE} STREQUAL "UART2")
    ADD_DEFINITIONS(-DBOARD_DEBUG_UART_INSTANCE=2)
elseif(${CONSOLE} STREQUAL "UART3")
    ADD_DEFINITIONS(-DBOARD_DEBUG_UART_INSTANCE=3)
elseif(${CONSOLE} STREQUAL "UART4")
    ADD_DEFINITIONS(-DBOARD_DEBUG_UART_INSTANCE=4)
else()
    message(FATAL_ERROR "Unsuported Console ${CONSOLE} ...")
endif()

set(MCUX_SDK_PROJECT_NAME hello_world_ca53_RTOS${RTOS_ID}_${CONSOLE}.elf)

# linker script created by using memory defined in rtos_memory.h
set(DDR_LD_FILE "MIMX8MM6xxxxx_ca53_ddr_ram_RTOS${RTOS_ID}.ld")

include(${BoardPath}/armgcc_aarch64/flags.cmake)
add_executable(${MCUX_SDK_PROJECT_NAME}
    "${AppPath}/freertos/main.c"
    "${BoardPath}/board.c"
    "${BoardPath}/mmu.c"
    "${ProjDirPath}/../hello_world_board.c"
    "${ProjDirPath}/../clock_config.c"
    "${ProjDirPath}/../pin_mux.c"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${IncludePath}
    ${ProjDirPath}/..
    ${OsPath}
    ${BoardPath}
    ${AppBoardPath}
    ${SdkDirPath}/devices/MIMX8MM6
)

set(CMAKE_MODULE_PATH
    ${OsPath}/freertos/Core_AArch64
    ${OsPath}/freertos/Core_AArch64/core/armv8a
    ${LibsPath}/log
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

include(middleware_freertos-kernel_extension)
include(middleware_freertos-kernel_heap_4)
include(middleware_freertos-kernel_aarch64)

if(${CONSOLE} STREQUAL "RAM_CONSOLE")
    include(utility_assert_ram_console)
    include(utility_ram_console)
else()
    include(utility_assert)
    include(utility_debug_console)
endif()
include(utilities_misc_utilities_aarch64)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE m)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE c)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE gcc)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE nosys)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY}
-Obinary ${EXECUTABLE_OUTPUT_PATH}/${MCUX_SDK_PROJECT_NAME} ${EXECUTABLE_OUTPUT_PATH}/hello_world_ca53_RTOS${RTOS_ID}_${CONSOLE}.bin)

# create linker script by using memory defined in rtos_memory.h
ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_C_COMPILER}
-E -P -x assembler-with-cpp -I${BoardPath} -DRTOSID=${RTOS_ID} -o ${DDR_LD_FILE}  ${BoardPath}/armgcc_aarch64/MIMX8MM6xxxxx_ca53_ddr_ram.ld.S)
