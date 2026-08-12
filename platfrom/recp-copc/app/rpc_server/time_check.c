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
#include "board_ctrl.h"
#include <unistd.h>
 
 #include "common_log.h" 
 
typedef enum __timers{
    INIT_ALL,
    START_CPU,
    RELEASE_NPS_HW,
    RELEASE_NPS_PO,
    LED_CHECK,
    TIMER_MAX,
}timer_e;

#define ADDR_CPU_ERRCODE0 0x111
#define ADDR_CPU_ERRCODE1 0x112

enum {
    ERRCODE0_CPU_OK=0,
    ERRCODE1_CP_OK=1,
    ERRCODE1_DP_OK=2,
    ERRCODE1_CPU_OK=3,
};

static ev_timer inits[TIMER_MAX];
//ev_timer_init (&fantimer, timeout_fan, 0., 10.); /* note, only repeat used */
//ev_timer_again (&fantimer); /* start timer */
//ev_run (loop, 0);
#define TIMER_FUNC(name) static void name(struct ev_loop *loop, ev_timer *w, int revents)

TIMER_FUNC(start_cpu);
TIMER_FUNC(led_check);
TIMER_FUNC(init_all);
static void set_next_timer(struct ev_loop * loop, ev_timer *w,void *cb, double next_time)
{
    if(cb)
    {
        ev_set_cb(w,cb);
    }
    w->repeat=next_time;
    ev_timer_again(loop,w);
    return ;
}

TIMER_FUNC(init_all)
{

    uint8_t val;
    cpld_write_byte(0x20, 0xff);
    system("payload on");
    //CPU 供电方式
    cpld_read_byte(0x3a,&val);
    if((val&7) == 0) val |= 1;
    cpld_write_byte(0x3a,val);
    set_sys_led_ctl(LED_CTL_BMC);
    set_sys_led(LED_RED, SPEED_SLOW, 1);
    set_next_timer(loop, w, start_cpu, 15.0);

}
TIMER_FUNC(start_cpu)
{
    DB_PRINT(DB_WARN,"now start cpu");
    set_sys_led_ctl(LED_CTL_BMC);
    set_sys_led(LED_YELLOW, SPEED_FAST, 1);
#if 0
    board_init();
#else 
    system("nps.sh all");
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
#if 0
TIMER_FUNC(release_nps_hw)
{
    DB_PRINT(DB_WARN, "release_nps_hw \n");
    //uint8_t val;
    //cpld_read_byte(0x34,&val);
    //val &= ~(0xf);
    //val |= 0xa;
    cpld_write_byte(0x34,0xfa);
    ev_timer_start(loop, &inits[RELEASE_NPS_PO]);
}
TIMER_FUNC(release_nps_po)
{
    DB_PRINT(DB_WARN, "release_nps_po \n");
    uint8_t val;
    cpld_read_byte(0x34,&val);
    val |= 0x5;
    cpld_write_byte(0x34,val);
    ev_timer_start(loop, &inits[LED_CHECK]);

}
#endif
static void led_check(struct ev_loop *loop, ev_timer *w, int revents)
{
    static int old_state = -1;
    uint8_t val;
    val = 0;
    cpld_read_byte(ADDR_CPU_ERRCODE0,&val);
    if(val != ERRCODE0_CPU_OK)
    {
        DB_PRINT(DB_WARN, "CPU status error (cpld 0x111 val 0x%x)",val);
        set_next_timer(loop, w, start_cpu, .5);
        return;
    }
    cpld_read_byte(ADDR_CPU_ERRCODE1, &val);
    if(val != old_state)
    {
        old_state = val;
        if(val == 1)
        {
            // CP ok;
            DB_PRINT(DB_INFO, "cp start success");
            set_sys_led(LED_BLUE, SPEED_SLOW,1);
        }else if(val == 2){
            //DP ok 
            DB_PRINT(DB_INFO, "nps start success");
            set_sys_led(LED_BLUE, SPEED_KEEP,1);
        }else if (val == ERRCODE1_CPU_OK){
            DB_PRINT(DB_INFO, "cpu start success");
            set_sys_led(LED_YELLOW, SPEED_SLOW, 1);
        }else{
            DB_PRINT(DB_WARN, "unknow cpld 0x112 val 0x%x",val);
        }
    }
    set_next_timer(loop, w, NULL, 1.0);
}

void timer_check(struct ev_loop *loop)
{

//    ev_timer_init(&fantimer, timeout_fan, 1., 1.); /* note, only repeat used */
//    ev_timer_start(loop, &fantimer);
    uint8_t val=0;
    cpld_read_byte(ADDR_CPU_ERRCODE1, &val);
    if(val == ERRCODE1_DP_OK)
    {
        ev_timer_init(&inits[INIT_ALL], led_check, 0, 0);
    }else{
        ev_timer_init(&inits[INIT_ALL], init_all, 0, 0);
    }
#if 0
    ev_timer_init(&inits[START_CPU],start_cpu, 0.5, 0);
    ev_timer_init(&inits[RELEASE_NPS_HW], release_nps_hw, 5.0, 0);
    ev_timer_init(&inits[RELEASE_NPS_PO], release_nps_po, 0.5, 0);
    ev_timer_init(&inits[LED_CHECK], led_check, 30.0, 1);
#endif 
    ev_timer_start(loop, &inits[INIT_ALL]);
}

