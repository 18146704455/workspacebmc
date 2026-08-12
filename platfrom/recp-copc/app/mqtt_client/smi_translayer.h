/*************************************************************************
  > File Name: smi_translayer.h
  > Author: 
  > Mail: 
  > Created Time: Tue 31 Dec 2019 04:29:48 PM CST
 ************************************************************************/

#ifndef _SMI_TRANSLAYER_H
#define _SMI_TRANSLAYER_H
#include <stdint.h>
#include <stdbool.h>
  
int initSmiMem();
void unmapSmiMem();
int smi_read(uint8_t busno, uint16_t smiaddr, uint16_t port,uint16_t regaddr, uint16_t* val,bool C45);
int smi_write(uint8_t busno, uint16_t smiaddr, uint16_t port,uint16_t regaddr, uint16_t val,bool C45);
#endif
