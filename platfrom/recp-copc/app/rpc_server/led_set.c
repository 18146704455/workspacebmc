#include "led_set.h"
#include "cpld.h"
void set_sys_led_ctl(led_ctl_e ctlr)
{
    uint8_t val;
    cpld_read_byte(0x2b, &val);
    switch(ctlr)
    {
        case LED_CTL_BMC:
            val &=~(0x3f);
            val |= 3<< 6;
            break;
        default:
            break;
    }
    cpld_write_byte(0x2b, val);
}

void set_sys_led(led_e color, led_speed_e speed, bool compel)
{
   static int old_status=0;
   uint8_t check=0;
   uint8_t val=0;
   if(compel)
   {
       check +=1;
   }

   if(color > ((old_status >> 4)&0xf) )
   {
       check +=1;
   }
   if(speed > (old_status & 0xf) )
   {
       check +=1;
   }

   if(check)
   {
       old_status = (color << 4) +speed ;
   }

   cpld_read_byte(0x2b, &val); 
   switch(speed)
   {
       case SPEED_KEEP:
           check=1;
           break;
        case SPEED_SLOW:
           check=2;
           break;
        case SPEED_FAST:
           check=3;
           break;
        default:
           return ;
   }
    val &=~(0x3f<<0);
   switch(color)
   {
       case LED_OFF:
           break;
       case LED_BLUE:
           val |= check <<2;
           break;
        case LED_GREEN:
           val |= check <<0;
           break;
        case LED_PURPLE:
           val |= check <<2;
           val |= check <<4;
           break;
        case LED_YELLOW:
           val |= check <<0;
           val |= check <<4;
           break;
        case LED_RED:
           val |= check <<4;
           break;
        default:
           return;
   }
   cpld_write_byte(0x2b,val);
}
