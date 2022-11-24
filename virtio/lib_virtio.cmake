# Description: virtio core
include_guard(GLOBAL)
message("lib_virtio component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/core.c
    ${CMAKE_CURRENT_LIST_DIR}/mmio.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/.
    ${CMAKE_CURRENT_LIST_DIR}/../include/virtio
    ${CMAKE_CURRENT_LIST_DIR}/../include/virtio/uapi
)
