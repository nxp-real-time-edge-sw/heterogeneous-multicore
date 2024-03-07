# Description: virtio network backend
include_guard(GLOBAL)
message("lib_virtio_net component is included.")

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group ${CMAKE_CURRENT_LIST_DIR}/virtio_net_${MCUX_DEVICE}.lib)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/virtio
    ${CMAKE_CURRENT_LIST_DIR}/../../include/virtio/uapi
)
