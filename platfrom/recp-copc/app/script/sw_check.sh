#!/bin/sh
read_check(){
    VAL=`sw rd $1 1 | cut -d " " -f 2`
    if [ $VAL != $2 ];then
        echo "not match:real $VAL want $2"
        exit
    fi
}

for i in $(seq 1 20);do
    sw wr 0x10 1 0xc17e
    read_check 0x10 0xc17e
    sw wr 0x10 1 0xc103
    read_check 0x10 0xc103
done

