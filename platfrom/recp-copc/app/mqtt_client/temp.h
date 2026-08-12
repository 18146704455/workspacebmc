/*************************************************************************
  > File Name: temp.h
  > Author: 
  > Mail: 
  > Created Time: Thu 16 Jan 2020 10:53:06 AM CST
 ************************************************************************/

#ifndef _TEMP_H
#define _TEMP_H
#include "base.h"
#include "list.h"
 
struct temp_device{
  char *match_id;
  char *name;
  char *path;
  int max_temp;
  int offset;
  struct list_head node_root;
  struct device dev;
  struct device_ops *ops;
};
#define to_temp_device(ptr) container_of(ptr,struct temp_device,dev)
int temp_cur(struct device *);
#endif
