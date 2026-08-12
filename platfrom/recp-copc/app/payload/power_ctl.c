/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-10-24 16:12:07
 * @LastEditors: smith.zj
 * @LastEditTime: 2019-10-25 15:25:39
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include "common_log.h"
#include "pwr_phase_ctrl.h"


#define ERROR_EXIT   \
{    print_usage();  \
     return -1;      \
}

static void print_usage(void)
{
    printf("\nUsage:\n\n");
    printf("payload on\n");
    printf("payload off\n\n");
}

int powr_up_phase(powr_phase_e pwr_phase)
{
    /*控制cpld上电*/
    if(pwr_ctrl_arry[pwr_phase].pwrctrl(&pwr_ctrl_arry[pwr_phase],true) != RET_OK) {
        DB_PRINT(DB_ERR,"powr ctrl failed\n");
        return RET_FAIL;
    }
    usleep(500000);
    /*检测powr good*/
    if(pwr_ctrl_arry[pwr_phase].check_pwrgood(&pwr_ctrl_arry[pwr_phase]) != RET_OK) {
        DB_PRINT(DB_ERR,"check powr good failed\n");
        return RET_FAIL;
    }

    return RET_OK;
}

int powr_down_phase(powr_phase_e pwr_phase)
{
    /*控制cpld上电*/
    if(pwr_ctrl_arry[pwr_phase].pwrctrl(&pwr_ctrl_arry[pwr_phase],false) != RET_OK) {
        DB_PRINT(DB_ERR,"powr ctrl failed\n");
        return RET_FAIL;
    }
    return RET_OK;
}

static int payload_on(void)
{
    int pwr_phase;
    for(pwr_phase = PWR_PHS_START;pwr_phase < PWR_PHS_END;pwr_phase++) {
        
        if(powr_up_phase(pwr_phase) != 0) {
            DB_PRINT(DB_CURR_LEVEL,"ERR:power up %s failed\n",powr_phase_str[pwr_phase]);
            return -1;
        } else {
            DB_PRINT(DB_CURR_LEVEL,"power up %s success\n",powr_phase_str[pwr_phase]);
        }
    }
    return 0;
}

static int payload_off(void)
{
    int pwr_phase;
    /*下电顺序与上电相反*/
    for(pwr_phase = PWR_PHS_END-1;pwr_phase >= PWR_PHS_START;pwr_phase--) {
        if(powr_down_phase(pwr_phase) != 0) {
            DB_PRINT(DB_ERR,"payload failed\n\t power up %s failed\n",powr_phase_str[pwr_phase]);
            return -1;
        }
    }
    return RET_OK;
}

int main(int argc, char* argv[])
{

    if(init_pwrphase_ctrl() != RET_OK) {
        DB_PRINT(DB_ERR,"init pwr ctrl faile\n");
    }

    if(argc != 2) {
        ERROR_EXIT;
    }

    if(strncmp(argv[1], "on", strlen("on")) == 0) {
        payload_on();
    } else if (strncmp(argv[1], "off", strlen("off")) == 0) {
	    payload_off();
    } else {
	    ERROR_EXIT;
    }
    return 0;
}
