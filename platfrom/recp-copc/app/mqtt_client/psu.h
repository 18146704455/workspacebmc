/*************************************************************************
  > File Name: psu.h
  > Author: 
  > Mail: 
  > Created Time: Wed 15 Jan 2020 04:18:16 PM CST
 ************************************************************************/

#ifndef _PSU_H
#define _PSU_H
#include <stdint.h>
#include "base.h"
#include "list.h"

uint32_t power_curr(struct device *);
uint32_t power_in(struct device *);
uint32_t power_power(struct device *);
struct psu_device{
    char *match_id;
    char *name;
    char *path;
    int pre_io;
    int offset;
    int online;
    struct list_head node_root; // node to config->psu_root;
    struct device dev;
    struct device_ops *ops;
};
#define to_psu_device(ptr) container_of(ptr, struct psu_device, dev)

#endif
