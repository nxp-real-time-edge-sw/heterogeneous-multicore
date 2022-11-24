# Description: net switch drivers
include_guard(GLOBAL)
message("lib_net_switch component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/data_pkt.c
    ${CMAKE_CURRENT_LIST_DIR}/net_switch.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/.
)
