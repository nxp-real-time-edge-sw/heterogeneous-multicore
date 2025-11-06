# CMake file for generating virtual switch library

# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 3.1.1)

set(LIBRARY_OUTPUT_PATH ${VirtualSwitchBuildPath})

set(LIBRARY_NAME libvirtual-switch.a)

set(HMC_SRC_PATH "${CMAKE_CURRENT_LIST_DIR}/../../..")
set(LIBRARY_SRC_PATH "${HMC_SRC_PATH}/net")
set(PTP_STACK_SRC_PATH "${HMC_SRC_PATH}/net/ptp_stack")

add_library(${LIBRARY_NAME}
	${PTP_STACK_SRC_PATH}/ptp_multisoc_stack.c
	${PTP_STACK_SRC_PATH}/ptp_stack_freertos.c
	${LIBRARY_SRC_PATH}/data_pkt.c
	${LIBRARY_SRC_PATH}/net_switch.c
    ${LIBRARY_SRC_PATH}/port_software.c
	${LIBRARY_SRC_PATH}/switch_freertos.c
	${HMC_SRC_PATH}/os/freertos/os/mqueue.c
)

target_include_directories(${LIBRARY_NAME} PRIVATE
    ${LIBRARY_SRC_PATH}
    ${PTP_STACK_SRC_PATH}
    ${HMC_SRC_PATH}/include
    ${HMC_SRC_PATH}/include/net
    ${HMC_SRC_PATH}/os
    ${SdkDirPath}/CMSIS/Core/Include
    ${SdkDirPath}/devices/MIMXRT1189/utilities/debug_console
    ${SdkDirPath}/devices/MIMXRT1189/drivers
    ${SdkDirPath}/devices/MIMXRT1189/periph
    ${SdkDirPath}/devices/MIMXRT1189
    ${SdkDirPath}/components/serial_manager
    ${SdkDirPath}/components/uart
    ${SdkDirPath}/rtos/freertos/freertos-kernel/include
    ${SdkDirPath}/rtos/freertos/freertos-kernel/portable/GCC/ARM_CM33_NTZ/non_secure
    ${CommonPath}/cm33
    ${RTOS_APPS}/devices/MIMXRT118x/common/cm33
    ${AppCommonPath}/../../.
    ${GenAVBPath}/include
    ${GenAVBPath}/include/rtos
)
