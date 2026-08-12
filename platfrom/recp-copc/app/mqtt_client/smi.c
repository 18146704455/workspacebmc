/*************************************************************************
  > File Name: smi.c
  > Author: 
  > Mail: 
  > Created Time: Tue 31 Dec 2019 05:17:42 PM CST
 ************************************************************************/
#include "smi_translayer.h"
#include "common_log.h"
#include <strings.h>
#include <stdlib.h>
   
#include <stdio.h>
 
int main(int argc, char *argv[])
{
    int rv;
    uint8_t bus;
    uint16_t smiaddr;
    uint16_t port;
    uint16_t regaddr;
  uint16_t value;
  if(argc < 4){
    pr_err("bus smiaddr port regaddr\n");
    exit(-1);
  }
  pr_log("%s %s %s %s %s ",argv[0],argv[1],argv[2],argv[3],argv[4]);
  sscanf(argv[1]," %i",&bus);
  sscanf(argv[2]," %i",&smiaddr);
  sscanf(argv[3]," %i",&port);
  sscanf(argv[4]," %i",&regaddr);
  pr_log("bus 0x%x smiaddr 0x%x port 0x%x regaddr 0x%x",
          bus,smiaddr, port,regaddr);
  initSmiMem();
  rv = smi_read(bus,smiaddr,port,regaddr, &value, true);
  pr_log("value 0x%x, rv %d",value, rv);
  unmapSmiMem();
  return 0;
}


