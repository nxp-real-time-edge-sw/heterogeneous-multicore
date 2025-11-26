# Description: virtio core

mcux_add_source(
    SOURCES core.c
            mmio.c
)

mcux_add_include(
    INCLUDES .
             ../../include/virtio
             ../../include/virtio/uapi
)
