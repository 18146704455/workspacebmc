/*************************************************************************
  > File Name: temp.c
  > Author: 
  > Mail: 
  > Created Time: Wed 15 Jan 2020 09:44:21 AM CST
 ************************************************************************/
#include "base.h"
#include "list.h"
#include "config.h"
#include <stdlib.h>
#include "sys_file.h"
#include "temp.h"
#include "common_log.h"
 
static int temp_max(struct device *dev)
{
  struct temp_device *temp_info=to_temp_device(dev);
  char *path=NULL;
  asprintf(&path,"%s/temp%d_max",temp_info->path,temp_info->offset);
  int value = read_sysfs_int(path,0);
  free(path);
  return value;
}
int temp_cur(struct device *dev)
{
  struct temp_device *temp_info=to_temp_device(dev);
  char *path=NULL;
  asprintf(&path,"%s/temp%d_input",temp_info->path,temp_info->offset);
  int value = read_sysfs_int(path,0);
  free(path);
  return value;

}
void temp_register(struct def *config, struct device *dev)
{
  struct temp_device *temp_info=to_temp_device(dev);
  list_add(&temp_info->node_root,&config->temp_root);
//  get_temp_cur(dev);
    pr_debug("%p",temp_info);
  temp_info->max_temp = temp_max(dev);
}
struct temp_device *temps;
int init_temp(struct def *config)
{
  struct temp_device *dev;
  for(dev=temps;dev&&dev->match_id; dev++)
  {
    temp_register(config,&dev->dev);
  }
  return 0;
}
