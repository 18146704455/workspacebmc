#!/bin/sh

close_debug_port()
{
    cpld wr 0x20 0x1f
}

set_switch_debug()
{
    sw wr 0x15 1 0xc13c             
    cpld wr 0x20 0x10
    phyw 0 0x10 0x1b 0x9484
    phyw 0 0x10 0 0x8100 
}

if [ $1 == "close" ]; then
    close_debug_port
else 
    set_switch_debug
fi
