/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-10-24 16:12:08
 * @LastEditors: smith.zj
 * @LastEditTime: 2019-10-25 15:22:42
 */
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string.h>
#include <errno.h>
#include "common_log.h"
#include "pwr_phase_ctrl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


extern int epld_read(uint32_t offset, uint8_t* value);
extern int epld_write(uint32_t offset, uint8_t value);
bool check_gpio_set(const char *desc)
{
    int fd;
    int iret;
    PRINT("%s\n",desc);
    char buf[20];
    int real_val;
    fd = open(desc,O_RDONLY);
    if(fd == -1 ) {
        DB_PRINT(DB_ERR,"ERR:open gpiofile failed(%s)\n",strerror(errno));
        return false;
    }
    iret = read(fd, buf, 10);
    if(iret == -1 ) {
        DB_PRINT(DB_ERR,"ERR:read gpiofile failed(%s)\n",strerror(errno));
        close(fd);
        return false;
    }
    close(fd);
    sscanf(buf,"%d",&real_val);
    PRINT("val %d\n", real_val);
     
    return (real_val == 1)?true:false;
}

RET_STATUS chk_powergood(powctrl_ctrl_s *pwrctrl)
{
    int i = 0;
    int retry = 10;
    for(i = 0;i < 2;i++) {
        if(pwrctrl->gpio_offset[i] == 0) {
            continue;
        }
        retry = 10;
        while(retry-- > 0) {
            if(check_gpio_set(pwrctrl->gpio_desc[i])) {
                break;
            }
            usleep(10000);
        }
        if(retry == 0 || retry < 0) {
            DB_PRINT(DB_ERR,"ERR:check power good timeout(%s)\n",powr_phase_str[pwrctrl->pwr_phase]);
            return RET_FAIL;
        }
    }
    return RET_OK;
}

RET_STATUS pwrctrl(struct _powctrl_ctrl_s* pwrctrl,bool up_down)
{
    uint8_t org_val;
    uint8_t val;
    int ret;
    ret = epld_read(EPLD_POWER_CTRL_REG,&org_val);
    if(ret != 0) {
        DB_PRINT(DB_ERR,"get cpld powr phase state failed\n");
        return RET_FAIL;
    }
    
    if(up_down) {
        val =org_val |( 1 << pwrctrl->pwr_phase);
    } else {
        val = org_val & (~(1 << pwrctrl->pwr_phase));
    }
    PRINT("org_val 0x%x val 0x%x\n",org_val,val);

    ret = epld_write(EPLD_POWER_CTRL_REG,val);
    if(ret != 0) {
        DB_PRINT(DB_ERR,"ERR:%s set cpld powr phase state failed\n",powr_phase_str[pwrctrl->pwr_phase]);
        return RET_FAIL;
    }

    return RET_OK;
}


powctrl_ctrl_s pwr_ctrl_arry[PWR_PHS_END] = {
    /*PWR_PHS_PWR1220_PWRCTL20*/
    {
        .check_pwrgood = chk_powergood,
        .pwrctrl = pwrctrl,
        .pwr_phase = PWR_PHS_PWR1220_PWRCTL20,
        .gpio_desc =   {"/sys/class/gpio/gpio331/value","/sys/class/gpio/gpio332/value"},
        .gpio_offset = {51,52},  /*GPIOG3*/
    },
    /*PWR_PHS_XP1R8V*/
    {
        .check_pwrgood = chk_powergood,
        .pwrctrl = pwrctrl,
        .pwr_phase = PWR_PHS_XP1R8V,
        .gpio_desc =    {"/sys/class/gpio/gpio330/value"},
        .gpio_offset = {50,0},  /*GPIOG2*/
    },
    /*PWR_PHS_PWR1220_PWRCTL5*/
    {
        .check_pwrgood = chk_powergood,
        .pwrctrl = pwrctrl,
        .pwr_phase = PWR_PHS_PWR1220_PWRCTL5,
        .gpio_desc =    {"/sys/class/gpio/gpio352/value"},
        .gpio_offset = {72,0},  /*GPIOJ0*/
    },
    /*PWR_PHS_PWR1220_PWRCTL11_9*/
    {
        .check_pwrgood = chk_powergood,
        .pwrctrl = pwrctrl,
        .pwr_phase = PWR_PHS_PWR1220_PWRCTL11_9,
        .gpio_desc =    {"/sys/class/gpio/gpio354/value","/sys/class/gpio/gpio354/value"},
        .gpio_offset = {74,73},/*GPIOJ2,GPIOJ1*/
    },
    /*PWR_PHS_XP0R9V*/
    {
        .check_pwrgood = chk_powergood,
        .pwrctrl = pwrctrl,
        .pwr_phase = PWR_PHS_XP0R9V,
        .gpio_desc =    {"/sys/class/gpio/gpio406/value",},
        .gpio_offset = {126,0},/*GPIOP6*/
    },
    /*PWR_PHS_XP12V_USER*/
    {
        .check_pwrgood = chk_powergood,
        .pwrctrl = pwrctrl,
        .pwr_phase = PWR_PHS_XP12V_USER,
        .gpio_offset = {0,0},/*GPIOP6*/
    },
};


//GPIOG3    51
//GPIOG2    50
//GPIOJ0    72
//GPIOJ2    74
//GPIOJ1    73
//GPIOP6    126

#define GPIO_EXPORT_STR "[ ! -f %s ] && echo %d > /sys/class/gpio/export"
#define GPIO_CTRL_STR "/sys/class/gpio%s/export"

#define GPIO_BASE   280
#define GPIOG3      (GPIO_BASE+51)
#define GPIOG2      (GPIO_BASE+50)
#define GPIOJ0      (GPIO_BASE+72)
#define GPIOJ2      (GPIO_BASE+74)
#define GPIOJ1      (GPIO_BASE+73)
#define GPIOP6      (GPIO_BASE+126)

RET_STATUS init_pwrphase_ctrl()
{
    char systembuf[256];
    int phase;
    int i = 0;
    /*使能GPIOG3 powergood测量中的缓冲器*/
    system("[ ! -f /sys/class/gpio/gpio424/value ] && echo 424 > /sys/class/gpio/export");
    system("echo out > /sys/class/gpio/gpio424/direction");

    for(phase = PWR_PHS_START;phase < PWR_PHS_END;phase++) {
        for(i = 0;i < 2;i++) {
            if(pwr_ctrl_arry[phase].gpio_offset[i] == 0) {
                continue;
            }
            sprintf(systembuf,GPIO_EXPORT_STR , pwr_ctrl_arry[phase].gpio_desc[i], (GPIO_BASE+pwr_ctrl_arry[phase].gpio_offset[i]));
            PRINT("%s:%s\n",powr_phase_str[phase],systembuf);
            system(systembuf);
        }
    }
    return RET_OK;
}
