# Description: virtio trans backend
include_guard(GLOBAL)
message("lib_virtio_trans component is included.")

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group ${CMAKE_CURRENT_LIST_DIR}/virtio_trans_${MCUX_DEVICE}.lib)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/virtio
    ${CMAKE_CURRENT_LIST_DIR}/../../include/virtio/uapi
)
