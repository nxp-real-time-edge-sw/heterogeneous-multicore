# Copyright 2025 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

if(CONFIG_CPU_CORTEX_A)
    board_runner_args(jlink "--device=MIMX9596_A55_0" "--no-reset" "--flash-sram")
    include(${SdkRootDirPath}/cmake/extension/runner/jlink.board.cmake)
endif()

if(CONFIG_CPU_CORTEX_M)
    if(CONFIG_MCUX_HW_CORE_ID_CM7)
        board_runner_args(jlink "--device=MIMX9596_M7")
    elseif(CONFIG_MCUX_HW_CORE_ID_CM33)
        board_runner_args(jlink "--device=MIMX9596_M33")
    endif()
    board_runner_args(linkserver  "--device=MIMX9596:MCIMX95-EVK")
    board_runner_args(pyocd "--target=mimx9596")

    include(${SdkRootDirPath}/cmake/extension/runner/linkserver.board.cmake)
    include(${SdkRootDirPath}/cmake/extension/runner/pyocd.board.cmake)
    include(${SdkRootDirPath}/cmake/extension/runner/jlink.board.cmake)
    include(${SdkRootDirPath}/cmake/extension/runner/openocd.board.cmake)
    include(${SdkRootDirPath}/cmake/extension/runner/canopen.board.cmake)
endif()
