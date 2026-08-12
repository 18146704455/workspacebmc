/*************************************************************************
  > File Name: base.h
  > Author: 
  > Mail: 
  > Created Time: Mon 13 Jan 2020 10:32:32 AM CST
 ************************************************************************/

#ifndef _BASE_H
#define _BASE_H
#include "list.h"
struct device {
  struct list_head node_root;
    void *data;
};
#endif
