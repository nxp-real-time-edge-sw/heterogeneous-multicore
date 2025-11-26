#!/bin/bash

build_type="ddr_release"
app_root_path="../../../../"

rm -rf ${build_type}
west sdk_build -p always ${app_root_path} -b evkmimx8mm --config ${build_type} -Dcore_id=ca53 -d ${build_type}
