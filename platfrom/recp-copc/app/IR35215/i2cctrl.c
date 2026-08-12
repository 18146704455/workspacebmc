/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-10-21 17:03:12
 * @LastEditors: smith.zj
 * @LastEditTime: 2019-10-31 11:06:51
 */
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
#include "smbus.h"
#include "i2cctrl.h"


#ifdef DB_CURR_LEVEL 
#undef DB_CURR_LEVEL
#define DB_CURR_LEVEL DB_ERR
#endif


int g_i2c_fd = -1;

int init_i2cctrl()
{

	DB_PRINT(DB_DEBUG,"init i2c control");

    if(g_i2c_fd != -1) {
        return 0;
    }

	g_i2c_fd = open_dev(I2C_BUS);
	if(g_i2c_fd == -1) {
		DB_PRINT(DB_ERR,"ERR:could not open dev file %s:%s",I2C_BUS,strerror(errno));
		return -1;
	} else {
		DB_PRINT(DB_DEBUG,"open %s success",I2C_BUS);
	}


	return 0;
}

void deinit_i2cctrl()
{
	DB_PRINT(DB_DEBUG,"deinit i2c control");
	printf("init i2c control\n");
	close_dev(g_i2c_fd);
    g_i2c_fd = -1;

}

int open_dev(char *devname)
{
	int fd = -1;
    fd = open(devname, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Error: Could not open file "
                "%s: %s\n", devname, strerror(errno));
        return -1; 
    } else {
    //	printf("%s:%d open bus %s success\n",__func__,__LINE__,devname);
    }
    return fd;
}

int set_addr(int fd,int addr)
{
    /* With force, let the user read from/write to the registers
     * even when a driver is also running
     */
	if(fd < 0) {
		fprintf(stderr, "Error: ivalid fd");
		return -1;
	}

    if (ioctl(fd, I2C_SLAVE, addr) < 0) {
        fprintf(stderr,"Error: Could not set address to 0x%02x: %s\n",
                addr, strerror(errno));
        return -errno;
    } else {
    //	printf("%s:%d set bus to addr %x success\n",__func__,__LINE__,addr);
    }
    return 0;
}


void close_dev(int fd)
{
    close(fd);
}
