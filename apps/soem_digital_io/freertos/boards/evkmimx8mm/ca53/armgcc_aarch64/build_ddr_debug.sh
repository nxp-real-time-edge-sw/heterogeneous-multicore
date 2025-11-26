#!/bin/bash

target_type="ddr_debug"
app_root_path="../../../../"

rm -rf ${target_type}
west sdk_build -p always ${app_root_path} -b evkmimx8mm --config ${target_type} -Dcore_id=ca53 -d ${target_type}
