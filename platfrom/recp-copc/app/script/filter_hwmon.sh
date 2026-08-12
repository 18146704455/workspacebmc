#!/bin/bash
BASE_DIR=/sys/class/hwmon

for file in `ls $BASE_DIR`
do
    if [ -d $BASE_DIR"/"$file ];then
	monitor=$(cat $BASE_DIR"/"$file/name)
    	echo -e "$monitor \t$file"
    fi
done
