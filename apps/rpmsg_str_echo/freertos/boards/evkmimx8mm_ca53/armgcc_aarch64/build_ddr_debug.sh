#!/bin/bash

# Define supported console by each RTOS instance
# By default, UART2 is used for uboot and Linux, UART4 is used for M-Core RTOS.
# Modify the console list according to usecase setup.
console_matrix=(rtos0_console_list rtos1_console_list)
rtos0_console_list=("RAM_CONSOLE" "UART4")
rtos1_console_list=("RAM_CONSOLE")

function clean() {
	if [ -d "CMakeFiles" ];then rm -rf CMakeFiles; fi
	if [ -f "Makefile" ];then rm -f Makefile; fi
	if [ -f "cmake_install.cmake" ];then rm -f cmake_install.cmake; fi
	if [ -f "CMakeCache.txt" ];then rm -f CMakeCache.txt; fi
}

# The demo supports to run up to 2 RTOS on Cortex-A Core.
if [ "$#" -eq 1 ]; then
	case "$1" in
		RTOS0)
		rtosid=(0)
		;;
		RTOS1)
		rtosid=(1)
		;;
		all)
		rtosid=(0 1)
		;;
		*)
			echo "invalid RTOS ID specified"
			exit 1
		;;
	esac
else
		rtosid=(0 1)
fi

for each_rtos in ${rtosid[*]}
do
	TMPA=${console_matrix[${each_rtos}]}
	TMPB=$TMPA[@]
	TMPC=(${!TMPB})
	for each_console in ${TMPC[@]}
	do
		clean
		cmake -DCMAKE_TOOLCHAIN_FILE="../../../../../../../mcux-sdk/tools/cmake_toolchain_files/armgcc_aarch64.cmake" -G "Unix Makefiles" -DRTOS_ID=${each_rtos} -DCONSOLE=${each_console} -DCMAKE_BUILD_TYPE=ddr_debug  .
		make -j $(nproc)
		if [ $? -ne 0 ]; then
			exit 1
		fi
		# append RAM Console buffer address to image name
		if [ $each_console = "RAM_CONSOLE" ]; then
			# use helper to get the buffer address from rtos_memory.h
			gcc -o  CMakeFiles/get_ram_console_addr -DRTOSID=${each_rtos} -I../../../../../../os/freertos/Core_AArch64/boards/evkmimx8mm/ ../../../../../../tools/helper/get_ram_console_addr.c
			buf_addr=$(./CMakeFiles/get_ram_console_addr)
			mv ddr_debug/rpmsg_str_echo_ca53_RTOS${each_rtos}_${each_console}.bin ddr_debug/rpmsg_str_echo_ca53_RTOS${each_rtos}_${each_console}-${buf_addr}.bin
			mv ddr_debug/rpmsg_str_echo_ca53_RTOS${each_rtos}_${each_console}.elf ddr_debug/rpmsg_str_echo_ca53_RTOS${each_rtos}_${each_console}-${buf_addr}.elf
		fi
	done
done
