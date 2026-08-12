#!/bin/bash

source `pwd`/hwmon_check.sh

tmp75_arr=( "$BASE_PATH/hwmon0" )
tmp75_label=("bf_pos_1" )

TPS_TEMP="temp1_input"
dump_temp_hwmon()
{
    local temp=$(cat $1/$TPS_TEMP)
    echo -e "\tTemperature:   $(echo $temp | sed 's/...$//') (C)"
}

hwmon_check dump_temp_hwmon "${tmp75_arr[*]}" "${tmp75_label[*]}"  "tmp75"
