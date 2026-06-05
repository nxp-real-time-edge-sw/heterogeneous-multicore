if (CONFIG_MCUX_COMPONENT_device.startup)
   mcux_add_armgcc_configuration(
        AS "-D__STARTUP_INITIALIZE_NONCACHEDATA"
    )
endif()

if (CONFIG_MCUX_PRJSEG_module.board.suite)
    mcux_add_configuration(
        CC "-DCPU_MIMX94398AVKM_cm33_core1"
    )
endif()
