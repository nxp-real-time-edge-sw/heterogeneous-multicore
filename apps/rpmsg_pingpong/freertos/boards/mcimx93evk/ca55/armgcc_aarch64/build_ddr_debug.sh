#!/bin/bash

build_target=ddr_debug
board=mcimx93evk
cpu=ca55

hmc_root_path="../../../../../../../"
app_root_path="../../../../"

name_str=("remote" "master")
console=("UART2" "UART1")

for i in "${!name_str[@]}"
do
        output_dir=${build_target}_${name_str[$i]}_rtos${i}_${console[$i]}
        rm -rf ${output_dir}
        mkdir ${output_dir}

        # use helper to get the rtos SRAM base address from rtos_memory.h, then set CONFIG_SRAM_BASE_ADDRESS which is used by "west flash"
        gcc -o  ${output_dir}/get_rtos_ram_base -DRTOSID=$i -I${hmc_root_path}/os/freertos/Core_AArch64/boards/${board}/ ${hmc_root_path}/tools/helper/get_rtos_ram_base.c
        sram_addr=$(./${output_dir}/get_rtos_ram_base)

        west sdk_build -p always ${app_root_path} -b ${board} --config ${build_target} -Dcore_id=${cpu} -DCONSOLE=${console[$i]} -DRTOS_ID=$i -DIS_MASTER=${name_str[$i]} -d ${output_dir} -DCONFIG_SRAM_BASE_ADDRESS=${sram_addr}
        if [ $? -ne 0 ]; then
                exit 1
        fi

        # append console name to image name
        image_name="rpmsg_pingpong_${name_str[$i]}_${cpu}_RTOS${i}_${console[$i]}"
        ln -s rpmsg_pingpong_${cpu}.bin ${output_dir}/${image_name}.bin
        ln -s rpmsg_pingpong_${cpu}.elf ${output_dir}/${image_name}.elf
done
