cmake_minimum_required(VERSION 3.13.1)

message("Building RTOS${RTOS_ID} with ${CONSOLE} Console ...")

set(DTS_EXTRA_CPPFLAGS "-DRTOSID=${RTOS_ID}" CACHE STRING "DTS extra flags" FORCE)
if(${BOARD} STREQUAL "mimx8mm_evk_a53" OR ${BOARD} STREQUAL "mimx8mp_evk_a53")
  set(EXTRA_DTC_OVERLAY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/boards/mimx8m_evk_a53_${CONSOLE}.overlay" CACHE STRING "Extra DTC overlay file" FORCE)
else()
  set(EXTRA_DTC_OVERLAY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD}_${CONSOLE}.overlay" CACHE STRING "Extra DTC overlay file" FORCE)
endif()

if(${CONSOLE} STREQUAL "RAM_CONSOLE")
  set(EXTRA_CONF_FILE "${CMAKE_CURRENT_SOURCE_DIR}/RAM_CONSOLE.conf" CACHE STRING "Extra config file" FORCE)
endif()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hello_world)

set(MCUX_SDK_PROJECT_NAME app)

zephyr_compile_definitions(RTOSID=${RTOS_ID})

if(CONFIG_BOARD_MIMX8MM_EVK_A53)
  set(BoardName "evkmimx8mm_ca53")
elseif(CONFIG_BOARD_MIMX8MP_EVK_A53)
  set(BoardName "evkmimx8mp_ca53")
elseif(CONFIG_BOARD_MIMX93_EVK_A55)
  set(BoardName "mcimx93evk_ca55")
else()
  message(FATAL_ERROR "unsupported board")
endif()

set(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})
set(OsPath "${ProjDirPath}/../../../os")
set(HeaderPath "${ProjDirPath}/../../../include")
set(AppPath "${ProjDirPath}/..")
set(BoardPath "${ProjDirPath}/boards/${BoardName}")

zephyr_include_directories(
	${BoardPath}
	${HeaderPath}
	${OsPath}
	${ProjDirPath}
)

zephyr_compile_definitions(OS_ZEPHYR)

# Cortex-A55/A53 core maximum clock frequency
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX8MM_EVK_A53 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1800000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX8MN_EVK_A53 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1600000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX8MP_EVK_A53 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1800000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX93_EVK_A55 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1700000000UL)

target_sources(app PRIVATE main.c)
