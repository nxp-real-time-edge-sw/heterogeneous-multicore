#!/bin/bash

app_path=../../../../
build_type=Release

# Define supported console by each RTOS instance
# By default, UART2 is used for uboot and Linux, UART4 is used for M-Core RTOS.
# Modify the console list according to usecase setup.
console_matrix=(rtos0_console_list rtos1_console_list rtos2_console_list rtos3_console_list)
rtos0_console_list=("RAM_CONSOLE" "UART4")
rtos1_console_list=("RAM_CONSOLE")
rtos2_console_list=("RAM_CONSOLE")
rtos3_console_list=("RAM_CONSOLE" "UART2")

smp_matrix=(rtos0_smp_list rtos1_smp_list rtos2_smp_list rtos3_smp_list)
rtos0_smp_list=("nonsmp" "smp_2cores" "smp_4cores")
rtos1_smp_list=("nonsmp")
rtos2_smp_list=("nonsmp")
rtos3_smp_list=("nonsmp")

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
		TMPD=${smp_matrix[${each_rtos}]}
		TMPE=$TMPD[@]
		TMPF=(${!TMPE})
		for each_smp in ${TMPF[@]}
		do
			DTC_OVERLAY_OPTION=""
			CONFIG_OPTION=""
			if [ ${each_smp} = "smp_4cores" ]; then
				SMP="SMP_4CORES_"
				BUILD_BOARD=imx8mp_evk/mimx8ml8/a53/smp
				DTC_OVERLAY=$(realpath "../../../imx8mp_evk_mimx8ml8_a53_smp_4cores.overlay")
				DTC_OVERLAY_OPTION="-DDTC_OVERLAY_FILE=${DTC_OVERLAY}"
				CONFIG_OPTION="-DCONFIG_MP_MAX_NUM_CPUS=4"
			elif [ ${each_smp} = "smp_2cores" ]; then
				SMP="SMP_2CORES_"
				BUILD_BOARD=imx8mp_evk/mimx8ml8/a53/smp
				CONFIG_OPTION="-DCONFIG_MP_MAX_NUM_CPUS=2"
			else
				SMP=""
				BUILD_BOARD=imx8mp_evk/mimx8ml8/a53
			fi

			BUILD_DIR="build_RTOS${each_rtos}_${SMP}${each_console}"
			if [ -d "${BUILD_DIR}" ];then rm -rf ./${BUILD_DIR}; fi
			west build -p always -b ${BUILD_BOARD} ${app_path} -DRTOS_ID=${each_rtos} -DCONSOLE=${each_console} ${CONFIG_OPTION} ${DTC_OVERLAY_OPTION} -DCMAKE_BUILD_TYPE=${build_type} -d ${BUILD_DIR}
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
			ln -s hello_world.bin ${BUILD_DIR}/zephyr/hello_world_ca53_RTOS${each_rtos}_${SMP}${console_name}.bin
			ln -s hello_world.elf ${BUILD_DIR}/zephyr/hello_world_ca53_RTOS${each_rtos}_${SMP}${console_name}.elf
		done
	done
done
