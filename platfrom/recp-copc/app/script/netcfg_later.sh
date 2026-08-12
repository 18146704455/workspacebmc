config_net_lc02v2(){

    #CMM1/debug
    sw wr 0x12 1 0xc17e         
    phyw 0 0x10 0x1b 0x9484     
    phyw 0 0x10 0 0x8100  

    sw wr 0x11 1 0xc17e     
    phyw 0 0x11 0x1b 0x9484 
    phyw 0 0x11 0 0x8140 

    #TH
    sw wr 0x17 1 0xc17e     

    #scm
    sw wr 0x10 1 0xc17e
    sw wr 0x16 1 0xc17e

    #cmm2
    sw wr 0x13 1 0xc17e

    #socket 0~2,3
    sw wr 0x14 1 0xc17e
    sw wr 0x15 1 0xc17e
    sw wr 0x18 1 0xc17e
    cpld wr 0x20 0x1f

    #bmc phy2
    sw wr 0x19 1 0xc17e     
    phyw 1 0x10 0x16 1      
    phyw 1 0x10 0x9 0x300   
    phyw 1 0x10 0 8140          
}
config_net_lcb(){
    #switch88e6185
    sw wr 0x10 1 0xc17e
    sw wr 0x11 1 0xc17e
    sw wr 0x12 1 0xc17e
    sw wr 0x13 1 0xc17e
    sw wr 0x14 1 0xc17e
    sw wr 0x15 1 0xc17e

    #phy0x07 bus0
    phyw 0 7 1e a000
    phyw 0 7 1f 2
    phyw 0 7 0 8140
    phyw 0 7 1e a000
    phyw 0 7 1f 0

    #phy0x03 bus1
    phyw 1 3 1e a001
    phyw 1 3 1f 8045
    phyw 1 3 1e a000
    phyw 1 3 1f 2
    phyw 1 3 0 8140
    
    #phy0x03 bus0 
    phyw 0 3 1e a001
    phyw 0 3 1f 8043
    phyw 0 3 1e 51
    phyw 0 3 1f 879
    phyw 0 3 1e a000
    phyw 0 3 1f 2
    phyw 0 3 0 8140
}
reset_netswitch() {
	cpld wr 0x30 0x0;
	sleep 1;
	cpld wr 0x30 0xf;
}

sleep 20
echo "config net switch" >> /tmp/boot.log_retry
for idx in $(seq 1 5);do
	netcfg_dsdt -slot 0x$SLOT_ID >> /tmp/netcfg.log
	if [ $? -ne 0 ];then
		echo "config net switch failed trytime $idx" >> /tmp/boot.log_retry
		reset_netswitch
        /script/sw_check.sh >> /tmp/mdio_check
		usleep 10000
	else
		echo "config net switch success" >> /tmp/boot.log
		break;
	fi
done
if [ $idx -eq 5 ];then
	echo "config net switch failed with all retry" >> /tmp/boot.log
else
    config_net_lcb
fi
exit 0
