#Description: FreeRTOS specific implentation; user_visible: True
include_guard(GLOBAL)
message("TPM counter driver component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/os/counter.c
    ${CMAKE_CURRENT_LIST_DIR}/os/counter_tpm.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${SdkDirPath}/drivers/tpm
)
