#!/bin/bash

# Define supported console by each RTOS instance
# By default, UART2 is used for uboot and Linux, UART4 is used for M-Core RTOS.
# Modify the console list according to usecase setup.

build_target=ddr_release
board=evkmimx8mm
cpu=ca53

console_matrix=(rtos0_console_list rtos1_console_list rtos2_console_list rtos3_console_list)
rtos0_console_list=("RAM_CONSOLE" "UART4")
rtos1_console_list=("RAM_CONSOLE")
rtos2_console_list=("RAM_CONSOLE")
rtos3_console_list=("RAM_CONSOLE" "UART2")

hmc_root_path="../../../../../../../"
app_root_path="../../../../"

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
		RTOS3)
		rtosid=(3)
		;;
		all)
		rtosid=(0 1 2 3)
		;;
		*)
			echo "invalid RTOS ID specified"
			exit 1
		;;
	esac
else
		rtosid=(0 1 2 3)
fi

for each_rtos in ${rtosid[*]}
do
	TMPA=${console_matrix[${each_rtos}]}
	TMPB=$TMPA[@]
	TMPC=(${!TMPB})
	for each_console in ${TMPC[@]}
	do
		output_dir=${build_target}_rtos${each_rtos}_${each_console}
		rm -rf ${output_dir}
		mkdir ${output_dir}

		# use helper to get the rtos SRAM base address from rtos_memory.h, then set CONFIG_SRAM_BASE_ADDRESS which is used by "west flash"
		gcc -o  ${output_dir}/get_rtos_ram_base -DRTOSID=${each_rtos} -I${hmc_root_path}/os/freertos/Core_AArch64/boards/${board}/ ${hmc_root_path}/tools/helper/get_rtos_ram_base.c
		sram_addr=$(./${output_dir}/get_rtos_ram_base)

		west sdk_build -p always ${app_root_path} -b ${board} --config ${build_target} -Dcore_id=${cpu} -DCONSOLE=${each_console} -DRTOS_ID=${each_rtos} -d ${output_dir} -DCONFIG_SRAM_BASE_ADDRESS=${sram_addr}
		if [ $? -ne 0 ]; then
			exit 1
		fi

		# append console name to image name
		image_name="hello_world_${cpu}_RTOS${each_rtos}_${each_console}"
		# append RAM Console buffer address to image name
		if [ $each_console = "RAM_CONSOLE" ]; then
			# use helper to get the buffer address from rtos_memory.h
			gcc -o  ${output_dir}/get_ram_console_addr -DRTOSID=${each_rtos} -I${hmc_root_path}/os/freertos/Core_AArch64/boards/${board}/ ${hmc_root_path}/tools/helper/get_ram_console_addr.c
			buf_addr=$(./${output_dir}/get_ram_console_addr)
			image_name=${image_name}-${buf_addr}
		fi
		ln -s hello_world_${cpu}.bin ${output_dir}/${image_name}.bin
		ln -s hello_world_${cpu}.elf ${output_dir}/${image_name}.elf
	done
done
