# Copyright 2025 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

mcux_set_variable(board mcimx91qsb)

if (NOT DEFINED device)
    mcux_set_variable(device MIMX9111)
endif()

include(${SdkRootDirPath}/devices/i.MX/i.MX91/${device}/variable.cmake)
