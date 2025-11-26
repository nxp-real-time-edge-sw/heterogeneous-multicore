# Description: Generic Software Mailbox

mcux_add_source(
    SOURCES generic_software_mailbox.c
)

mcux_add_include(
    INCLUDES .
             ../../include/mailbox
)
include(${CMAKE_CURRENT_LIST_DIR}/../../libs/gen_sw_mbox/lib_gen_sw_mbox.cmake)
