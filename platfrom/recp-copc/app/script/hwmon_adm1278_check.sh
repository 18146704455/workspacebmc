#!/bin/bash

source `pwd`/hwmon_check.sh

adm1278_arr=("$BASE_PATH/hwmon1")
adm1278_label=("adm1278")

POWERR_VOLT="in1_input"
POWERR_CURR="curr1_input"
POWERR_POW="power1_input"
POWERR_TEMP="temp1_input"
dump_adm1278_hwmon()
{
	volt=$(cat $1/$POWERR_VOLT)
	curr=$(cat $1/$POWERR_CURR)
	powr=$(cat $1/$POWERR_POW)
	
	echo -e "\tVoltage IN:     $volt  (mV)"
	echo -e "\tCurrent:        $curr (mA)"
	echo -e "\tPower:          $powr (mW)"
}

hwmon_check dump_adm1278_hwmon "${adm1278_arr[*]}" "${adm1278_label[*]}"  "adm1278"
