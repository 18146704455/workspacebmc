#!/bin/bash

source `pwd`/hwmon_check.sh

max1617_arr=("$BASE_PATH/hwmon3")
max1617_label=("BAREFOOT" )


dump_max1617_hwmon()
{
    localt=$(cat $1/temp1_input  | sed 's/...$//')
    remotet=$(cat $1/temp2_input | sed 's/...$//')
    echo -e "\tlocal: $localt C, remote: $remotet C"
}

hwmon_check dump_max1617_hwmon "${max1617_arr[*]}" "${max1617_label[*]}"  "max1617"
