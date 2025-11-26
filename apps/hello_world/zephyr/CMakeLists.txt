cmake_minimum_required(VERSION 3.13.1)

if(NOT DEFINED RTOS_ID)
  set(RTOS_ID "0")
endif()

if(NOT DEFINED CONSOLE)
  set(CONSOLE "default")
endif()

message("Building RTOS${RTOS_ID} with ${CONSOLE} Console ...")

if(NOT ${CONSOLE} STREQUAL "default")
  set(DTS_EXTRA_CPPFLAGS "-DRTOSID=${RTOS_ID}" CACHE STRING "DTS extra flags" FORCE)
  if(${BOARD} STREQUAL "imx8mm_evk/mimx8mm6/a53" OR ${BOARD} STREQUAL "imx8mp_evk/mimx8ml8/a53" OR ${BOARD} STREQUAL "imx8mp_evk/mimx8ml8/a53/smp")
    set(EXTRA_DTC_OVERLAY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/boards/imx8m_evk_a53_${CONSOLE}.overlay" CACHE STRING "Extra DTC overlay file" FORCE)
  elseif(${BOARD} STREQUAL "imx93_evk/mimx9352/a55" OR ${BOARD} STREQUAL "imx93_evk/mimx9352/a55/smp")
    set(EXTRA_DTC_OVERLAY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/boards/imx93_evk_a55_${CONSOLE}.overlay" CACHE STRING "Extra DTC overlay file" FORCE)
  elseif(${BOARD} STREQUAL "imx943_evk/mimx94398/a55")
    set(EXTRA_DTC_OVERLAY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/boards/imx943_evk_a55_${CONSOLE}.overlay" CACHE STRING "Extra DTC overlay file" FORCE)
  elseif(${BOARD} STREQUAL "imx95_evk/mimx9596/a55" OR ${BOARD} STREQUAL "imx95_evk/mimx9596/a55/smp")
    set(EXTRA_DTC_OVERLAY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/boards/imx95_evk_a55_${CONSOLE}.overlay" CACHE STRING "Extra DTC overlay file" FORCE)
  elseif(${BOARD} STREQUAL "imx91_qsb/mimx9111")
  elseif(${BOARD} STREQUAL "imx91_evk/mimx9131")
  else()
    message(FATAL_ERROR "unsupported board")
  endif()
endif()

if(${CONSOLE} STREQUAL "RAM_CONSOLE")
  set(EXTRA_CONF_FILE "${CMAKE_CURRENT_SOURCE_DIR}/RAM_CONSOLE.conf" CACHE STRING "Extra config file" FORCE)
endif()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hello_world)

set(MCUX_SDK_PROJECT_NAME app)

zephyr_compile_definitions(RTOSID=${RTOS_ID})

if(CONFIG_BOARD_IMX8MM_EVK_MIMX8MM6_A53)
  set(BoardName "evkmimx8mm/ca53")
elseif(CONFIG_BOARD_IMX8MP_EVK_MIMX8ML8_A53 OR CONFIG_BOARD_IMX8MP_EVK_MIMX8ML8_A53_SMP)
  set(BoardName "evkmimx8mp/ca53")
elseif(CONFIG_BOARD_IMX91_QSB_MIMX9111)
  set(BoardName "mcimx91qsb")
elseif(CONFIG_BOARD_IMX91_EVK_MIMX9131)
  set(BoardName "mcimx91evk")
elseif(CONFIG_BOARD_IMX93_EVK_MIMX9352_A55 OR CONFIG_BOARD_IMX93_EVK_MIMX9352_A55_SMP)
  set(BoardName "mcimx93evk/ca55")
elseif(CONFIG_BOARD_IMX943_EVK_MIMX94398_A55)
  set(BoardName "imx943evk/ca55")
elseif(CONFIG_BOARD_IMX95_EVK_MIMX9596_A55 OR CONFIG_BOARD_IMX95_EVK_MIMX9596_A55_SMP)
  set(BoardName "imx95lpd5evk19/ca55")
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
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX91_QSB SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1400000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX91_EVK SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1400000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX93_EVK_A55 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1700000000UL)
zephyr_compile_definitions_ifdef(CONFIG_BOARD_MIMX95_EVK_A55 SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY=1800000000UL)

target_sources(app PRIVATE main.c)
