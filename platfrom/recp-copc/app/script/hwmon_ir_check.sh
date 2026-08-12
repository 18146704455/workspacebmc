#!/bin/bash
source `pwd`/test_check.sh

ir35215_arr=("$BASE_PATH/0-0041/hwmon/*")
ir35215_label=("BAREFOOT")

TPS_VIN="in1_input"
TPS_VOUT="in2_input"
TPS_CURR="curr1_input"
TPS_POWER="power1_input"
TPS_TEMP="temp1_input"

dump_ir35215_hwmon()
{
#    echo $1/$TPS_VIN
	vin=$(cat $1/$TPS_VIN)
	vout=$(cat $1/$TPS_VOUT)
	curr=$(cat $1/$TPS_CURR)
	powr=$(cat $1/$TPS_POWER)
	temp=$(cat $1/$TPS_TEMP)
	
	echo -e "\tVoltage IN:     $vin  (mV)"
	echo -e "\tVoltage OUT:    $vout (mV)"
	echo -e "\tCurrent:        $curr (mA)"
	echo -e "\tPower:          $powr (mW)"
	echo -e "\tTemperature:   $(echo $temp | sed 's/...$//') (C)"
}


hwmon_check dump_ir35215_hwmon "${ir35215_arr[*]}" "${ir35215_label[*]}"  "ir35215"
