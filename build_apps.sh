#!/bin/bash

# usage:
# build or clean all applications: ./build_apps [clean]
# build or clean specified applications: ./build_apps [clean] [core] [os] [board-list] [app-list]

valid_apps=("hello_world" "lwip_ping" "rpmsg_perf" "rpmsg_pingpong" "rpmsg_str_echo" "rpmsg_uart_sharing" "rt_latency" "soem_digital_io" "soem_servo" "virtio_net_backend" "virtio_perf")

valid_acore_boards=("evkmimx8mm_ca53" "evkmimx8mp_ca53" "mcimx91evk_ca55" "mcimx91qsb_ca55" "mcimx93evk_ca55" "mcimx95evk_ca55")
valid_mcore_boards=("evkmimx8mm_cm4" "evkmimx8mp_cm7" "mcimx93evk_cm33")

valid_oss=("freertos" "zephyr")

valid_core=("a-core" "m-core")

is_clean="no"
is_acore="no"
is_mcore="no"

# all binary images will be copied to this directory
images_dir_name="deploy/images"
deploy_dir_name="deploy"

function print_usage() {
    echo "$0 [clean]                                        - build or clean all applications"
    echo "$0 [clean] [os] [board-list] [app-list] [core]    - build or clean specified applications"
    echo "      - os: specify freertos or zephyr or both if no specified."
    echo "      - core: a-core or m-core"
    echo "      - board-list: specify one or some or all boards if no specified: ${valid_acore_boards[*]} ${valid_mcore_boards[*]}"
    echo "      - app-list: specify one or some or all applications if no specified: ${valid_apps[*]}"
}

apps_list=()
acore_boards_list=()
mcore_boards_list=()
oss_list=()

# $1: array name to lookup
# $2: element to search
function array_contains_elem() {
    lookup_array="$1[@]"

    # loop over all array elements
    for each_elem in "${!lookup_array}"; do
        if [[ $each_elem == "$2" ]]; then
            return 0
        fi
    done

    return 1
}

function check_app() {
    is_app="no"
    if array_contains_elem "valid_apps" "$1"; then
        is_app="yes"
        apps_list+=("$1")
    fi
}

function check_board() {
    is_board="no"
    if array_contains_elem "valid_acore_boards" "$1"; then
        is_board="yes"
        acore_boards_list+=("$1")
    elif array_contains_elem "valid_mcore_boards" "$1"; then
        is_board="yes"
        mcore_boards_list+=("$1")
    fi
}

function check_os() {
    is_os="no"
    if array_contains_elem "valid_oss" "$1"; then
        is_os="yes"
        oss_list+=("$1")
    fi
}

function check_core() {
    is_core="no"
	if [[ "$1" == "a-core" ]]; then
		is_core="yes"
		is_acore="yes"
	elif [[ "$1" == "m-core" ]]; then
		is_core="yes"
		is_mcore="yes"
    fi
}

function del_empty_dir() {
    if [ -d "$1" ]; then
        if [ "$(ls -A "$1"|wc -w)" == "0" ]; then
            rm -rf "$1"
        fi
    fi
}

for each_parm in "$@"
do
    check_app "${each_parm}"
    if [[ ${is_app} == "no" ]]; then
        check_board "${each_parm}"
        if [[ ${is_board} == "no" ]]; then
            check_os "${each_parm}"
            if [[ ${is_os} == "no" ]]; then
                check_core "${each_parm}"
                if [[ ${is_core} == "no" ]]; then
                    if [[ "${each_parm}" == "clean" ]]; then
                        is_clean="yes"
                    else
                        echo "Invalid parameter ${each_parm}"
                        print_usage
                        exit 1
                    fi
                fi
            fi
        fi
    fi
done

if [ "${is_clean}" == "no" ]; then
    if [[ "${is_acore}" == "no" && "${is_mcore}" == "no" && "${is_clean}" == "no" ]]; then
        echo "Please specify a-core or m-core"
        print_usage
        exit 1
    elif [[ "${is_acore}" == "yes" && "${is_mcore}" == "yes" && "${is_clean}" == "no" ]]; then
        echo "Can't building a-core and m-core simulteneously with the same toolchain"
        print_usage
        exit 1
    fi
else
    if [ "${is_acore}" == "no" ] && [ "${is_mcore}" == "no" ]; then
        boards_list=("${valid_mcore_boards[@]}")
        boards_list+=("${valid_acore_boards[@]}")
    fi
fi

