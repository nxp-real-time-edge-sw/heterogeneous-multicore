#Description: FreeRTOS specific implentation; user_visible: True
include_guard(GLOBAL)
message("common_freertos component is included.")


target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS_helper.c
    ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS_tick_config.c
    ${CMAKE_CURRENT_LIST_DIR}/cpu_load.c
    ${CMAKE_CURRENT_LIST_DIR}/idle.c
    ${CMAKE_CURRENT_LIST_DIR}/irq.c
    ${CMAKE_CURRENT_LIST_DIR}/../os/mqueue.c
    ${CMAKE_CURRENT_LIST_DIR}/../os/stdio.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/.
)

if ((NOT DEFINED CONSOLE) OR (NOT ${CONSOLE} STREQUAL "RAM_CONSOLE"))
include(driver_uart)
endif()
