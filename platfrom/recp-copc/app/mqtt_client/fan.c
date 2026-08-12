/*************************************************************************
  > File Name: fan.c
  > Author: 
  > Mail: 
  > Created Time: Sat 11 Jan 2020 04:07:22 PM CST
 ************************************************************************/
#include "config.h"
#include "common_log.h"
 
#include "time_check.h"
#include "list.h"
#include "base.h"
#include "gpio.h"
#include "sys_file.h"
#include <stdlib.h> 
#include "fan.h"
static gpio_st io_map[]={
    {.name="pre",.model=0},
    {.name="led_b",.model=1},
    {.name="led_r",.model=1},
    {0},
};
static gpio_class_st fan_io[]={
    {
    .class="fan16_gpio",
    .gpios=io_map,
    },{0
    },
};
//return 1 online ; 0 no fan 
int get_fan_online(struct device *dev)
{
#if 1
   struct fan_device *fan_info = to_fan_device(dev);

    int ret=0;
    ret = gpio_get_value(fan_info->pre_io,1);
    
    if(ret != fan_info->online)
    {
        if(ret == 1)
        {
            // no online
            gpio_set_value(fan_info->led_io[0],1);
            gpio_set_value(fan_info->led_io[1],0);
        }else{
            gpio_set_value(fan_info->led_io[0],0);
            gpio_set_value(fan_info->led_io[1],1);

        }
    }

 return !ret; 
#else 
 return 1;
#endif
}
int get_fan_maxrpm(struct device *dev)
{
    return 12000;
}
int get_fan_rpm(struct device *dev)
{
    
   struct fan_device *fan_info = to_fan_device(dev);
    int base=fan_info->slot % 3;
    base=base<<1;

   char *path=NULL;

    int c= asprintf(&path,"%s/fan%d_input",fan_info->path,base+1);
    int value = read_sysfs_int(path,0);
    free(path);
    return value;
}
int get_fan_pwm(struct device *dev)
{
    struct fan_device *fan_info = to_fan_device(dev);
    int base=fan_info->slot % 3;
    base=base<<1;

    char *path=NULL;
    int c= asprintf(&path,"%s/%s%d",fan_info->path,"pwm",base+1);
    int value = read_sysfs_int(path,0);
    free(path);
    return value;
}
const char * get_fan_type(struct device *dev)
{
   struct fan_device *fan_info = to_fan_device(dev);
    return "max31790";
}
int get_fan_slot(struct device *dev)
{
   struct fan_device *fan_info = to_fan_device(dev);

    return fan_info->slot;
}
int set_fan_pwm(struct device* dev, int pwm)
{
    char tmp_buf[16];
    struct fan_device *fan_info = to_fan_device(dev);
    int base=fan_info->slot % 3;
    base=base<<1;

    int real_pwm = pwm *5;
    real_pwm = real_pwm>>1;
    snprintf(tmp_buf,sizeof(tmp_buf),"%d ",real_pwm); 
    char *path=NULL;
    int c= asprintf(&path,"%s/%s%d",fan_info->path,"pwm",base+1);
    write_sysfs_raw(path,tmp_buf,strlen(tmp_buf));
    free(path);

    asprintf(&path,"%s/%s%d",fan_info->path,"pwm",base+2);
    write_sysfs_raw(path,tmp_buf,strlen(tmp_buf));
    free(path);
    return 0;
}
TIMER_FUNC(check_fan)
{
    struct dev_check *p = (struct dev_check*)w;
    struct device *dev=p->dev;
    struct fan_device *fan_info;
    fan_info = to_fan_device(dev);
    int new_state=get_fan_online(dev);
    if(fan_info->online != new_state)
    {
        public_fan_msg(dev);
    }
    fan_info->online=new_state;
    set_next_timer(loop,w,NULL,1.0); 

}
extern struct def *g_config;
int fan_init(struct device *dev)
{
    struct fan_device *fan_info = to_fan_device(dev);
    pr_debug("reg %s",fan_info->path);
    io_map[0].offset=fan_info->pre_io;
    io_map[1].offset=fan_info->led_io[0];
    io_map[2].offset=fan_info->led_io[1];
    gpio_init(fan_io);
    gpio_set_value(fan_info->led_io[0],1);
    gpio_set_value(fan_info->led_io[1],0);
    fan_info->online=0;
    int base=fan_info->slot % 3;
    base=base<<1;

    list_add(&fan_info->note_root,&g_config->fan_root);

    char *path=NULL;
    asprintf(&path,"%s/pwm%d_enable",fan_info->path,base+1);
    write_sysfs_raw(path,"1 ",2);
    free(path);
    asprintf(&path,"%s/pwm%d_enable",fan_info->path,base+2);
    write_sysfs_raw(path,"1 ",2);
    free(path);
    reg_dev_check(dev,check_fan);

    return 0;
}

#if 0
void fan_register(struct def*config, struct device *dev)
{
    pr_debug();
    struct dev_check *p;
    p = (struct dev_check *)malloc(sizeof(struct dev_check ));
    if(!p)
    {
        pr_err(" error no MEM");
        //error no MEM
        return ;
    }
    p->timer.data=p;
    p->dev=dev;
    list_add(&p->node_def,&config->root);
    ev_timer_init(&p->timer,check_fan, 3,3.0);
    ev_timer_start(config->loop,&p->timer);
}
#endif
struct fan_device *fans;
int init_fan(struct def* config)
{
  pr_debug();
  struct fan_device *dev;
  for(dev=fans;dev&&dev->match_id;dev++)
  {
      pr_debug("%s",dev->match_id);
      fan_init(&dev->dev);
      //fan_register(config,&dev->dev);
  }

}
