#standby_power 3.3v 1.5v 2.5v 
#power_init
echo "init 3.3v,2.5v,12v,step 0-step 5"
#step1 wait for AGOOD
#step2 ignore
#Enable the gpio port shared by the cpld and ast2500
#echo 371 > /sys/class/gpio/export
#echo out > /sys/class/gpio/gpio371/direction
#echo 0 > /sys/class/gpio/gpio371/value
#echo in >  /sys/class/gpio/gpio354/direction

#step3 VCC_5VSTBY_EN=0 VCC_2V5STBY_EN=1
i2cset -f -y 0 0x57 0x0e 0x03  
#step4 BUF_BP_EN=1 COM_BUF_EN=1
i2cset -f -y 0 0x57 0x0e 0x83   
i2cset -f -y 0 0x57 0x10 0x0c  

#step5 Turn off all power supplies except the standby power supply
i2cset -f -y 0 0x57 0x0e 0x83
i2cset -f -y 0 0x57 0x0f 0x00
i2cset -f -y 0 0x57 0x10 0x01


usleep 20000

#Mount the PCA9548 chip
#i2cset -f -y 0 0x70 0xff 0xff
#Enable the gpio port shared by the cpld and ast2500
#echo 371 > /sys/class/gpio/export
#echo out > /sys/class/gpio/gpio371/direction
echo 0 > /sys/class/gpio/gpio371/value

#echo 281 > /sys/class/gpio/export
#echo 353 > /sys/class/gpio/export
#echo 489 > /sys/class/gpio/export
#echo 495 > /sys/class/gpio/export
#echo 354 > /sys/class/gpio/export
echo in >  /sys/class/gpio/gpio354/direction

#echo "init 3.3v,2.5v,12v,step 0-step 5"
#i2cset -f -y 0 0x57 0x0e 0x03  
#i2cset -f -y 0 0x57 0x0e 0x83  
#i2cset -f -y 0 0x57 0x10 0x0c  

#i2cset -f -y 0 0x57 0x0e 0x83
#i2cset -f -y 0 0x57 0x0f 0x00
#i2cset -f -y 0 0x57 0x10 0x01 

#while
echo "payload_power_on or POR1220_CTL"
echo "vcc_5v_en"
i2cset -f -y 0 0x57 0x0e 0xc3  
usleep 20000

echo "vcc_3v3_en,com_12v,other"
i2cset -f -y 0 0x57 0x0e 0xd3
i2cset -f -y 0 0x57 0x0f 0x2c
#i2cset -f -y 0 0x57 0x0e 0x00
usleep 100000
echo "com_pwr_ok=1"
i2cset -f -y 0 0x57 0x0e 0xd5
i2cset -f -y 0 0x57 0x0f 0x3c 

echo "wait com_s3,s4,s5,com_cb_rst_n"
i=1
	while [ $i -le 10000 ]
	do
		let i++
		CB_S3=`cat /sys/class/gpio/gpio489/value |awk '{print}'`
		CB_S4=`cat /sys/class/gpio/gpio495/value |awk '{print}'`
		CB_S5=`cat /sys/class/gpio/gpio281/value |awk '{print}'`
		CB_RST=`cat /sys/class/gpio/gpio353/value |awk '{print}'`
		status=$((CB_S3|CB_S4 << 1|CB_S5 <<2|CB_RST<<3))
		if [ $status -eq 15 ];then
			echo "start step7"
			break
		else
			echo "status $status  wait..."
		fi
		usleep 1000
	done
	if [ $i -gt 10000 ];then
		echo "cpu start failed, exit startup process"
		exit
	fi

usleep 2500000

i2cset -f -y 0 0x57 0x0e 0xd1 
echo "core_en=1,avdd=1"
i2cset -f -y 0 0x57 0x0f 0x3f
usleep 5000
echo "vcc_2v5_en"
i2cset -f -y 0 0x57 0x0e 0xf1 
usleep 5000
echo "REFCLLK_EN,PO_RST_N=1"
i2cset -f -y 0 0x57 0x0f 0x7f
i2cset -f -y 0 0x57 0x0f 0xff 
usleep 50000
echo "bmc_pp_pci_rst_n_3v3"
i2cset -f -y 0 0x57 0x10 0x01 
echo out > /sys/class/gpio/gpio371/direction
echo 1 > /sys/class/gpio/gpio371/value
i2cset -f -y 0 0x57 0x0e 0xf0

#while
#NOP
#while
#echo "hvout1=1,hvout2=1,hvout3=1"
#i2cset -f -y 0 0x57 0x0e 0xf7 
#usleep 500000
#echo "hvout1=0,hvout2=0,hvout3=0"
#i2cset -f -y 0 0x57 0x0e 0xf0 
#usleep 500000


