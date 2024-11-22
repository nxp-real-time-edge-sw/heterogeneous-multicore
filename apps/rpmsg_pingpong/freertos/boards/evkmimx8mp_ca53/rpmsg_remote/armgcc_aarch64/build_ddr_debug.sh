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
	cmake -DCMAKE_TOOLCHAIN_FILE="../../../../../../../../mcux-sdk/tools/cmake_toolchain_files/armgcc_aarch64.cmake" -G "Unix Makefiles" -DRTOS_ID=${rtosid} -DCONSOLE=${each_console} -DCMAKE_BUILD_TYPE=ddr_debug  .
	make -j $(nproc)
	if [ $? -ne 0 ]; then
		exit 1
	fi
	# append RAM Console buffer address to image name
	if [ $each_console = "RAM_CONSOLE" ]; then
		# use helper to get the buffer address from rtos_memory.h
		gcc -o  CMakeFiles/get_ram_console_addr -DRTOSID=${rtosid} -I../../../../../../../os/freertos/Core_AArch64/boards/evkmimx8mp/ ../../../../../../../tools/helper/get_ram_console_addr.c
		buf_addr=$(./CMakeFiles/get_ram_console_addr)
		mv ddr_debug/rpmsg_pingpong_remote_ca53_${each_console}.bin ddr_debug/rpmsg_pingpong_remote_ca53_${each_console}-${buf_addr}.bin
		mv ddr_debug/rpmsg_pingpong_remote_ca53_${each_console}.elf ddr_debug/rpmsg_pingpong_remote_ca53_${each_console}-${buf_addr}.elf
	fi
done
