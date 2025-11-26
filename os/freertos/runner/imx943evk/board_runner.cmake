# Copyright 2025 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

if(CONFIG_CPU_CORTEX_A)
    board_runner_args(jlink "--device=MIMX94398_A55_0" "--no-reset" "--flash-sram")
    include(${SdkRootDirPath}/cmake/extension/runner/jlink.board.cmake)
endif()
