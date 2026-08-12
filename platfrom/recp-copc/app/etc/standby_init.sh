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
