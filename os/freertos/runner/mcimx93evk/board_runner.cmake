# Copyright 2025 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

if(CONFIG_CPU_CORTEX_A)
    board_runner_args(jlink "--device=MIMX9352_A55_0" "--no-reset" "--flash-sram")
    include(${SdkRootDirPath}/cmake/extension/runner/jlink.board.cmake)
endif()

if(CONFIG_CPU_CORTEX_M)
    board_runner_args(jlink "--device=MIMX9352_M33")
    board_runner_args(linkserver  "--device=MIMX9352:MCIMX93-EVK")
    board_runner_args(pyocd "--target=mimx9352")

    include(${SdkRootDirPath}/cmake/extension/runner/linkserver.board.cmake)
    include(${SdkRootDirPath}/cmake/extension/runner/pyocd.board.cmake)
    include(${SdkRootDirPath}/cmake/extension/runner/jlink.board.cmake)
    include(${SdkRootDirPath}/cmake/extension/runner/openocd.board.cmake)
    include(${SdkRootDirPath}/cmake/extension/runner/canopen.board.cmake)
endif()
