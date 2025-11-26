#!/bin/bash

hmc_root_path="../../../../../../../"
app_root_path="../../../../"

rm -rf release
west sdk_build -p always ${app_root_path} -b evkmimx8mp --config release -Dcore_id=cm7 -d release
