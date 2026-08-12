#!/bin/sh

awk -F',' 'BEGIN{i = 0;} /0x/ {
    page = substr($1,3,2);
    addr = substr($1,5,2);
    val  = substr($2,3,2);
    if(i == 3){system("sleep 1")};
    system("si3547 wr 1 " page);
    system("si3547 wr " addr " " val);
    if(i % 64 == 0){system("echo process " i " regs")};
    i++;
}' $1
