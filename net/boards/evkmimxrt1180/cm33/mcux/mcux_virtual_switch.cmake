if(CONFIG_MCUX_PRJSEG_module.hmc.virtual_sw_lib)
    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}/middleware/heterogeneous-multicore
        SOURCES
        include/net/*.h
        net/lib_port_genavb.h
        net/lib_virtual_switch.h
    )

    mcux_add_include(
        BASE_PATH ${SdkRootDirPath}/middleware/heterogeneous-multicore
        INCLUDES
        include/net
        net
    )

    set(VirtualSwitchBuildPath "${APPLICATION_BINARY_DIR}/virtual_switch_build")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../" "${VirtualSwitchBuildPath}/")
    mcux_add_library(
        BASE_PATH ${VirtualSwitchBuildPath}
        LIBS "libvirtual-switch.a"
        TOOLCHAINS armgcc iar mcux
    )

endif()
