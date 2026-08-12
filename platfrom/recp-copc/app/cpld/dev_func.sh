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



io_init(){
    #GPIOA0
    io_out 280 1
    #GPIOA1
    io_in 281 1
    
    #GPIOB0
    io_in 288 0
    #GPIOB5
    io_out 293 0
    #GPIOB6
    io_out 294 0
    #GPIOB7
    io_out 295 0
    
    
    #GPIOD0
    io_in 304 1
    #GPIOD2
    io_in 306 1
    #GPIOD3
    io_in 307 0
    #GPIOD4
    io_in 308 1
    #GPIOD5
    io_in 309 0
    #GPIOD6
    io_in 310 0
    #GPIOD7
    io_in 311 0

    #BMC_uart 1
    #GPIOL6
    #GPIOL7
    
    #GPION0
    io_in 384 0
    #GPION1
    io_in 385 1
    #GPION2
    io_in 386 1
    #GPION3
    io_in 387 1

    #GPIOO2
    io_out 394 1
    #GPIOO3
    io_in 395 0
    #GPIOO5
    io_out 397 1
    #GPIOO6
    io_out 398 1
    #GPIOO7
    io_in 399 0


    #GPIOP0
    io_out 400 1
    #GPIOP1
    io_in 401 1
    #GPIOP2
    io_out 402 1
    #GPIOP3
    io_in 403 0

    #GPIOS1
    io_in 425 1
    #GPIOS2
    io_in 426 1
    #GPIOS3
    io_in 427 1

    #GPIOY0
    io_in 472 1
    #GPIOY1
    io_in 473 1
    #GPIOY2
    io_in 474 0
    #GPIOY3
    io_in 475 0
    #GPIOZ0
    io_in 480 0
    #GPIOZ1
    io_in 481 1
    #GPIOZ2
    io_in 482 0

    #GPIOAA3
    io_out 491 0
    #GPIOAA5
    io_in 493 1

    #GPIOM1
    io_in 377 0
    #GPIOM2
    io_in 378 0
    #GPIOM3
    io_in 379 0

}

io_copc_init()
{
    #gpio_ctrl
    #Enable the gpio port shared by the cpld and ast2500
    #Put the I/O port control in the standby power control
    #io_out 371 0 

    #power_ctrl:com_s3,s4,s5,com_cb_rst_n 
    io_in 281 0
    io_in 353 0
    io_in 489 0
    io_in 495 0
    io_in 354 0

#CPLD_bmc_public_gpio
io_in 376 0
io_in 354 0
io_in 355 0
io_in 488 0
io_in 491 0
io_in 493 0

    #reset
#come_reset
io_in 488  0
#tofino(pwron_rst)
io_in 355  0
#tofino(core_rst)
io_in 354  0
#tofino(pcie_rst)
io_in 371  0

#cpld
#io_in 490  0
#come(cb_rst)
#io_in 353  0
#bmc_phy(CTC21101)
io_in 283  0
#PCA9548
io_in 496  0

}




io_copc_init
#io_init
