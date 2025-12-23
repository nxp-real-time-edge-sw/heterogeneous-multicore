#!/bin/bash

app_path=../../../../
build_type=Release
BUILD_DIR=build_release

west build -p always -b imx95_evk/mimx9596/m7 ${app_path} -DCMAKE_BUILD_TYPE=${build_type} -d ${BUILD_DIR}
