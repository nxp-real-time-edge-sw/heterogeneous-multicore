#!/bin/bash

# Define supported console by each RTOS instance
# By default, LPUART1 is used for uboot and Linux, LPUART3 is used for M7-Core RTOS.
# Modify the console list according to usecase setup.
console_matrix=(rtos0_console_list rtos1_console_list rtos2_console_list)
rtos0_console_list=("RAM_CONSOLE" "UART3")
rtos1_console_list=("RAM_CONSOLE")
rtos2_console_list=("RAM_CONSOLE" "UART1")

function clean() {
	if [ -d "CMakeFiles" ];then rm -rf CMakeFiles; fi
	if [ -f "Makefile" ];then rm -f Makefile; fi
	if [ -f "cmake_install.cmake" ];then rm -f cmake_install.cmake; fi
	if [ -f "CMakeCache.txt" ];then rm -f CMakeCache.txt; fi
}

if [ "$#" -eq 1 ]; then
	case "$1" in
		RTOS0)
		rtosid=(0)
		;;
		RTOS1)
		rtosid=(1)
		;;
		RTOS2)
		rtosid=(2)
		;;
		all)
		rtosid=(0 1 2)
		;;
		*)
			echo "invalid RTOS ID specified"
			exit 1
		;;
	esac
else
		rtosid=(0 1 2)
fi

for each_rtos in ${rtosid[*]}
do
	TMPA=${console_matrix[${each_rtos}]}
	TMPB=$TMPA[@]
	TMPC=(${!TMPB})
	for each_console in ${TMPC[@]}
	do
		clean
		cmake -DCMAKE_TOOLCHAIN_FILE="../../../../../../../mcux-sdk/tools/cmake_toolchain_files/armgcc_aarch64.cmake" -G "Unix Makefiles" -DRTOS_ID=${each_rtos} -DCONSOLE=${each_console} -DCMAKE_BUILD_TYPE=ddr_release  .
		make -j $(nproc)
		if [ $? -ne 0 ]; then
			exit 1
		fi
		# append RAM Console buffer address to image name
		if [ $each_console = "RAM_CONSOLE" ]; then
			# use helper to get the buffer address from rtos_memory.h
			gcc -o  CMakeFiles/get_ram_console_addr -DRTOSID=${each_rtos} -I../../../../../../os/freertos/Core_AArch64/boards/mcimx95evk/ ../../../../../../tools/helper/get_ram_console_addr.c
			buf_addr=$(./CMakeFiles/get_ram_console_addr)
			mv ddr_release/hello_world_ca55_RTOS${each_rtos}_${each_console}.bin ddr_release/hello_world_ca55_RTOS${each_rtos}_${each_console}-${buf_addr}.bin
			mv ddr_release/hello_world_ca55_RTOS${each_rtos}_${each_console}.elf ddr_release/hello_world_ca55_RTOS${each_rtos}_${each_console}-${buf_addr}.elf
		fi
	done
done