if [ ${is_acore} == "yes" ]; then
    if [ ${#mcore_boards_list[@]} -ne 0 ]; then
        echo "${mcore_boards_list[@]} is not a-core boards"
        print_usage
        exit 1
    fi
    if [ ${#acore_boards_list[@]} -eq 0 ]; then
        boards_list=("${valid_acore_boards[@]}")
    else
        boards_list=("${acore_boards_list[@]}")
    fi
fi

if [ ${is_mcore} == "yes" ]; then
    if [ ${#acore_boards_list[@]} -ne 0 ]; then
        echo "${acore_boards_list[@]} is not m-core boards"
        print_usage
        exit 1
    fi
    if [ ${#mcore_boards_list[@]} -eq 0 ]; then
        boards_list=("${valid_mcore_boards[@]}")
    else
        boards_list=("${mcore_boards_list[@]}")
    fi
fi

if [ ${#apps_list[@]} -eq 0 ]; then
    apps_list=("${valid_apps[@]}")
fi

if [ ${#oss_list[@]} -eq 0 ]; then
    oss_list=("${valid_oss[@]}")
fi

clean_cmd="./clean.sh"
if [[ "${is_acore}" == "yes" ]]; then
	target_dir="armgcc_aarch64"
	freertos_cmd="./build_ddr_release.sh"
	zephyr_cmd="./build_release.sh"
elif [[ "${is_mcore}" == "yes" ]]; then
	target_dir="armgcc"
	freertos_cmd="./build_release.sh"
fi
cmds=("${freertos_cmd}" "${zephyr_cmd}")

multicore_dir=$(cd "$(dirname "$0")" || exit; pwd)

for each_app in "${apps_list[@]}"; do
    for each_os in "${oss_list[@]}"; do
        for each_board in "${boards_list[@]}"; do
            # clean
            if [ ${is_clean} == "yes" ]; then # clean
                if array_contains_elem "valid_acore_boards" "${each_board}"; then
                    target_dir="armgcc_aarch64"
                else
                    target_dir="armgcc"
                fi
                board_dir="${multicore_dir}/apps/${each_app}/${each_os}/boards/${each_board}"
                if [ -d "${board_dir}" ]; then
                    cd "${board_dir}" || exit
                    for build_dir in `find . -type d -name ${target_dir}`; do
                        cd "${board_dir}/${build_dir}" || exit
                        if [ -f "${clean_cmd}" ]; then
                            ${clean_cmd}
                            if [ $? -ne 0 ]; then
                                echo "clean ${each_os} ${each_app} application on ${each_board} failed"
                                exit 1
                            fi
                            # delete binary and elf image
                            bin_file="${multicore_dir}/${images_dir_name}/${each_board}/${each_os}/${each_app}*.bin"
                            elf_file="${multicore_dir}/${images_dir_name}/${each_board}/${each_os}/${each_app}*.elf"
                            rm -f ${bin_file} ${elf_file}

                            del_empty_dir "${multicore_dir}/${images_dir_name}/${each_board}/${each_os}"
                            del_empty_dir "${multicore_dir}/${images_dir_name}/${each_board}"
                            del_empty_dir "${multicore_dir}/${images_dir_name}"
                            del_empty_dir "${multicore_dir}/${deploy_dir_name}"
                        fi
                    done
                fi
            # build
            else
                board_dir="${multicore_dir}/apps/${each_app}/${each_os}/boards/${each_board}"
                if [ -d "${board_dir}" ]; then
                    cd "${board_dir}" || exit
                    for build_dir in `find . -type d -name ${target_dir}`; do
                        cd "${board_dir}/${build_dir}" || exit
                        for cmd in "${cmds[@]}"; do
                            if [ -f "${cmd}" ]; then
                                echo "building ${each_os} ${each_app} application on ${each_board} ..."
                                ${cmd}
                                if [ $? -ne 0 ]; then
                                    echo "build ${each_os} ${each_app} application on ${each_board} failed"
                                    exit 1
                                fi
                                # copy binary and elf image
                                output_dir="${multicore_dir}/${images_dir_name}/${each_board}/${each_os}"
                                if [ ! -d "${output_dir}" ]; then
                                    mkdir -p "${output_dir}"
                                fi
                                if [[ "${cmd}" =~ "smp" ]]; then
                                    find . -name "${each_app}"_smp.bin -exec cp {} "${output_dir}" \;
                                    find . -name "${each_app}"_smp.elf -exec cp {} "${output_dir}" \;
                                else
                                    find . -name "${each_app}*".bin -exec cp {} "${output_dir}" \;
                                    find . -name "${each_app}*".elf -exec cp {} "${output_dir}" \;
                                fi
                            fi
                        done
                    done
                fi
                # return back
                cd "${multicore_dir}" || exit
            fi
        done
    done
done
