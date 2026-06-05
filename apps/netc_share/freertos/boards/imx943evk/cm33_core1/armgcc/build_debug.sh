#!/bin/bash

app_root_path="../../../../"

rm -rf debug
west sdk_build -p always ${app_root_path} -b imx943evk --config debug -Dcore_id=cm33_core1 -d debug
