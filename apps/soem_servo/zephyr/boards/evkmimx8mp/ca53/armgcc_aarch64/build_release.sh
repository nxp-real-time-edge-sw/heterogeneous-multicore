#!/bin/sh

app_path=../../../../
build_type=Release

west build -p always -b imx8mp_evk/mimx8ml8/a53 ${app_path} -DCMAKE_BUILD_TYPE=${build_type} -d build_release
