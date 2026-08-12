#!/bin/bash
###
 # @Descripttion: 
 # @version: 
 # @Author: smith.zj
 # @LastEditors: smith.zj
### 

source /usr/sbin/dev_func.sh

COMe_type_dict=(["00"]="b641" ["01"]="y6897")

COMe_Type="y6897"

get_come_type()
{
	local id=`io_in_value 288`	
	echo $id0  ${COMe_type_dict[$id]}
	export COMe_Type=${COMe_type_dict[$id]}
}

set_come_cpld_base_reg()
{
    case $COMe_Type in
        "y6897")
            cpld wr 0x2c 0xa0
            ;;
        "b641")
            cpld wr 0x2c 0xfe
            ;;
        *)
            cpld wr 0x2c 0xfe
            ;;
    esac
}

get_come_type
set_come_cpld_base_reg
echo $COMe_Type

