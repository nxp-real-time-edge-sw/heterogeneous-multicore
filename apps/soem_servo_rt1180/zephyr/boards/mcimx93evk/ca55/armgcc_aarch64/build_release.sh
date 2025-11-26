#!/bin/sh

app_path=../../../../
build_type=Release

west build -p always -b imx93_evk/mimx9352/a55 ${app_path} -DCMAKE_BUILD_TYPE=${build_type} -d build_release
