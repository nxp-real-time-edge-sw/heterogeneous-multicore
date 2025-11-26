# Copyright 2025 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

if(CONFIG_CPU_CORTEX_A)
    board_runner_args(jlink "--device=MIMX8ML8_A53_0" "--no-reset" "--flash-sram")
    include(${SdkRootDirPath}/cmake/extension/runner/jlink.board.cmake)
endif()

if(CONFIG_CPU_CORTEX_M)
    board_runner_args(jlink "--device=MIMX8ML8_M7")
    board_runner_args(linkserver  "--device=MIMX8ML8:EVK-MIMX8MP")
    board_runner_args(pyocd "--target=mimx8ml8")

    include(${SdkRootDirPath}/cmake/extension/runner/linkserver.board.cmake)
    include(${SdkRootDirPath}/cmake/extension/runner/pyocd.board.cmake)
    include(${SdkRootDirPath}/cmake/extension/runner/jlink.board.cmake)
    include(${SdkRootDirPath}/cmake/extension/runner/openocd.board.cmake)
    include(${SdkRootDirPath}/cmake/extension/runner/canopen.board.cmake)
endif()
