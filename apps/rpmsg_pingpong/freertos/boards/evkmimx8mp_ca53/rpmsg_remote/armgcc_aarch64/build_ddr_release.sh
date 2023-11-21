#!/bin/bash

# Define supported console
# By default, UART2 is used for uboot and Linux, UART4 is used for M-Core RTOS.
# Modify the console list according to usecase setup.
rtos_console_list=("RAM_CONSOLE" "UART4")

function clean() {
	if [ -d "CMakeFiles" ];then rm -rf CMakeFiles; fi
	if [ -f "Makefile" ];then rm -f Makefile; fi
	if [ -f "cmake_install.cmake" ];then rm -f cmake_install.cmake; fi
	if [ -f "CMakeCache.txt" ];then rm -f CMakeCache.txt; fi
}


# The remote side start up firstly, and make it use rtosid 0
rtosid=0

for each_console in ${rtos_console_list[*]}
do
	clean
	cmake -DCMAKE_TOOLCHAIN_FILE="../../../../../../../../mcux-sdk/tools/cmake_toolchain_files/armgcc_aarch64.cmake" -G "Unix Makefiles" -DRTOS_ID=${rtosid} -DCONSOLE=${each_console} -DCMAKE_BUILD_TYPE=ddr_release  .
	make -j $(nproc)
	if [ $? -ne 0 ]; then
		exit 1
	fi
done
