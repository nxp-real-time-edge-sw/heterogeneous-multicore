#!/bin/sh

app_path=../../../../
build_type=Release

west build -p always -b imx8mm_evk/mimx8mm6/a53 ${app_path} -DCMAKE_BUILD_TYPE=${build_type} -d build_release
