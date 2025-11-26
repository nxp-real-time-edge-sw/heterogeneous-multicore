# Description: virtio performance test backend

mcux_add_source(
    SOURCES trans.c
)

mcux_add_include(
    INCLUDES .
)

include(${CMAKE_CURRENT_LIST_DIR}/lib_virtio.cmake)
