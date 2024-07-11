#Description: FreeRTOS specific implentation; user_visible: True
include_guard(GLOBAL)
message("common_freertos component is included.")


target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS_helper.c
    ${CMAKE_CURRENT_LIST_DIR}/../os/mqueue.c
    ${CMAKE_CURRENT_LIST_DIR}/../os/stdio.c
    ${CMAKE_CURRENT_LIST_DIR}/cpu_load.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/.
)
