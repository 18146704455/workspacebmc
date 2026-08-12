#!/bin/sh

io_out(){
    local num=$1
    local value=0
    [ "$2"x != ""x ] && value=$2
    [ ! -d "/sys/class/gpio/gpio$num" ] && echo ${num} >/sys/class/gpio/export 
    [ "out"x != `cat /sys/class/gpio/gpio$num/direction`x ] && echo out >/sys/class/gpio/gpio$num/direction 
    echo $value >/sys/class/gpio/gpio$num/value 
}
io_in(){
    local num=$1
    local value=0
    [ "$2"x != ""x ] && value=$2
    [ ! -d "/sys/class/gpio/gpio$num" ] && echo ${num} >/sys/class/gpio/export 
    [ "in"x != `cat /sys/class/gpio/gpio$num/direction`x ] && echo in >/sys/class/gpio/gpio$num/direction 
}

io_in_value(){
    local num=$1
    local value=0
    [ "$2"x != ""x ] && value=$2
    [ ! -d "/sys/class/gpio/gpio$num" ] && echo ${num} >/sys/class/gpio/export 
    [ "in"x != `cat /sys/class/gpio/gpio$num/direction`x ] && echo in >/sys/class/gpio/gpio$num/direction 
    echo `cat /sys/class/gpio/gpio$num/value`

}

gpio_open_uart(){
    io_in 372 0
}

change_uart_COME(){
	io_out 372 1
}


gpio_open_uart
sleep 2
change_uart_COME
