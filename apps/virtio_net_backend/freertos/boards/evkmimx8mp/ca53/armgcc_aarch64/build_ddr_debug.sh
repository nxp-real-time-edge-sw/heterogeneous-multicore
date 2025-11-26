#!/bin/bash

build_type="ddr_debug"
app_root_path="../../../../"

rm -rf ${build_type}
west sdk_build -p always ${app_root_path} -b evkmimx8mp --config ${build_type} -Dcore_id=ca53 -d ${build_type}
