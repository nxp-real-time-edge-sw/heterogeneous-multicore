#!/bin/sh

app_path=../../../
build_type=Release

west build -p always -b imx91_qsb/mimx9111 ${app_path} -DCMAKE_BUILD_TYPE=${build_type} -d build_release
