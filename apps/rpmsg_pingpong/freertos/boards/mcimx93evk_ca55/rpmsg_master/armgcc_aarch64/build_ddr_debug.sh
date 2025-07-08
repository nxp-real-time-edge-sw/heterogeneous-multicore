#!/bin/bash

# Define supported console
# By default, UART1 is used for uboot and Linux, UART2 is used for M-Core RTOS.
# Modify the console list according to usecase setup.
rtos_console_list=("UART1")

function clean() {
	if [ -d "CMakeFiles" ];then rm -rf CMakeFiles; fi
	if [ -f "Makefile" ];then rm -f Makefile; fi
	if [ -f "cmake_install.cmake" ];then rm -f cmake_install.cmake; fi
	if [ -f "CMakeCache.txt" ];then rm -f CMakeCache.txt; fi
}

# The master side start up secondly, and make it use rtosid 1
rtosid=1

for each_console in ${rtos_console_list[*]}
do
	clean
	cmake -DCMAKE_TOOLCHAIN_FILE="../../../../../../../../mcux-sdk/tools/cmake_toolchain_files/armgcc_aarch64.cmake" -G "Unix Makefiles" -DRTOS_ID=${rtosid} -DCONSOLE=${each_console} -DCMAKE_BUILD_TYPE=ddr_debug  .
	make -j $(nproc)
	if [ $? -ne 0 ]; then
		exit 1
	fi
done
