#!/bin/sh

app_path=../../../
build_type=Release

west build -p always -b imx91_evk/mimx9131 ${app_path} -DCMAKE_BUILD_TYPE=${build_type} -d build_release
