/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @LastEditors: smith.zj
 */
/*************************************************************************
  > File Name: gpio.c
  > Author: 
  > Mail: 
  > Created Time: Tue 29 Oct 2019 07:38:19 PM CST
 ************************************************************************/
#include <stdio.h>
#include "gpio.h"
#include <stdlib.h>
#include <string.h>
#include "common_log.h"
#include "sys_file.h"
int gpio_set_value(int gpio, int val)
{
    char tmp_buf[128];
    FILE *fp=NULL;
    sprintf(tmp_buf,"echo %d >/sys/class/gpio/gpio%d/value",val, gpio);
    fp = popen(tmp_buf, "r");
    pr_debug("%s",tmp_buf);
    return pclose(fp);
}
int gpio_get_value(int num, int def)
{
  char tmp_buf[128];
  FILE *fp=NULL;
  int ret=def;
  int value = 0;
  sprintf(tmp_buf,"/sys/class/gpio/gpio%d/value",num);
  
  
  /*fp = popen(tmp_buf, "r");
  if(fp != NULL)
  {
    if (fscanf(fp, "%d", &ret) <=0 ) {
        ret=def;
    }
    pclose(fp);
  }*/
  value = read_sysfs_int(tmp_buf,def);
  pr_debug("read %s %d",tmp_buf,value);
  return value;
}



