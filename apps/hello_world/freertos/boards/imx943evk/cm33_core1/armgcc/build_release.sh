#!/bin/bash

hmc_root_path="../../../../../../../"
app_root_path="../../../../"

rm -rf release
west sdk_build -p always ${app_root_path} -b imx943evk --config release -Dcore_id=cm33_core1 -d release
