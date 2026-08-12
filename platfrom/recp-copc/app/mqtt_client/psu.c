/*************************************************************************
  > File Name: psu.c
  > Author: 
  > Mail: 
  > Created Time: Mon 13 Jan 2020 02:36:38 PM CST
 ************************************************************************/
#include "config.h"
#include <stdlib.h>
#include <stdint.h>
#include "sys_file.h"
#include "time_check.h"
#include "gpio.h"
#include "psu.h"
#include "common_log.h"

#define LC03_CURRENT_MULTIPLE    4
  
uint32_t power_curr(struct device *dev)
{
    struct psu_device *psu_info= to_psu_device(dev);
    char *path=NULL;
    asprintf(&path,"%s/curr%d_input",psu_info->path,psu_info->offset);
    uint32_t value = read_sysfs_int(path,0);
    value *= LC03_CURRENT_MULTIPLE;
    free(path);
    return value;
}
uint32_t power_in(struct device *dev)
{
    struct psu_device *psu_info= to_psu_device(dev);
    char *path=NULL;
    asprintf(&path,"%s/in%d_input",psu_info->path,psu_info->offset);
    uint32_t value = read_sysfs_int(path,0);
    free(path);
    return value;
}
uint32_t power_power(struct device *dev)
{
    struct psu_device *psu_info= to_psu_device(dev);
    char *path=NULL;
    asprintf(&path,"%s/power%d_input",psu_info->path,psu_info->offset);
    uint32_t value = read_sysfs_int(path,0);
    value *= LC03_CURRENT_MULTIPLE;
    free(path);
    return value;
}
static int power_online(struct device *dev)
{
    struct psu_device *psu_info= to_psu_device(dev);
    if(psu_info->pre_io < 0) return 1;
    return gpio_get_value(psu_info->pre_io,0);

}
static int power_alarm(struct device *dev,char **alarm)
{
    *alarm=NULL;
    return 0;
}
TIMER_FUNC(power_check)
{
    char *alarm;
    struct dev_check *p = (struct dev_check*)w;
    struct device *dev=p->dev;
    struct psu_device *psu_info=to_psu_device(dev);
    int new_state = power_online(dev);
    if(psu_info->online != new_state )
    {
        public_psu_msg(dev);
    }
    if(power_alarm(dev,&alarm))
    {
        public_alarm_msg(alarm);
        free(alarm);
    }

    set_next_timer(loop,w,NULL,1);
}
static gpio_st psu_map[]={
    {.name="pre",.model=0},
    {0},
};
static gpio_class_st psu_io[]={
    {
        .class="psu_gpio",
        .gpios=psu_map,
    },
    {0},
};
void power_register(struct def *config, struct device *dev)
{
    struct psu_device *psu_info= to_psu_device(dev);
    pr_debug("reg %p",psu_info);
    pr_debug("match_id %s",psu_info->match_id);
    pr_debug("name %s",psu_info->name);
    if(psu_info->pre_io > 0)
    {
        psu_map[0].offset=psu_info->pre_io;
        gpio_init(psu_io);
        reg_dev_check(dev,power_check);
    }
    list_add(&psu_info->node_root,&config->psu_root);
        
}
struct psu_device *psus;
int init_power(struct def *config)
{
    pr_debug("reg %p",psus);
    pr_debug("statr");
    struct psu_device   *dev;
    for(dev=psus;dev&&dev->match_id;dev++)
    {
        pr_debug("reg %p",dev);
        power_register(config,&dev->dev);
    }

    pr_debug("end");
}
