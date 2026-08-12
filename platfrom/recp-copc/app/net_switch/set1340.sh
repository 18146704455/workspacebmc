#!/bin/sh

debug=1
PHYW=./phyw
PHYR=phyr

CHIP_BUS=1

PORT0_ADDR=0x10
PORT1_ADDR=0x11
PORT2_ADDR=0x12
PORT3_ADDR=0x13

exec_phyw()
{
    $PHYW $*
    if [ $debug -eq  1 ]; then
        echo "$PHYW $*"
    fi
}

set_errta()
{
    case $1 in
        0 )
            port_addr=$PORT0_ADDR
            ;;
        1 )
            port_addr=$PORT1_ADDR
            ;;
        2 )
            port_addr=$PORT2_ADDR
            ;;
        3 )
            port_addr=$PORT3_ADDR
            ;;
    esac


    exec_phyw $CHIP_BUS  $port_addr 0x16 0x00ff
    exec_phyw  $CHIP_BUS  $port_addr 0x18 0x2800
    exec_phyw  $CHIP_BUS  $port_addr 0x17 0x2001
    exec_phyw  $CHIP_BUS  $port_addr 0x16 0
    exec_phyw  $CHIP_BUS  $port_addr 0x16 0x0000
    exec_phyw  $CHIP_BUS  $port_addr 0x1D 0x0003
    exec_phyw  $CHIP_BUS  $port_addr 0x1E 0x002
    exec_phyw  $CHIP_BUS  $port_addr 0x1D 0x0
}

set_1000baset_slave()
{
     case $1 in                      
         0 )
             port_addr=$PORT0_ADDR
             ;;
         1 )
             port_addr=$PORT1_ADDR
             ;;
         2 )
             port_addr=$PORT2_ADDR
            ;;
         3 )
             port_addr=$PORT3_ADDR
             ;;
     esac
     exec_phyw  $CHIP_BUS  $port_addr 0x9 0x1200 
     exec_phyw  $CHIP_BUS  $port_addr 0x1 0x9140
 }

 help()
 {
    echo "+--------------------------------------------------------+";
    echo "|####  port=[0,5]                                        |";
    echo "+--------------------------------------------------------+";
    echo "|    set1340 fix [port]                                    |";
    echo "+--------------------------------------------------------+";
 }

 menu()
 {
     case "$1" in
        "fix" )
         set_errta   $2
         set_1000baset_slave $2
        ;;
    * )
        help
        ;;
    esac
}

menu $*
