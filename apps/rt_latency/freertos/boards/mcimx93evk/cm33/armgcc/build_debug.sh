#!/bin/bash

hmc_root_path="../../../../../../../"
app_root_path="../../../../"

rm -rf debug
west sdk_build -p always ${app_root_path} -b mcimx93evk --config debug -Dcore_id=cm33 -d debug
