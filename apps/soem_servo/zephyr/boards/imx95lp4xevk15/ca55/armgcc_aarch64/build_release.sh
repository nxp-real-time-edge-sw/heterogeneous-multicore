#!/bin/sh

app_path=../../../../
build_type=Release

west build -p always -b imx95_evk_15x15/mimx9596/a55 ${app_path} -DCMAKE_BUILD_TYPE=${build_type} -d build_release
