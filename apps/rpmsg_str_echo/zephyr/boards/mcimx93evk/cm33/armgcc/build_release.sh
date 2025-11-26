#!/bin/bash

app_path=../../../../
build_type=Release
BUILD_DIR=build_release

west build -p always -b imx93_evk/mimx9352/m33 ${app_path} -DCMAKE_BUILD_TYPE=${build_type} -d ${BUILD_DIR}

ln -s rpmsg_str_echo.bin ${BUILD_DIR}/zephyr/rpmsg_str_echo_cm33.bin
ln -s rpmsg_str_echo.elf ${BUILD_DIR}/zephyr/rpmsg_str_echo_cm33.elf
