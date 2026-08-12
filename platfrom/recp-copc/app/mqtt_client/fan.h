/*************************************************************************
  > File Name: fan.h
  > Author: 
  > Mail: 
  > Created Time: Mon 13 Jan 2020 01:41:37 PM CST
 ************************************************************************/

#ifndef _FAN_H
#define _FAN_H
#include "config.h"
 
#include "base.h"
struct fan_device {
  char *match_id;
  int slot;
  char *path;
  int pre_io;
  int led_io[2];
  int online;
  //struct eeprom_device eeprom;
  struct device dev;
  struct list_head note_root;
  struct device_ops *ops;
};
#define to_fan_device(ptr) container_of(ptr,struct fan_device,dev)

int init_fan(struct def *);
const char * get_fan_type(struct device *); 
int get_fan_online(struct device *);
int get_fan_rpm(struct device *);
int get_fan_maxrpm(struct device *);
int get_fan_pwm(struct device *);
int set_fan_pwm(struct device *,int );
int get_fan_slot(struct device *);
#endif
