#!/bin/bash

hmc_root_path="../../../../../../../"
app_root_path="../../../../"

rm -rf debug
west sdk_build -p always ${app_root_path} -b evkmimx8mm --config debug -Dcore_id=cm4 -d debug
