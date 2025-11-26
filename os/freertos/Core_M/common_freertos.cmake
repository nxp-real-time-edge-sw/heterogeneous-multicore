#Description: FreeRTOS specific implentation; user_visible: True

mcux_add_source(
    SOURCES
    FreeRTOS_helper.c
    ../os/mqueue.c
    ../os/stdio.c
    cpu_load.c
)

mcux_add_include(
    INCLUDES
    .
)
