# Description: Generic Software Mailbox
include_guard(GLOBAL)
message("generic_software_mailbox is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/generic_software_mailbox.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/.
    ${CMAKE_CURRENT_LIST_DIR}/../../include/mailbox
)

include(lib_gen_sw_mbox)
