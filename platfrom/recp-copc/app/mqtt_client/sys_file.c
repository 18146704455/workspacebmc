/*************************************************************************
  > File Name: sys_file.c
  > Author: 
  > Mail: 
  > Created Time: Tue 14 Jan 2020 05:12:26 PM CST
 ************************************************************************/
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "common_log.h" 
#define TMP_BUF_SIZE 256
static int sysfs_raw_internal(const char *device, int model,ssize_t (bc)(int,void *,size_t),char *data, int len)
{
  int fd;
  int re_len=-1;
  fd=open(device,model);
  if(fd)
    re_len = bc(fd,data,len);
  close(fd);
  return re_len;

}
static int read_sysfs_raw_internal(const char *device ,char *data, int len)
{
  return sysfs_raw_internal(device,O_RDONLY,read,data,len); 
}
static int write_sysfs_raw_internal(const char *device, char *data, int len)
{
  pr_debug("write %s :[%s]",device,data);
  return sysfs_raw_internal(device,O_WRONLY,write,data,len); 
}
static int read_sysfs_format(const char *device, const char *format, int def)
{
  int len;
  int value;
  char tmp_buf[TMP_BUF_SIZE];
  len = read_sysfs_raw_internal(device,tmp_buf,sizeof(tmp_buf));
  if(len <=0)
  {
    return def;
  }
  len=sscanf(tmp_buf,format,&value);
  return len > 0 ? value:def;

}

void read_dir_tail_int(const char *dirname, const char *prefix, int *tailnum) {
  char *_fmt=NULL;
  asprintf(&_fmt, "%s%%d", prefix);
  sscanf(dirname, _fmt, tailnum);
  free(_fmt);
}

int sysfs_get_dir_tail_num(const char *path, const char *prefix, int *tailnum) {
  DIR *dir;
  struct dirent *entry;
  int ret=-1;

  dir = opendir(path);
  while (dir && (entry=readdir(dir)) != NULL) {
    if ((entry->d_type & DT_DIR) && strstr(entry->d_name, prefix)) {
      read_dir_tail_int(entry->d_name, prefix, tailnum);
      ret = 0;
      break;
    }
  }
  closedir(dir);
  return ret;
}

int read_sysfs_int(const char *device, int def)
{
  return read_sysfs_format(device,"%i",def);
}
int read_sysfs_hex(const char *device, int def)
{
  return read_sysfs_format(device,"%x",def);
}
int read_sysfs_raw(const char *device, char* data, int len)
{
  return read_sysfs_raw_internal(device,data,len);
}
int write_sysfs_raw(const char *device, char* data, int len)
{
  return write_sysfs_raw_internal(device,data,len);
}


int get_sub_channel_i2c_bus(uint8_t bus, uint8_t address, uint8_t channel, uint8_t *realbus) {
  int _ret = 0;
  int _bus_num = -1;
  char *_buff = NULL;
  asprintf(&_buff, "/sys/bus/i2c/devices/i2c-%d/%d-00%02x/channel-%d/i2c-dev/", bus, bus, address, channel);
  if (sysfs_get_dir_tail_num(_buff, "i2c-", &_bus_num)) {
    _ret = -1;
  }
  free(_buff);
  *realbus = _bus_num;
  return _ret;
}

int get_i2c_dev_hwmon_path(uint8_t bus, uint8_t address, uint8_t *path) {
  int _ret = 0;
  char *_buff = NULL;
  int _hwmon_num;
  asprintf(&_buff, "/sys/bus/i2c/devices/i2c-%d/%d-00%02x/hwmon/", bus, bus, address);
  if (sysfs_get_dir_tail_num(_buff, "hwmon", &_hwmon_num)) {
    _ret = -1;
  }
  free(_buff);
  sprintf(path, "/sys/class/hwmon/hwmon%d", _hwmon_num);
  return _ret;
}

void register_i2c_device(char *compatible, uint8_t bus, uint8_t addr) {
  char *_buff = NULL;
  asprintf(&_buff, "echo %s 0x%x > /sys/bus/i2c/devices/i2c-%d/new_device", compatible, addr, bus);
  pr_log("%s", _buff);
  system(_buff);
  free(_buff);
}

void unregister_i2c_device(uint8_t bus, uint8_t addr) {
  char *_buff = NULL;
  asprintf(&_buff, "echo 0x%x > /sys/bus/i2c/devices/i2c-%d/delete_device", addr, bus);
  pr_log("%s", _buff);
  system(_buff);
  free(_buff);
}

