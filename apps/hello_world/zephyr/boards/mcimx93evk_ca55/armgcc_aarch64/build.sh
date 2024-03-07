#!/bin/bash

CMSIS_PATH=$(realpath ../../../../../../../modules/hal/cmsis)
HAL_NXP_PATH=$(realpath ../../../../../../../modules/hal/nxp)

ZEPHYR_TOOLCHAIN_VARIANT=cross-compile
CROSS_COMPILE=$(find "$ARMGCC_DIR" -name "*-gcc" | sed -e 's/gcc$//')
export ZEPHYR_TOOLCHAIN_VARIANT
export CROSS_COMPILE

# Define supported console by each RTOS instance
# By default, LPUART1 is used for uboot and Linux, LPUART2 is used for M-Core RTOS.
# Modify the console list according to usecase setup.
console_matrix=(rtos0_console_list rtos1_console_list)
rtos0_console_list=("RAM_CONSOLE" "UART2")
rtos1_console_list=("RAM_CONSOLE" "UART1")

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
		if [ -d "build_RTOS${each_rtos}_${each_console}" ];then rm -rf ./build_RTOS${each_rtos}_${each_console}; fi
		cmake -B build_RTOS${each_rtos}_${each_console} -GNinja -DZEPHYR_MODULES="$CMSIS_PATH;$HAL_NXP_PATH" -DRTOS_ID=${each_rtos} -DCONSOLE=${each_console} -DBOARD=mimx93_evk_a55  ../../../
		ninja -C build_RTOS${each_rtos}_${each_console}
		if [ $? -ne 0 ]; then
			exit 1
		fi
		mv build_RTOS${each_rtos}_${each_console}/zephyr/hello_world.bin build_RTOS${each_rtos}_${each_console}/zephyr/hello_world_ca55_RTOS${each_rtos}_${each_console}.bin
		mv build_RTOS${each_rtos}_${each_console}/zephyr/hello_world.elf build_RTOS${each_rtos}_${each_console}/zephyr/hello_world_ca55_RTOS${each_rtos}_${each_console}.elf
	done
done
