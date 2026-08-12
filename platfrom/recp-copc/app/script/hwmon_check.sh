#!/bin/bash
BASE_PATH=/sys/class/hwmon


hwmon_check()
{
	hwmon_arr=($2)
	hwmon_label=($3)
	dump_func=($1)
	for ((i = 0;i < ${#hwmon_arr[@]};i++))
	do
		echo "===================module $4 ${hwmon_label[$i]} info =========================="
		$dump_func ${hwmon_arr[$i]}
	done
}
