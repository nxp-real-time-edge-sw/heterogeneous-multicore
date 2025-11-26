#!/bin/bash

app_path=../../../../
build_type=Release

# Define supported console by each RTOS instance
# By default, LPUART1 is used for uboot and Linux, LPUART8 is used for M-Core RTOS.
# Modify the console list according to usecase setup.
console_matrix=(rtos0_console_list rtos1_console_list rtos2_console_list)
rtos0_console_list=("RAM_CONSOLE" "UART8")
rtos1_console_list=("RAM_CONSOLE")
rtos2_console_list=("RAM_CONSOLE" "UART1")

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
		output_dir=build_RTOS${each_rtos}_${each_console}
		if [ -d "${output_dir}" ];then rm -rf ./${output_dir}; fi
		west build -p always -b imx943_evk/mimx94398/a55 ${app_path} -DRTOS_ID=${each_rtos} -DCONSOLE=${each_console} -DCMAKE_BUILD_TYPE=${build_type} -d ${output_dir}
		if [ $? -ne 0 ]; then
			exit 1
		fi
		# append RAM Console buffer address to image name
		if [ $each_console = "RAM_CONSOLE" ]; then
			# parse RAM Console address from dts
			buf_addr=$(sed -n '/ram_console:/p' ${output_dir}/zephyr/zephyr.dts)
			buf_addr=${buf_addr##*@}
			buf_addr=0x${buf_addr%% *}
			console_name="RAM_CONSOLE-$buf_addr"
		else
			console_name=${each_console}
		fi
		ln -s hello_world.bin ${output_dir}/zephyr/hello_world_ca55_RTOS${each_rtos}_${console_name}.bin
		ln -s hello_world.elf ${output_dir}/zephyr/hello_world_ca55_RTOS${each_rtos}_${console_name}.elf
	done
done
