#!/bin/bash

app_path=../../../../

west build -p always -b imx95_evk/mimx9596/a55 ${app_path} -d build_release -DCONFIG_COVERAGE=y -DCONFIG_COVERAGE_DUMP=y -DCONFIG_LOG_PRINTK=n
