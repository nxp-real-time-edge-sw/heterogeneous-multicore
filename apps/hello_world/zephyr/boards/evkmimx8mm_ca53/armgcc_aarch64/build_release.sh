#!/bin/bash

CMSIS_PATH=$(realpath ../../../../../../../modules/hal/cmsis)
HAL_NXP_PATH=$(realpath ../../../../../../../modules/hal/nxp)

ZEPHYR_TOOLCHAIN_VARIANT=cross-compile
CROSS_COMPILE=$(find "$ARMGCC_DIR" -name "*-gcc" | sed -e 's/gcc$//')
export ZEPHYR_TOOLCHAIN_VARIANT
export CROSS_COMPILE

# Define supported console by each RTOS instance
# By default, UART2 is used for uboot and Linux, UART4 is used for M-Core RTOS.
# Modify the console list according to usecase setup.
console_matrix=(rtos0_console_list rtos1_console_list rtos2_console_list rtos3_console_list)
rtos0_console_list=("RAM_CONSOLE" "UART4")
rtos1_console_list=("RAM_CONSOLE")
rtos2_console_list=("RAM_CONSOLE")
rtos3_console_list=("RAM_CONSOLE" "UART2")

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
		if [ -d "build_RTOS${each_rtos}_${each_console}" ];then rm -rf ./build_RTOS${each_rtos}_${each_console}; fi
		cmake -B build_RTOS${each_rtos}_${each_console} -GNinja -DZEPHYR_MODULES="$CMSIS_PATH;$HAL_NXP_PATH" -DRTOS_ID=${each_rtos} -DCONSOLE=${each_console} -DBOARD=imx8mm_evk/mimx8mm6/a53  ../../../
		ninja -C build_RTOS${each_rtos}_${each_console}
		if [ $? -ne 0 ]; then
			exit 1
		fi
		# append RAM Console buffer address to image name
		if [ $each_console = "RAM_CONSOLE" ]; then
			# parse RAM Console address from dts
			buf_addr=$(sed -n '/ram_console:/p' build_RTOS${each_rtos}_${each_console}/zephyr/zephyr.dts)
			buf_addr=${buf_addr##*@}
			buf_addr=0x${buf_addr%% *}
			console_name="RAM_CONSOLE-$buf_addr"
		else
			console_name=${each_console}
		fi
		mv build_RTOS${each_rtos}_${each_console}/zephyr/hello_world.bin build_RTOS${each_rtos}_${each_console}/zephyr/hello_world_ca53_RTOS${each_rtos}_${console_name}.bin
		mv build_RTOS${each_rtos}_${each_console}/zephyr/hello_world.elf build_RTOS${each_rtos}_${each_console}/zephyr/hello_world_ca53_RTOS${each_rtos}_${console_name}.elf
	done
done
