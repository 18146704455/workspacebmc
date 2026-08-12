#!/bin/sh
usage(){
	echo "[ERR:param err]"
	echo "[The parameter meanings are as follows]"
	echo "[A:rj45--->88e6185--->come]"
	echo "[B:rj45--->bmc]"
	echo "[C:bmc--->come]"
	echo "[USAGE:]"
	echo "[	./copc_net.sh A"
	echo "[	./copc_net.sh B"
	echo "[	./copc_net.sh C"

}

#switch88e6185
sw wr 0x10 1 0xc17e
sw wr 0x11 1 0xc17e
sw wr 0x12 1 0xc17e
sw wr 0x13 1 0xc17e
sw wr 0x14 1 0xc17e
sw wr 0x15 1 0xc17e
sw wr 0x16 1 0xc17e
sw wr 0x17 1 0xc17e
sw wr 0x18 1 0xc17e
sw wr 0x19 1 0xc17e


if [ $# -ne 0 ];then
	if [ $# -ne 1 ];then
		usage
		exit
	fi
fi
case $1 in
	A)
		#phy1--come  addr:0x07 mdio:0
		phyw 0 7 1e a000
		phyw 0 7 1f 2
		phyw 0 7 0 8140
		phyw 0 7 1e a000
		phyw 0 7 1f 0

		#phy0--bmc   addr:0x03 mdio:0
		phyw 0 3 1e a001
		phyw 0 3 1f 8043
		phyw 0 3 1e 51
		phyw 0 3 1f 879
		phyw 0 3 1e a000
		phyw 0 3 1f 2
		phyw 0 3 0 8140
		;;
	B)
		#phy0--bmc   addr:0x03 mdio:0
		phyw 0 3 1e a000
		phyw 0 3 1f 0
		phyw 0 3 1e a001
		phyw 0 3 1f 8040
		phyw 0 3 1e 51
		phyw 0 3 1f 879
		phyw 0 3 0 9140
		;;
	C)
		#phy1--come  addr:0x07 mdio:0
		phyw 0 7 1e a000
		phyw 0 7 1f 2
		phyw 0 7 0 8140
		phyw 0 7 1e a000
		phyw 0 7 1f 0

		#phy0--bmc   addr:0x03 mdio:0
		phyw 0 3 1e a000
		phyw 0 3 1f 2
		phyw 0 3 1e a001
		phyw 0 3 1f 8045
		phyw 0 3 0 8140
		;;
	*)
		usage
		;;
esac

