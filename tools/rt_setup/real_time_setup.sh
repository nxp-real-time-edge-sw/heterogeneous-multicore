#!/bin/bash -e

# Detect the platform we are running on.
function detect_soc ()
{
    if grep -q 'i.MX8MM' /sys/devices/soc0/soc_id; then
        echo 'imx8mm'
    elif grep -q 'i.MX8MP' /sys/devices/soc0/soc_id; then
        echo 'imx8mp'
    elif grep -q 'i.MX93' /sys/devices/soc0/soc_id; then
        echo 'imx93'
    elif grep -q 'i.MX95' /sys/devices/soc0/soc_id; then
        echo 'imx95'
    else
        echo 'Unknown'
    fi
}

# $1: cpu core number
function disable_cpu_idle()
{
    # Disable CPU idle deep state transitions exceeding 1us
    if [ -e /sys/devices/system/cpu/cpu"$1"/power/pm_qos_resume_latency_us ]; then
        echo "1" > /sys/devices/system/cpu/cpu"$1"/power/pm_qos_resume_latency_us
    fi
}

# Check if a list of strings contains an element
# $1: list to parse
# $2: item to check
function listincludes()
{
    for word in $1; do
        [[ $word = "$2" ]] && return 0
    done

    return 1
}

# Disable run-time CPU frequency changes:
# - Skip if CPU Freq is disabled
# - Otherwise, set performance or userspace governor with a fixed frequency
function set_cpu_freq_policy()
{
    if [ ! -d /sys/devices/system/cpu/cpufreq ] || \
       [ ! -f /sys/devices/system/cpu/cpufreq/policy0/scaling_available_governors ]; then
        echo "CPU Frequency Scaling is not available, skip configuration"
        return;
    fi

    read -r -a avail_governors <  /sys/devices/system/cpu/cpufreq/policy0/scaling_available_governors

    if listincludes "${avail_governors[*]}" "performance" ; then
        echo "Setting Performance as CPU frequency scaling governor"
        echo performance > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
    elif listincludes "${avail_governors[*]}" "userspace" ; then
        read -r -a avail_frequencies <  /sys/devices/system/cpu/cpufreq/policy0/scaling_available_frequencies
        max_freq=${avail_frequencies[-1]}
        echo "Setting Userspace as CPU frequency scaling governor at $max_freq"
        echo userspace > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
        echo "$max_freq" > /sys/devices/system/cpu/cpufreq/policy0/scaling_setspeed
    else
        echo "Error: can not set performance or userspace as CPU frequency scaling governor"
        exit 1
    fi
}


# $1: register address in hex
function get_reg_value()
{
    local reg_addr="$1"

    echo "0x$(/unit_tests/memtool "$reg_addr" 1 | grep -i "$reg_addr": | sed "s/$reg_addr: *//gi")"
}

function set_ddrc_configuration()
{
    local value=0, timing_orig=0, mstr=0, pwrctl=0, rfshctl=0, rfshtmg=0, new_timing=0

    if [ "$SOC" = "imx93" ]; then
        if [ -e /sys/devices/platform/imx93-lpm/auto_clk_gating ]; then
            echo "Disable auto clock gating"
            echo 0 > /sys/devices/platform/imx93-lpm/auto_clk_gating
        fi
    elif [ "$SOC" = "imx8mm" ] || [ "$SOC" = "imx8mp" ]; then
        echo "Disable DDR Low-Power Mode"
        # Disable selfref_en and powerdown_en in PWRCTL
        pwrctl=$(get_reg_value "0x3D400030")
        value=$(printf '0x%X' $(( pwrctl & ~0x3 )))
        /unit_tests/memtool 0x3d400030="${value}"

        # Check if we are using LPDDR4
        mstr=$(get_reg_value "0x3D400000")
        if [ $(( mstr & 0x20 )) -eq "0" ]; then
            return;
        fi

        # If current configuration is using all bank refresh, set the minimum refresh rate to a per bank value
        # with the assumption of having 8 banks (common SDRAM configuration).
        rfshctl=$(get_reg_value "0x3D400050")
        if [ $(( rfshctl & 0x4 )) -eq "0" ]; then
            echo "DDRC: Enable per-bank LPDDR4 refresh"
            rfshtmg=$(get_reg_value "0x3D400064")
            value=$(printf '0x%X' $(( rfshtmg & ~0x3ff )))
            timing_orig=$(printf '0x%X' $(( rfshtmg & 0x3ff )))
            new_timing=$(printf '0x%X' $(( timing_orig / 8 )))
            value=$(printf '0x%X' $(( value | new_timing )))
            echo "DDRC: Change tRFC(min) from all bank value (${timing_orig}) to per bank value (${new_timing})"
            /unit_tests/memtool 0x3d400064="${value}"
            # Now enable per bank refresh
            rfshctl=$(get_reg_value "0x3D400050")
            value=$(printf '0x%X' $(( rfshctl | 0x4 )))
            /unit_tests/memtool 0x3d400050="${value}"
        fi
    fi
}

function disable_cpu_idle_all()
{
    echo "Disable CPU idle for all cores"
    if [ "$SOC" = "imx93" ]; then
        disable_cpu_idle 0
        disable_cpu_idle 1
    elif [ "$SOC" = "imx8mm" ] || [ "$SOC" = "imx8mp" ]; then
        disable_cpu_idle 0
        disable_cpu_idle 1
        disable_cpu_idle 2
        disable_cpu_idle 3
    elif [ "$SOC" = "imx95" ]; then
        disable_cpu_idle 0
        disable_cpu_idle 1
        disable_cpu_idle 2
        disable_cpu_idle 3
        disable_cpu_idle 4
        disable_cpu_idle 5
    fi
}

function disable_rtc_device()
{
    if [ "$SOC" = "imx93" ]; then
        # Unbind the BBNSM RTC device
        BBNSM_RTC_DEV="44440000.bbnsm:rtc"
        if [ -L /sys/bus/platform/drivers/bbnsm_rtc/${BBNSM_RTC_DEV} ]; then
            echo "Unbind RTC device"
            echo "${BBNSM_RTC_DEV}" > /sys/bus/platform/drivers/bbnsm_rtc/unbind
        fi
    elif [ "$SOC" = "imx8mm" ] || [ "$SOC" = "imx8mp" ]; then
        # Unbind the RTC device
        RTC_DEV="30370000.snvs:snvs-rtc-lp"
        if [ -L /sys/bus/platform/drivers/snvs_rtc/${RTC_DEV} ]; then
            echo "Unbind RTC device"
            echo "${RTC_DEV}" > /sys/bus/platform/drivers/snvs_rtc/unbind
        fi
    elif [ "$SOC" = "imx95" ]; then
        # Unbind the SCMI RTC device
        SCMI_DEV="scmi_dev.11"
        if [ -L /sys/bus/scmi_protocol/drivers/scmi-imx-bbm/${SCMI_DEV} ]; then
            echo "Unbind RTC device"
            echo "${SCMI_DEV}" > /sys/bus/scmi_protocol/drivers/scmi-imx-bbm/unbind
        fi
    fi
}

function set_real_time_configuration()
{
    set_ddrc_configuration

    disable_cpu_idle_all

    disable_rtc_device

    set_cpu_freq_policy
}

# Detect the SoC we're running on.
SOC=$(detect_soc)

set_real_time_configuration

