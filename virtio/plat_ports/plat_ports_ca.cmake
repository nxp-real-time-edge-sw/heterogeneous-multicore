# Description: platform support for Cortex-A

mcux_add_source(
    SOURCES isr_ca.c
)

mcux_add_include(
    INCLUDES .
             ../../include/plat_ports
)
