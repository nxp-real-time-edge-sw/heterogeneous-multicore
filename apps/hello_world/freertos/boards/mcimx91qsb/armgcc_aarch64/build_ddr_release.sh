#!/bin/bash

target_type="ddr_release"
app_root_path="../../../"

rm -rf ${target_type}
west sdk_build -p always ${app_root_path} -b mcimx91qsb --config ${target_type} -d ${target_type}
