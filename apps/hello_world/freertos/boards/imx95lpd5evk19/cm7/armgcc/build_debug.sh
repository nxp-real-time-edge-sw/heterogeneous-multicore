#!/bin/bash

hmc_root_path="../../../../../../../"
app_root_path="../../../../"

rm -rf debug
west sdk_build -p always ${app_root_path} -b imx95lpd5evk19 --config debug -Dcore_id=cm7 -d debug
