/*************************************************************************
  > File Name: gpio.c
  > Author: 
  > Mail: 
  > Created Time: Tue 29 Oct 2019 07:38:19 PM CST
 ************************************************************************/
#include <stdio.h>
#include "gpio.h"
#include <stdlib.h>
#include <string.h>


int gpio_init(gpio_class_st *gpio_class)
{
  char tmp_buf[128];
  int tmp_gpio=0;
  gpio_class_st *b_class;
  gpio_st *p_g;
  for(b_class=gpio_class;b_class&&b_class->class &&b_class->class[0]!= '\0'; b_class++)
  {
    for(p_g=b_class->gpios; p_g&&p_g->name&&p_g->name[0]!='\0'; p_g++)
    {
      tmp_gpio=b_class->base + p_g->offset;
      sprintf(tmp_buf, "echo %d >/sys/class/gpio/export ",tmp_gpio);
      system(tmp_buf);
      sprintf(tmp_buf, "echo %s >/sys/class/gpio/gpio%d/direction ",
              p_g->model == 0 ? "in" : "out", tmp_gpio);
      system(tmp_buf);
      sprintf(tmp_buf, "echo %d >/sys/class/gpio/gpio%d/active_low ",p_g->reverse, tmp_gpio);
      system(tmp_buf);
    }
  }
  return 0;
}
int gpio_set_value(int gpio, int val)
{
    char tmp_buf[128];
    FILE *fp=NULL;
    sprintf(tmp_buf,"echo %d >/sys/class/gpio/gpio%d/value",val, gpio);
    fp = popen(tmp_buf, "r");
    return pclose(fp);
}
int gpio_get_value(int num, int def)
{
  char tmp_buf[128];
  FILE *fp=NULL;
  int ret=def;
  sprintf(tmp_buf,"cat /sys/class/gpio/gpio%d/value ",num);
  fp = popen(tmp_buf, "r");
  if(fp != NULL)
  {
    if (fscanf(fp, "%d", &ret) <=0 ) {
        ret=def;
    }
    pclose(fp);
  }
  return ret;
}
int gpio_class_get_num(gpio_class_st *b_class, char *name)
{
  char tmp_buf[128];
  int tmp_gpio=0;
  gpio_st *p_g;
  for(p_g=b_class->gpios; p_g&& p_g->name[0]!='\0'; p_g++)
  {
    if(strcmp(p_g->name, name) == 0)
    {
      return  b_class->base + p_g->offset;
    }
  }
  return 0;
}


