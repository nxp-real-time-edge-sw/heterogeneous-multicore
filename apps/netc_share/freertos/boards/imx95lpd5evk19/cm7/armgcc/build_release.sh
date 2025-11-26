#!/bin/bash

app_root_path="../../../../"

rm -rf release
west sdk_build -p always ${app_root_path} -b imx95lpd5evk19 --config release -Dcore_id=cm7 -d release
