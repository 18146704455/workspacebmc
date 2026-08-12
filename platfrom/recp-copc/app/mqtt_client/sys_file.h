/*************************************************************************
  > File Name: sys_file.h
  > Author: 
  > Mail: 
  > Created Time: Tue 14 Jan 2020 08:34:56 PM CST
 ************************************************************************/

#ifndef _SYS_FILE_H
#define _SYS_FILE_H

#include <stdint.h>

int read_sysfs_int(const char *, int);
int read_sysfs_hex(const char *,int);
int read_sysfs_raw(const char *device, char* data, int len);
int write_sysfs_raw(const char *device, char* data, int len);

int get_sub_channel_i2c_bus(uint8_t bus, uint8_t address, uint8_t channel, uint8_t *realbus);
int get_i2c_dev_hwmon_path(uint8_t bus, uint8_t address, uint8_t *path);
void register_i2c_device(char *compatible, uint8_t bus, uint8_t addr);
void unregister_i2c_device(uint8_t bus, uint8_t addr);

#endif
