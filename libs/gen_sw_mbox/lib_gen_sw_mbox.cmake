# Description: lib providing helper functions for Generic software mailbox

mcux_add_source(
    SOURCES gen_sw_mbox.c
)

mcux_add_include(
    INCLUDES .
             ../../include
             ../../os
)
