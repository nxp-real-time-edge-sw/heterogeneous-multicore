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

    mcux_add_library(
        BASE_PATH ${SdkRootDirPath}/gen_avb-libs
        LIBS "libvirtual-switch.a"
        TOOLCHAINS armgcc iar mcux
    )
endif()
