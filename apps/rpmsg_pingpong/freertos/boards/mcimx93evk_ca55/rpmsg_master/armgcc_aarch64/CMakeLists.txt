# SOC DEVICE SETTING
SET(MCUX_DEVICE "MIMX9352_ca55")

# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 3.12.0)

# THE VERSION NUMBER
SET (Tutorial_VERSION_MAJOR 1)
SET (Tutorial_VERSION_MINOR 0)

# config to select component, the format is CONFIG_USE_${component}
set(CONFIG_USE_middleware_multicore_rpmsg_lite_freertos true)
set(CONFIG_USE_middleware_freertos-kernel_MIMX9352 true)

# ENABLE ASM
ENABLE_LANGUAGE(ASM)

# ENABLE DRIVER CACHE CONTROL
ADD_DEFINITIONS(-DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1)

ADD_DEFINITIONS(-DAARCH64=1)

ADD_DEFINITIONS(-DGEN_SW_MBOX_MASTER=1)

SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)

SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

# CURRENT DIRECTORY
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})

SET(ProjRootPath "${ProjDirPath}/../../../../../../../")
SET(SdkDirPath "${ProjRootPath}/../mcux-sdk")
SET(RtosPath "${ProjRootPath}/../FreeRTOS-Kernel")
SET(OsPath "${ProjRootPath}/os")
SET(BoardPath "${OsPath}/freertos/Core_AArch64/boards/mcimx93evk")
SET(LibsPath "${ProjRootPath}/libs")
SET(IncludePath "${ProjRootPath}/include")
SET(AppPath "${ProjRootPath}/apps/rpmsg_pingpong")
SET(MulticorePath "${ProjRootPath}/../middleware/multicore")

SET(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})
SET(LIBRARY_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})

project(rpmsg_pingpong_master_ca55)

message("building RTOS with ${CONSOLE} Console ...")
if(${CONSOLE} STREQUAL "RAM_CONSOLE")
    SET(DEBUG_CONSOLE_CONFIG "-DSDK_DEBUGCONSOLE=2 -DCONFIG_RAM_CONSOLE")
elseif(${CONSOLE} STREQUAL "UART1")
    ADD_DEFINITIONS(-DBOARD_DEBUG_UART_INSTANCE=1)
elseif(${CONSOLE} STREQUAL "UART2")
    ADD_DEFINITIONS(-DBOARD_DEBUG_UART_INSTANCE=2)
else()
    message(FATAL_ERROR "Unsuported Console ${CONSOLE} ...")
endif()

set(MCUX_SDK_PROJECT_NAME rpmsg_pingpong_master_ca55_${CONSOLE}.elf)

# linker script created by using memory defined in rtos_memory.h
set(DDR_LD_FILE "MIMX9352xxxxx_ca55_ddr_ram_RTOS${RTOS_ID}.ld")

include(${BoardPath}/armgcc_aarch64/flags.cmake)
add_executable(${MCUX_SDK_PROJECT_NAME}
    "${AppPath}/freertos/main_master.c"
    "${BoardPath}/board.c"
    "${BoardPath}/mmu.c"
    "${ProjDirPath}/../app_board.c"
    "${ProjDirPath}/../pin_mux.c"
    "${ProjDirPath}/../clock_config.c"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${IncludePath}
    ${ProjDirPath}/..
    ${OsPath}
    ${BoardPath}
    ${AppBoardPath}
    ${SdkDirPath}/devices/MIMX9352
)

set(CMAKE_MODULE_PATH
    ${OsPath}/freertos/Core_AArch64
    ${OsPath}/freertos/Core_AArch64/core/armv8a
    ${MulticorePath}
    ${RtosPath}
    ${SdkDirPath}
    ${LibsPath}/gen_sw_mbox
    ${LibsPath}/rpmsg
    ${LibsPath}/log
)

# include entry for cmake components
include(all_devices)

# include modules
include(device_system_ca55)

include(common_freertos)
include(common_freertos_core_armv8a)

include(component_lpuart_adapter)
include(component_lists)
include(component_serial_manager)
include(component_serial_manager_uart)

include(driver_cache_armv8a)
include(driver_clock)
include(driver_common)
include(driver_lpuart)

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

include(lib_log)
include(lib_rpmsg)
include(lib_gen_sw_mbox)

include(middleware_multicore_rpmsg_lite_aarch64_freertos)
include(middleware_multicore_rpmsg_lite_MIMX9352)
include(middleware_multicore_rpmsg_lite_freertos)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE m)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE c)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE gcc)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE nosys)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY}
-Obinary ${EXECUTABLE_OUTPUT_PATH}/${MCUX_SDK_PROJECT_NAME} ${EXECUTABLE_OUTPUT_PATH}/rpmsg_pingpong_master_ca55_${CONSOLE}.bin)

# create linker script by using memory defined in rtos_memory.h
ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_C_COMPILER}
-E -P -x assembler-with-cpp -I${BoardPath} -DRTOSID=${RTOS_ID} -o ${DDR_LD_FILE}  ${BoardPath}/armgcc_aarch64/MIMX9352xxxxx_ca55_ddr_ram.ld.S)
