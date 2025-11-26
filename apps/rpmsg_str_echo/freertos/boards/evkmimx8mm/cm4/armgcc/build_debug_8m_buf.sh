#!/bin/bash

hmc_root_path="../../../../../../../"
app_root_path="../../../../"

build_tpye=debug
output_dir=${build_tpye}_8m_buf

rm -rf ${output_dir}
west sdk_build -p always ${app_root_path} -b evkmimx8mm --config ${build_tpye} -Dcore_id=cm4 -DRPMSG_8M_BUF=1 -d ${output_dir}
if [ $? -ne 0 ]; then
	exit 1
fi

ln -s rpmsg_str_echo_cm4.bin ${output_dir}/rpmsg_str_echo_cm4_8m_buf.bin
ln -s rpmsg_str_echo_cm4.elf ${output_dir}/rpmsg_str_echo_cm4_8m_buf.elf
