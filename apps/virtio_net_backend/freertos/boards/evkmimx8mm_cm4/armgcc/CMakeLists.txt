# SOC DEVICE SETTING
SET(MCUX_DEVICE "MIMX8MM6")

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
SET(VirtioDevPath "${ProjRootPath}/virtio/devices")
SET(IncludePath "${ProjRootPath}/include")
SET(MailboxPath "${ProjRootPath}/virtio/mailbox")
SET(OsBoardPath "${ProjRootPath}/os/freertos/Core_M/boards/evkmimx8mm_cm4")
SET(PlatPortsPath "${ProjRootPath}/virtio/plat_ports")
SET(SdkDirPath "${ProjRootPath}/../mcux-sdk")
SET(RtosPath "${ProjRootPath}/../FreeRTOS-Kernel")
SET(OsPath "${ProjRootPath}/os")
SET(LibsPath "${ProjRootPath}/libs")
SET(NetPath "${ProjRootPath}/net")
SET(AppPath "${ProjRootPath}/apps/virtio_net_backend")

SET(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})
SET(LIBRARY_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})


project(virtio_net_backend_cm4)

set(MCUX_SDK_PROJECT_NAME virtio_net_backend_cm4.elf)

include(${OsBoardPath}/armgcc/flags.cmake)

include(${ProjDirPath}/config.cmake)

add_executable(${MCUX_SDK_PROJECT_NAME}
"${AppPath}/freertos/main.c"
"${ProjDirPath}/../virtio_board.c"
"${ProjDirPath}/../pin_mux.c"
"${ProjDirPath}/../pin_mux.h"
"${ProjDirPath}/../board.c"
"${ProjDirPath}/../board.h"
"${ProjDirPath}/../clock_config.c"
"${ProjDirPath}/../clock_config.h"
"${ProjDirPath}/../fsl_iomuxc.h"
"${OsPath}/freertos/os/mqueue.c"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${IncludePath}
    ${ProjDirPath}/..
    ${OsPath}
    ${OsBoardPath}
)

set(CMAKE_MODULE_PATH
    ${OsPath}/freertos/Core_M
    ${PlatPortsPath}
    ${MailboxPath}
    ${NetPath}
    ${RtosPath}
    ${SdkDirPath}
    ${VirtioDevPath}
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

include(driver_enet)
include(driver_phy-common)
include(driver_phy-device-ar8031)

include(lib_net_switch)
include(lib_port_enet_freertos)
include(lib_switch_hal_freertos)

include(lib_virtio_net)

include(mu_mailbox)
include(plat_ports_cm)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE m)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE c)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE gcc)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE nosys)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY}
-Obinary ${EXECUTABLE_OUTPUT_PATH}/${MCUX_SDK_PROJECT_NAME} ${EXECUTABLE_OUTPUT_PATH}/virtio_net_backend_cm4.bin)
