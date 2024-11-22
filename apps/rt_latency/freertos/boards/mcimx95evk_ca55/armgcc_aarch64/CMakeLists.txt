# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 3.12.0)

# THE VERSION NUMBER
SET (Tutorial_VERSION_MAJOR 1)
SET (Tutorial_VERSION_MINOR 0)

ADD_DEFINITIONS(-DAARCH64=1)

if(${CMAKE_BUILD_TYPE} STREQUAL "ddr_debug")
	ADD_DEFINITIONS(-DCONFIG_CPU_LOAD_STATS=1)
endif()

# ENABLE ASM
ENABLE_LANGUAGE(ASM)

SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)

SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

SET(BoardName "mcimx95evk_ca55")

SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})
SET(ProjRootPath "${ProjDirPath}/../../../../../../")
SET(SdkDirPath "${ProjRootPath}/../mcux-sdk")
SET(RtosPath "${ProjRootPath}/../FreeRTOS-Kernel")
SET(BoardPath "${ProjRootPath}/os/freertos/Core_AArch64/boards/mcimx95evk")
SET(OsPath "${ProjRootPath}/os")
SET(LibsPath "${ProjRootPath}/libs")
SET(IncludePath "${ProjRootPath}/include")
SET(AppPath "${ProjRootPath}/apps/rt_latency")

SET(ProjRootPath "${ProjDirPath}/../../../../../")

SET(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})
SET(LIBRARY_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})

project(rt_latency)
set(BIN_NAME rt_latency_ca55)

set(MCUX_SDK_PROJECT_NAME ${BIN_NAME}.elf)

include(${BoardPath}/armgcc_aarch64/flags.cmake)
include(${ProjDirPath}/config.cmake)

add_executable(${MCUX_SDK_PROJECT_NAME}
    "${AppPath}/freertos/main.c"
    "${AppPath}/common/rt_latency.c"
    "${BoardPath}/board.c"
    "${BoardPath}/mmu.c"
    "${ProjDirPath}/../rt_latency_board.c"
    "${AppPath}/freertos/boards/${BoardName}/clock_config.c"
    "${AppPath}/freertos/boards/${BoardName}/pin_mux.c"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${IncludePath}
    ${AppPath}
    ${AppPath}/common
    ${AppPath}/freertos
    ${AppPath}/common/boards/${BoardName}
    ${AppPath}/freertos/boards/${BoardName}
    ${OsPath}
    ${BoardPath}
    ${SdkDirPath}/devices/MIMX9596
)

set(CMAKE_MODULE_PATH
    ${OsPath}/freertos
    ${OsPath}/freertos/Core_AArch64
    ${OsPath}/freertos/Core_AArch64/core/armv8a
    ${LibsPath}/log
    ${LibsPath}/stats
    ${RtosPath}
    ${SdkDirPath}
)

# include entry for cmake components
include(all_devices)

# include modules
include(lib_log)
include(lib_stats)
include(driver_counter_tpm)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE m)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE c)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE gcc)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE nosys)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY}
	-Obinary ${EXECUTABLE_OUTPUT_PATH}/${MCUX_SDK_PROJECT_NAME} ${EXECUTABLE_OUTPUT_PATH}/${BIN_NAME}.bin)
