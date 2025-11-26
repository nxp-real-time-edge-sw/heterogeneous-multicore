# Description: platform support for Cortex-M

mcux_add_source(
    SOURCES isr_cm.c
)

mcux_add_include(
    INCLUDES .
             ../../include/plat_ports
)
