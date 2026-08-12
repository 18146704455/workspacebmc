/*************************************************************************
  > File Name: sys_file.h
  > Author: 
  > Mail: 
  > Created Time: Tue 14 Jan 2020 08:34:56 PM CST
 ************************************************************************/

#ifndef _SYS_FILE_H
#define _SYS_FILE_H
int read_sysfs_int(const char *, int);
long read_sysfs_long(const char *, long);
int read_sysfs_hex(const char *,int);
int read_sysfs_raw(const char *device, char* data, int len);
int write_sysfs_raw(const char *device, char* data, int len);
char *sysfs_hwmon_dir(const char *path, char **hwmon_dir);
int write_hwmon_raw(char *hwmondir,char *file,char *data, int len);

#endif
