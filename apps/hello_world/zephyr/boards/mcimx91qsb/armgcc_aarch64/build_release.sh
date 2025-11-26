#!/bin/bash

app_path=../../../
build_type=Release

console="UART1"

west build -p always -b imx91_qsb/mimx9111 ${app_path} -DRTOS_ID=0 -DCONSOLE=${console} -DCMAKE_BUILD_TYPE=${build_type} -d build_RTOS0_${console}
