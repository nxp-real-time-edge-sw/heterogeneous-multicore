cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(soem_servo)

if(CONFIG_BOARD_MIMX8MM_EVK_A53)
    set(BoardName "evkmimx8mm")
elseif(CONFIG_BOARD_MIMX8MP_EVK_A53)
  set(BoardName "evkmimx8mp")
elseif(CONFIG_BOARD_MIMX93_EVK_A55)
    set(BoardName "mcimx93evk")
endif()

set(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})
SET(ProjRootPath "${ProjDirPath}/../../../")
SET(MiddlewarePath "${ProjRootPath}/../mcuxsdk/mcuxsdk/middleware")
set(OsPath "${ProjDirPath}/../../../os")
set(HeaderPath "${ProjRootPath}/include")
SET(LibsPath "${ProjRootPath}/libs")
set(AppPath "${ProjDirPath}/..")
set(BoardPath "${ProjDirPath}/boards/${BoardName}")

zephyr_include_directories(
  ${AppPath}
  ${HeaderPath}
  ${OsPath}
  ${ProjDirPath}
)

set(MCUX_SDK_PROJECT_NAME app)

set(CMAKE_MODULE_PATH
  ${LibsPath}/log
  ${LibsPath}/stats
  ${MiddlewarePath}/soem
)

zephyr_compile_definitions(OS_ZEPHYR)

# Cortex-A53 core maximum clock frequency
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX8MM_EVK_A53 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1800000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX8MP_EVK_A53 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1800000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX93_EVK_A55 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1700000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX95_EVK_A55 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1800000000UL)

include(lib_log)
include(lib_stats)
include(middleware_soem_zephyr)

target_sources(app PRIVATE main.c)
target_sources(app PRIVATE axis_nc.c)
target_sources(app PRIVATE cia402.c)
target_sources(app PRIVATE servo.c)
