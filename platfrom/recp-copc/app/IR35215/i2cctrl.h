/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-10-21 17:03:12
 * @LastEditors: smith.zj
 * @LastEditTime: 2020-07-29 13:41:30
 */
#ifndef __I2C_CTRL_H__
#define __I2C_CTRL_H__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "common.h"

#define	I2C_BUS "/dev/i2c-11"

extern int g_i2c_fd;


#define DEV_FD_IS_INIT(bus) ((bus) != -1)


int init_i2cctrl();
int open_dev(char *devname);
int set_addr(int fd,int addr);
void close_dev(int fd);


#endif
