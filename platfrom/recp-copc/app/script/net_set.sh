#!/bin/bash

set_eth1_down(){
	#The phy connected to the eth1 network adapter and the RJ45 is the same, and the channel can be selected as one or the other (competition)
	echo "close eth1"
	ifconfig eth1 down
}

change_eth2_speed(){
	echo "change eth1 speed"
	ethtool -s eth2 autoneg off speed 1000
}

set_eth1_net(){
	#The mido.bin tool depends on the NIC activity status (NIC name)
	 mdio.bin eth1 0x1e 0xa000
	 mdio.bin eth1 0x1f 2
	 mdio.bin eth1 0x1e 0xa001
	 mdio.bin eth1 0x1f 0x8043

	 mdio.bin eth1 0x00 0x8140
}

set_eth1_net
sleep 2
set_eth1_down
change_eth2_speed


