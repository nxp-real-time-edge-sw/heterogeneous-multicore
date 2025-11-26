#Description: FreeRTOS specific implentation; user_visible: True

mcux_add_source(
    SOURCES
    FreeRTOS_helper.c
    FreeRTOS_tick_config.c
    cpu_load.c
    idle.c
    irq.c
    ../os/mqueue.c
    ../os/stdio.c
)

mcux_add_include(
    INCLUDES
    .
)

if ((NOT DEFINED CONSOLE) OR (NOT ${CONSOLE} STREQUAL "RAM_CONSOLE"))
mcux_add_source(
    SOURCES uart.c
)
endif()
