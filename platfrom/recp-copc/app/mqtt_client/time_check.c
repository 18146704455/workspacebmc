/*************************************************************************
    > File Name: time_chech.c
  > Author: 
  > Mail: 
  > Created Time: Fri 16 Aug 2019 09:42:26 AM CST
 ************************************************************************/
#include "cpld.h"
#include "time_check.h"
#include "led_set.h" 
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <config.h>
  
#include "common_log.h" 
 
void set_next_timer(struct ev_loop * loop, ev_timer *w,void *cb, double next_time)
{
    if(cb)
    {
        ev_set_cb(w,cb);
    }
    w->repeat=next_time;
    ev_timer_again(loop,w);
    return ;
}
void reg_dev_check(struct device *dev, void *cb)
{
    struct dev_check *p=malloc(sizeof( struct dev_check));
    p->timer.data=p;
    p->dev=dev;
    list_add(&p->node_def,&g_config->root);
    ev_timer_init(&p->timer,cb,3,3.0);
    ev_timer_start(g_config->loop,&p->timer);
}



#define ADDR_CPU_ERRCODE0 0x111
#define ADDR_CPU_ERRCODE1 0x112

enum {
    ERRCODE0_CPU_OK=0,
    ERRCODE1_CP_OK=1,
    ERRCODE1_DP_OK=2,
    ERRCODE1_CPU_OK=3,
    ERRCODE1_RESTART_CPU=4,
};

//ev_timer_init (&fantimer, timeout_fan, 0., 10.); /* note, only repeat used */
//ev_timer_again (&fantimer); /* start timer */
//ev_run (loop, 0);

TIMER_FUNC(start_cpu);
TIMER_FUNC(led_check);
TIMER_FUNC(init_all);

TIMER_FUNC(init_all)
{

#if 0
    uint8_t val;
    cpld_write_byte(0x20, 0xff);
    system("payload on");
    //CPU 供电方式
    cpld_read_byte(0x3a,&val);
    if((val&7) == 0) val |= 1;
    cpld_write_byte(0x3a,val);
#endif
#if 1
    system("nps.sh ");
#endif
    set_next_timer(loop, w, start_cpu, 15.0);

}
TIMER_FUNC(start_cpu)
{
    pr_log("now start cpu");
    set_sys_led_ctl(LED_CTL_BMC);
    set_sys_led(LED_YELLOW, SPEED_FAST, 1);
#if 0
    system("nps.sh all");
#endif 
#if 1
    system("nps.sh ");
    //system(" cpld wr 0x30 0xdf;usleep 100000;cpld wr 0x30 0xff;transceiver -load 1 -n 4  &");
#endif
    set_next_timer(loop, w, led_check, 90);
#if 0
    uint8_t val;
    //复位
    //GEARboX
    cpld_read_byte(0x33,&val);
    val &= ~(0x3);
    cpld_write_byte(0x33,val);
    //NPS 
    cpld_write_byte(0x34, 0);
    //tcam
    cpld_write_byte(0x38, 0);
    system("payload off");
    system("payload on");
    //解复位 
    //GEARboX
    cpld_read_byte(0x33, &val);
    val |= 0x3;
    cpld_write_byte(0x33, val);


    //reboot cpu 
    cpld_write_byte(0x3a,0);
    cpld_write_byte(0x3a,1);
    //cpld_write_byte(0x38, 0x1f);
    //cpld_write_byte(0x38, 0x3f);
    /*reset nps*/
    //cpld_read_byte(0x34,&val);
    //val &= ~(0xf);
    ev_timer_start(loop, &inits[RELEASE_NPS_HW]);
#endif
}
static void led_check(struct ev_loop *loop, ev_timer *w, int revents)
{
    static int old_state = -1;
    static int error_st[1]={0};
    uint8_t val;
    val = 0;
    cpld_read_byte(ADDR_CPU_ERRCODE0,&val);
    if(val != ERRCODE0_CPU_OK)
    {
        error_st[0]+=1;
        pr_err("CPU status error once %d (cpld 0x111 val 0x%x)",error_st[0],val);
    }else{
        error_st[0]=0;
    }

    if(error_st[0]>=3)
    {
        set_next_timer(loop, w, start_cpu, 0.5);
        error_st[0]=0;
        return;
    }

    cpld_read_byte(ADDR_CPU_ERRCODE1, &val);
    if(val != old_state)
    {
        old_state = val;
        if(val == 1)
        {
            // CP ok;
            pr_log( "cp start success");
            set_sys_led(LED_BLUE, SPEED_SLOW,1);
        }else if(val == 2){
            //DP ok 
            pr_log( "nps start success");
            set_sys_led(LED_BLUE, SPEED_KEEP,1);
        }else if (val == ERRCODE1_CPU_OK){
            pr_log( "cpu start success");
            set_sys_led(LED_YELLOW, SPEED_SLOW, 1);
        }else if (val == ERRCODE1_RESTART_CPU){
            pr_log("cpu reboot");
            set_next_timer(loop,w,start_cpu,0.5);
            return;
        }else{
            pr_warn( "unknow cpld 0x112 val 0x%x",val);
        }
    }
    set_next_timer(loop, w, NULL, 1.0);
}
TIMER_FUNC(first_start)
{
    //
    uint8_t val=0;
    cpld_read_byte(ADDR_CPU_ERRCODE1, &val);
    pr_info("cpld 0x112 val 0x%x", val);
    if(val == ERRCODE1_DP_OK)
    {
        pr_info("start led_check");
        //ev_timer_init(&inits[INIT_ALL], led_check, 0, 0);
        set_next_timer(loop, w, led_check, 1.0);
    }else{
        pr_info("start init_all");
        set_next_timer(loop, w, init_all, 1.0);
        //ev_timer_init(&inits[INIT_ALL], init_all, 0, 0);
    }
    set_sys_led_ctl(LED_CTL_BMC);
    set_sys_led(LED_RED, SPEED_SLOW, 1);

}
typedef struct __led_reg {
    char *name;
    uint8_t los_reg;
    uint8_t link_reg;
    uint8_t act_reg;
}sfp_led_reg;
sfp_led_reg sfp_led[]={
    {"sfp0-7",  0xa1,0x82,0x92},
    {"sfp8-15", 0xa2,0x83,0x93},
    {"sfp16-23",0xa3,0x84,0x94},
    {"sfp24-31",0xa4,0x85,0x92},
    {0},
};
sfp_led_reg qsfp_led[]={
    {"qsfp0-7", 0xff,0x80,0x90},
    {0},
};
TIMER_FUNC(sfp_led_check)
{
    sfp_led_reg *p;
    uint8_t value;
    for(p=sfp_led;p&&p->name;p++)
    {
        cpld_read_byte(p->los_reg,&value);
        value ^= 0xff; //取反 los 是低有效
        cpld_write_byte(p->link_reg,value);
        cpld_write_byte(p->act_reg,value);
    }
    set_next_timer(loop,w,NULL,1);
}
void timer_check(struct def *config)
{
    reg_dev_check(NULL,first_start);
    //reg_dev_check(NULL,sfp_led_check);
}

