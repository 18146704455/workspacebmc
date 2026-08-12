/*************************************************************************
    > File Name: time_chech.h
  > Author: 
  > Mail: 
  > Created Time: Fri 16 Aug 2019 09:38:35 AM CST
 ************************************************************************/

#ifndef _TIME_CHECK_H
#define _TIME_CHECK_H
#include "config.h"
#include "base.h"
#include <ev.h>

struct dev_check{
  ev_timer timer;
  struct list_head node_def;
  struct device *dev;
};


// EV_P   struct ev_loop *loop  
void timer_check(struct def *);
void set_next_timer(struct ev_loop * loop, ev_timer *w,void *cb, double next_time);

#define TIMER_FUNC(name) static void name(struct ev_loop *loop, ev_timer *w, int revents)

void reg_dev_check(struct device *, void *);

#endif
