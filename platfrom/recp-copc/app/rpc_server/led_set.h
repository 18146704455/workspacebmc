/*************************************************************************
    > File Name: led.h
  > Author: 
  > Mail: 
  > Created Time: Fri 16 Aug 2019 01:28:39 PM CST
 ************************************************************************/

#ifndef _LED_SET_H
#define _LED_SET_H
#include <stdbool.h>
 
typedef enum _led{
    LED_OFF=0,
    LED_BLUE,
    LED_GREEN,
    LED_YELLOW,
    LED_PURPLE,
    LED_RED
} led_e;
typedef enum _speed{
    SPEED_KEEP,
    SPEED_FAST,
    SPEED_SLOW,
} led_speed_e;

typedef enum _ctl_led {
    LED_CTL_BMC,
}led_ctl_e;

void set_sys_led_ctl(led_ctl_e );
void set_sys_led(led_e, led_speed_e, bool);

#endif
