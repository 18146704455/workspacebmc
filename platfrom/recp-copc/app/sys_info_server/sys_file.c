/*************************************************************************
  > File Name: sys_file.c
  > Author: 
  > Mail: 
  > Created Time: Tue 14 Jan 2020 05:12:26 PM CST
 ************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "common_log.h" 
#define TMP_BUF_SIZE 256
static int sysfs_raw_internal(const char *filename, int model, ssize_t (bc)(int, const void *,size_t), char *data, int len)
{
  int fd;
  int re_len=-1;
  fd=open(filename,model);
  if(fd)
    re_len = bc(fd,data,len);
  close(fd);
  return re_len;
}
static int read_sysfs_raw_internal(const char *filename ,char *data, int len)
{
    return sysfs_raw_internal(filename, O_RDONLY, read, data, len); 
}
static int write_sysfs_raw_internal(const char *filename, char *data, int len)
{
    pr_debug("write %s :[%s]",filename,data);
    return sysfs_raw_internal(filename, O_WRONLY, write, data, len); 
}
static unsigned long long read_sysfs_format(const char *filename, const char *format, unsigned long long def)
{
  int len;
  unsigned long long value;
  char tmp_buf[TMP_BUF_SIZE];
  len = read_sysfs_raw_internal(filename, tmp_buf, sizeof(tmp_buf));
  if(len <=0)
  {
    return def;
  }
  len=sscanf(tmp_buf,format,&value);
  return len > 0 ? value:def;

}

char *sysfs_hwmon_dir(const char *prefix, char **hwmon_dir)
{
    DIR *dir;
    struct dirent *entry;
    char *pre_dir="/";


   asprintf(&pre_dir,"%s/hwmon",prefix);
   //pr_debug("hwmon_dir:[%s]",*hwmon_dir); 
   dir = opendir(pre_dir);
   while(dir && (entry=readdir(dir)) != NULL)
   {
       if((entry->d_type & DT_DIR)&& strstr(entry->d_name, "hwmon"))
       {
           //found the correct 'hwmon??' directory
           asprintf(hwmon_dir,"%s/hwmon/%s",prefix,entry->d_name);
           goto end;
       }
   }
   *hwmon_dir=NULL;
end:
   free(pre_dir);
   closedir(dir);
   return *hwmon_dir;
}

int read_sysfs_int(const char *filename, int def)
{
  return read_sysfs_format(filename,"%i",def);
}
long read_sysfs_long(const char *filename, long def)
{
    return read_sysfs_format(filename,"%ld", def);
}
int read_sysfs_hex(const char *filename, int def)
{
  return read_sysfs_format(filename,"%x",def);
}
int read_sysfs_raw(const char *filename, char* data, int len)
{
  return read_sysfs_raw_internal(filename,data,len);
}
int write_sysfs_raw(const char *filename, char* data, int len)
{
  return write_sysfs_raw_internal(filename,data,len);
}

int write_hwmon_raw(char *hwmondir,char *file,char *data, int len)
{
     DIR *dir;
     struct dirent *entry;
     char *pre_dir = NULL;
    char *path = NULL;
    int iret = -1;
    asprintf(&pre_dir,"%s/hwmon",hwmondir);
    //pr_debug("hwmon_dir:[%s]",*hwmon_dir);
    dir = opendir(pre_dir);
    while(dir && (entry=readdir(dir)) != NULL)
    {
        if((entry->d_type & DT_DIR)&& strstr(entry->d_name, "hwmon"))
        {
            //found the correct 'hwmon??' directory
            asprintf(&path,"%s/hwmon/%s/%s",hwmondir,entry->d_name,file);
            break;
        }
    }
    if(path == NULL)
    {
        goto end;
    }
    iret = write_sysfs_raw(path,data,len);

    
    end:
    if(dir)
        closedir(dir);
    if(pre_dir)
        free(pre_dir);
    if(path)
        free(path);
    pre_dir =NULL;
    path =NULL;
   return iret;
}
