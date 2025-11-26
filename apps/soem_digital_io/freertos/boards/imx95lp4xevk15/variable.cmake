# Copyright 2025 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

mcux_set_variable(board imx95lp4xevk15)

if (NOT DEFINED device)
    mcux_set_variable(device MIMX9596)
endif()

mcux_set_variable(soc_series i.MX95)

include(${SdkRootDirPath}/devices/i.MX/${soc_series}/${device}/variable.cmake)
