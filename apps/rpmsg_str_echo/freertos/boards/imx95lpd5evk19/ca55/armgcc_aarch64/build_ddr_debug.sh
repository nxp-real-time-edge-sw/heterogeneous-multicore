#!/bin/bash

build_target=ddr_debug
board=imx95lpd5evk19
cpu=ca55

hmc_root_path="../../../../../../../"
app_root_path="../../../../"

# Define supported console by each RTOS instance
# By default, LPUART1 is used for uboot and Linux, LPUART3 is used for M7-Core RTOS.
# Modify the console list according to usecase setup.
console_matrix=(rtos0_console_list rtos1_console_list)
rtos0_console_list=("RAM_CONSOLE" "UART3")
rtos1_console_list=("RAM_CONSOLE" "UART1")

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
		image_name="rpmsg_str_echo_${cpu}_RTOS${each_rtos}_${each_console}"
		# append RAM Console buffer address to image name
		if [ $each_console = "RAM_CONSOLE" ]; then
			# use helper to get the buffer address from rtos_memory.h
			gcc -o  ${output_dir}/get_ram_console_addr -DRTOSID=${each_rtos} -I${hmc_root_path}/os/freertos/Core_AArch64/boards/${board}/ ${hmc_root_path}/tools/helper/get_ram_console_addr.c
			buf_addr=$(./${output_dir}/get_ram_console_addr)
			image_name=${image_name}-${buf_addr}
		fi
		ln -s rpmsg_str_echo_${cpu}.bin ${output_dir}/${image_name}.bin
		ln -s rpmsg_str_echo_${cpu}.elf ${output_dir}/${image_name}.elf
	done
done
