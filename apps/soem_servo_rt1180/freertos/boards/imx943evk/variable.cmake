# Copyright 2025 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

mcux_set_variable(board imx943evk)

if (NOT DEFINED device)
    mcux_set_variable(device MIMX94398)
endif()

mcux_set_variable(soc_series i.MX943)

include(${SdkRootDirPath}/devices/i.MX/${soc_series}/${device}/variable.cmake)
