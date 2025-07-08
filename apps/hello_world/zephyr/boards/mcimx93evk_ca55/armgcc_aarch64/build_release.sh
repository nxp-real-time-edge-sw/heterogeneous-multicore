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

smp_matrix=(rtos0_smp_list rtos1_smp_list)
rtos0_smp_list=("nonsmp" "smp")
rtos1_smp_list=("nonsmp")

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
		TMPD=${smp_matrix[${each_rtos}]}
		TMPE=$TMPD[@]
		TMPF=(${!TMPE})
		for each_smp in ${TMPF[@]}
		do
			if [ ${each_smp} = "smp" ]; then
				SMP="SMP_"
				BUILD_BOARD=imx93_evk/mimx9352/a55/smp
			else
				SMP=""
				BUILD_BOARD=imx93_evk/mimx9352/a55
			fi

			BUILD_DIR="build_RTOS${each_rtos}_${SMP}${each_console}"
			if [ -d "${BUILD_DIR}" ];then rm -rf ./${BUILD_DIR}; fi
			cmake -B ${BUILD_DIR} -GNinja -DZEPHYR_MODULES="$CMSIS_PATH;$HAL_NXP_PATH" -DRTOS_ID=${each_rtos} -DCONSOLE=${each_console} -DBOARD=${BUILD_BOARD} ../../../
			ninja -C ${BUILD_DIR}
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
			mv ${BUILD_DIR}/zephyr/hello_world.bin ${BUILD_DIR}/zephyr/hello_world_ca55_RTOS${each_rtos}_${SMP}${console_name}.bin
			mv ${BUILD_DIR}/zephyr/hello_world.elf ${BUILD_DIR}/zephyr/hello_world_ca55_RTOS${each_rtos}_${SMP}${console_name}.elf
		done
	done
done
