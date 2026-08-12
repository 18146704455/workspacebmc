/*************************************************************************
  > File Name: gpio.h
  > Author: 
  > Mail: 
  > Created Time: Tue 29 Oct 2019 07:39:44 PM CST
 ************************************************************************/

#ifndef _GPIO_H
#define _GPIO_H 
typedef struct _gpio_event_st{
    int gpio;
    void *priv;
    void (*gpio_change)(struct _gpio_event_st *, int);
}gpio_event_st;

typedef struct _gpio_st{
    char *name;
    int offset;
    int model;  //0 in ,1 out
    int reverse;
}gpio_st;
typedef struct _gpio_class_st {
    char *class;
    int base;
    gpio_st *gpios;
}gpio_class_st;

int gpio_class_get_num(gpio_class_st* ,char *);
int gpio_init(gpio_class_st *);
int gpio_get_value(int ,int);
int gpio_set_value(int ,int);
int gpio_event(int ,void*);
#endif
