#!/bin/bash

function print_usage() {
	echo "./build_all.sh core_type [directory]               - build all a-core or m-core applications in [directory]"
	echo "./build_all.sh [core_type] [directory] clean       - clean all a-core or m-core or for both applications building in [directory]"
	echo "                                                   - core_type: a-core or m-core"
	echo ""
	echo " For example:"
	echo "     ./build_all.sh a-core                         -build all a-core applications in apps directory"
	echo "     ./build_all.sh m-core apps/hello_world        -build all m-core applications in apps/hello_world directory"
	echo "     ./build_all.sh clean                          -clean all applications building in apps directory"
	echo "     ./build_all.sh a-core clean                   -clean all a-core applications building in apps directory"
	echo "     ./build_all.sh m-core apps/hello_world clean  -clean all m-core applications building in apps/hello_world directory"
	echo "     ./build_all.sh apps/hello_world clean         -clean all applications building in apps/hello_world directory"
	exit 1
}

valid_params=("clean" "a-core" "m-core")

valid_plats=("evkmimx8mp" "evkmimx8mm" "mcimx93evk")

imgs_dir_name="binaries"

function check_sub_dir() {
	if [[ "${valid_params[0]}" != "$1" ]] && [[ "${valid_params[1]}" != "$1" ]] && [[ "${valid_params[2]}" != "$1" ]]; then
		if [ -d $1 ]; then
			sub_dir=$1
		else
			print_usage
		fi
	fi
}


function get_plat_name() {
	if [[ "$1" =~ "${valid_plats[0]}" ]]; then
		plat_name=${valid_plats[0]}
	elif [[ "$1" =~ "${valid_plats[1]}" ]]; then
		plat_name=${valid_plats[1]}
	elif [[ "$1" =~ "${valid_plats[2]}" ]]; then
		plat_name=${valid_plats[2]}
	else
		echo "Wrong directory name: $1"
		exit 1
	fi
}

if [ $# -gt 3 ]; then
	print_usage
fi

params=("$1" "$2" "$3")

if [[ "${params[0]}" == "a-core" ]] || [[ "${params[1]}" == "a-core" ]] || [[ "${params[2]}" == "a-core" ]]; then
	build_dir=("armgcc_aarch64")
	freertos_cmd="./build_ddr_release.sh"
elif [[ "${params[0]}" == "m-core" ]] || [[ "${params[1]}" == "m-core" ]] || [[ "${params[2]}" == "m-core" ]]; then
	build_dir=("armgcc")
	freertos_cmd="./build_release.sh"
fi

if [[ "${params[0]}" == "clean" ]] || [[ "${params[1]}" == "clean" ]] || [[ "${params[2]}" == "clean" ]]; then
	if [ "${build_dir}" == "" ]; then
		build_dir=("armgcc_aarch64" "armgcc")
	fi
	freertos_cmd="./clean.sh"
elif [[ "${build_dir}" == "" ]]; then
	print_usage
fi

cmds=(${freertos_cmd})

if [ "$1" != "" ]; then
	check_sub_dir $1
fi
if [ "$2" != "" ]; then
	check_sub_dir $2
fi
if [ "$3" != "" ]; then
	check_sub_dir $3
fi

multicore_dir=$(cd $(dirname $0); pwd)

cd ${multicore_dir}

# Currently any clean command will delete all binaries in build directory
if [[ ${cmds[@]} =~ "./clean.sh" ]]; then
	rm -rf ${multicore_dir}/${imgs_dir_name}
fi

# only build in "apps" directory
if [[ ${sub_dir} == "" ]]; then
	sub_dir="./apps"
fi
cd ${sub_dir}

for each_dir in ${build_dir[*]}
do
	for dir_name in `find -name ${each_dir}`
	do
		cd ${dir_name}
		for cmd in ${cmds[*]}
		do
			echo ""
			echo "############################################################################################################"
			echo "Execute ${cmd} in ${sub_dir}"/"${dir_name}"
			echo "############################################################################################################"
			echo ""
			if [ -f ${cmd} ]; then
				${cmd}
				if [ $? -ne 0 ]; then
					echo "Execute ${cmd} in ${sub_dir}"/"${dir_name} failed"
					exit 1
				fi
			fi

			# copy binary to output directory
			if [[ ${cmd} != "./clean.sh" ]]; then
				get_plat_name ${sub_dir}"/"${dir_name}
				for each_bin in `find -name *.bin`
				do
					output_dir="${multicore_dir}/${imgs_dir_name}/${plat_name}"
					if [ ! -d ${output_dir} ]; then
						mkdir -p ${output_dir}
					fi
					cp ${each_bin} ${output_dir}
				done
			fi
		done
		# return back
		cd ${multicore_dir}
		cd ${sub_dir}
	done
done;
