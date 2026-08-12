/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2020-07-29 13:38:32
 * @LastEditors: smith.zj
 * @LastEditTime: 2020-08-03 16:20:25
 */ 
#ifndef __PEX8725_H__
#define __PEX8725_H__
#include <stdint.h>

int ir35215_MFR_read(uint16_t reg, uint16_t *data);
int ir35215_MFR_write(uint16_t reg, uint16_t data);
int ir_set_pmbus_addr(unsigned char addr);


#endif

