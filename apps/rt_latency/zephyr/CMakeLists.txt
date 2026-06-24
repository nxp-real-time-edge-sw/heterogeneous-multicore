cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(rt_latency)

if(CONFIG_BOARD_IMX8MM_EVK)
  set(BoardName "evkmimx8mm")
elseif(CONFIG_BOARD_IMX8MP_EVK)
  set(BoardName "evkmimx8mp")
elseif(CONFIG_BOARD_IMX91_QSB)
  set(BoardName "mcimx91qsb")
elseif(CONFIG_BOARD_IMX91_EVK)
  set(BoardName "mcimx91evk")
elseif(CONFIG_BOARD_IMX93_EVK)
  set(BoardName "mcimx93evk")
elseif(CONFIG_BOARD_IMX943_EVK)
  set(BoardName "imx943evk")
elseif(CONFIG_BOARD_IMX95_EVK)
  set(BoardName "imx95lpd5evk19")
else()
  message(FATAL_ERROR "unsupported board")
endif()

if(CONFIG_CPU_CORTEX_A53)
  set(CpuName "ca53")
elseif(CONFIG_CPU_CORTEX_A55)
  set(CpuName "ca55")
else()
  message(FATAL_ERROR "unsupported CPU")
endif()

set(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})
SET(ProjRootPath "${ProjDirPath}/../../../")
set(OsPath "${ProjDirPath}/../../../os")
set(HeaderPath "${ProjRootPath}/include")
SET(LibsPath "${ProjRootPath}/libs")
set(AppPath "${ProjDirPath}/..")
set(BoardPath "${ProjDirPath}/boards/${BoardName}")
set(CpuPath "${ProjDirPath}/boards/${BoardName}/${CpuName}")

# config to select component, the format is CONFIG_USE_${component}
set(CONFIG_USE_middleware_zephyr-kernel true)
set(CONFIG_USE_middleware_multicore_rpmsg_lite_zephyr true)
ADD_DEFINITIONS(-DAARCH64=1)

zephyr_include_directories(
  ${AppPath}/common
  ${BoardPath}
  ${CpuPath}
  ${HeaderPath}
  ${OsPath}
  ${ProjDirPath}
)

set(MCUX_SDK_PROJECT_NAME app)

set(CMAKE_MODULE_PATH
    ${LibsPath}/log
    ${LibsPath}/stats
)

zephyr_compile_definitions(OS_ZEPHYR)

# Cortex-A55/A53 core maximum clock frequency
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX8MM_EVK_A53 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1800000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX8MN_EVK_A53 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1600000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX8MP_EVK_A53 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1800000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX91_EVK SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1400000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX93_EVK_A55 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1700000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX95_EVK_A55 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1800000000UL)

include(lib_log)
include(lib_stats)

target_sources(app PRIVATE main.c)
target_sources(app PRIVATE ../common/rt_latency.c)
