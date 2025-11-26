#!/bin/bash

app_path=../../../../
build_type=Release

# Define supported console by each RTOS instance
# By default, LPUART1 is used for uboot and Linux, LPUART2 is used for M-Core RTOS.
# Modify the console list according to usecase setup.
console_matrix=(rtos0_console_list)
rtos0_console_list=("RAM_CONSOLE" "UART2")
rtosid=(0)

for each_rtos in ${rtosid[*]}
do
	TMPA=${console_matrix[${each_rtos}]}
	TMPB=$TMPA[@]
	TMPC=(${!TMPB})
	for each_console in ${TMPC[@]}
	do
		BUILD_DIR="build_RTOS${each_rtos}_${each_console}"
		if [ -d "${BUILD_DIR}" ];then rm -rf ./${BUILD_DIR}; fi
		west build -p always -b imx93_evk/mimx9352/a55 ${app_path} -DRTOS_ID=${each_rtos} -DCONSOLE=${each_console} -DCMAKE_BUILD_TYPE=${build_type} -d ${BUILD_DIR}
		if [ $? -ne 0 ]; then
			exit 1
		fi
		# append RAM Console buffer address to image name
		if [ $each_console = "RAM_CONSOLE" ]; then
			# parse RAM Console address from dts
			buf_addr=$(sed -n '/ram_console:/p' ${BUILD_DIR}/zephyr/zephyr.dts)
			buf_addr=${buf_addr##*@}
			buf_addr=0x${buf_addr%% *}
			console_name="RAM_CONSOLE-$buf_addr"
		else
			console_name=${each_console}
		fi
		ln -s rpmsg_str_echo.bin ${BUILD_DIR}/zephyr/rpmsg_str_echo_ca55_RTOS${each_rtos}_${console_name}.bin
		ln -s rpmsg_str_echo.elf ${BUILD_DIR}/zephyr/rpmsg_str_echo_ca55_RTOS${each_rtos}_${console_name}.elf
	done
done
