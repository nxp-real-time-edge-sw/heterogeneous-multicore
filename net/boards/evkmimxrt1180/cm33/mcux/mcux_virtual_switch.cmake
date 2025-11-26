if(CONFIG_MCUX_PRJSEG_module.hmc.virtual_sw_lib)
    set(VirtualSwitchBuildPath "${APPLICATION_BINARY_DIR}/virtual_switch_build")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../" "${VirtualSwitchBuildPath}/")
    mcux_add_library(
        BASE_PATH ${VirtualSwitchBuildPath}
        LIBS "libvirtual-switch.a"
        TOOLCHAINS armgcc iar mcux
    )
endif()
