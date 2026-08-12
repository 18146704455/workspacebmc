#!/bin/sh
change_uart_NPS(){
	cpld wr 0x56 $1
}

change_uart_COME(){
	cpld wr 0x56 8
}

usage(){
	echo "[ERR:param err]"
	echo "[USAGE:]"
	echo "[	./utili.sh come"
	echo "[	./utili.sh nps0|1|2|3"

}

if [ $# -ne 1 ];then
	if [ $# -ne 2 ];then
		usage
		exit
	fi
fi
case $1 in
	come)
	    cpld wr 0x56 8
		;;
	nps0)
	    cpld wr 0x56 0
		;;
	nps1)
	    cpld wr 0x56 1
		;;
	nps2)
	    cpld wr 0x56 2
		;;
	nps3)
	    cpld wr 0x56 3
		;;
	*)
		usage
		;;
esac














