# Description: lib providing helper functions for log library
include_guard(GLOBAL)
message("lib_log component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/log.c
    ${CMAKE_CURRENT_LIST_DIR}/sdk_printf.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/.
)
