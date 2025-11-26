# Description: virtio network backend

mcux_add_source(
    SOURCES network.c
)

mcux_add_include(
    INCLUDES .
)

include(${CMAKE_CURRENT_LIST_DIR}/lib_virtio.cmake)
