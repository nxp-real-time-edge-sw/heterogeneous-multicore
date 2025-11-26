# CMake file for generating virtual switch library

# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)

set(LIBRARY_OUTPUT_PATH "${APPLICATION_BINARY_DIR}/virtual_switch_build")

set(LIBRARY_NAME virtual-switch)

set(HMC_SRC_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../../")
set(LIBRARY_SRC_PATH "${HMC_SRC_PATH}/net")
set(PTP_STACK_SRC_PATH "${HMC_SRC_PATH}/net/ptp_stack")
set(GenAVBPath "${SdkRootDirPath}/middleware/gen_avb")
set(AVB_APPS_PATH "${SdkRootDirPath}/../freertos_avb_apps")

add_library(${LIBRARY_NAME}
    ${PTP_STACK_SRC_PATH}/ptp_multisoc_stack.c
    ${PTP_STACK_SRC_PATH}/ptp_stack_freertos.c
    ${LIBRARY_SRC_PATH}/data_pkt.c
    ${LIBRARY_SRC_PATH}/net_switch.c
    ${LIBRARY_SRC_PATH}/port_software.c
    ${LIBRARY_SRC_PATH}/switch_freertos.c
    ${HMC_SRC_PATH}/os/freertos/os/mqueue.c
    ${HMC_SRC_PATH}/os/freertos/os/stdio.c
)

target_include_directories(${LIBRARY_NAME} PRIVATE
    ${LIBRARY_SRC_PATH}
    ${PTP_STACK_SRC_PATH}
    ${HMC_SRC_PATH}/include
    ${HMC_SRC_PATH}/include/net
    ${HMC_SRC_PATH}/os
    ${HMC_SRC_PATH}/os/freertos/Core_M
    ${SdkRootDirPath}/arch/arm/CMSIS/Core/Include
    ${SdkRootDirPath}/components/debug_console
    ${SdkRootDirPath}/devices/RT/RT1180/MIMXRT1189/drivers
    ${SdkRootDirPath}/devices/RT/RT1180/periph
    ${SdkRootDirPath}/devices/RT/RT1180/MIMXRT1189
    ${SdkRootDirPath}/components/serial_manager
    ${SdkRootDirPath}/components/uart
    ${SdkRootDirPath}/rtos/freertos/freertos-kernel/include
    ${SdkRootDirPath}/rtos/freertos/freertos-kernel/portable/GCC/ARM_CM33_NTZ/non_secure
    ${SdkRootDirPath}/drivers/common
    ${AVB_APPS_PATH}/devices/MIMXRT118x/common/cm33
    ${AVB_APPS_PATH}/boards/src/demo_apps
    ${AppCommonPath}/../../.
    ${GenAVBPath}/include
    ${GenAVBPath}/include/rtos
)
