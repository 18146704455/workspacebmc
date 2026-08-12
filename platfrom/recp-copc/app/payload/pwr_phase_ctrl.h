/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-10-24 16:12:08
 * @LastEditors: smith.zj
 * @LastEditTime: 2019-10-25 13:38:17
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <linux/types.h>
#include <stdbool.h>


#define EPLD_POWER_CTRL_REG 0xfe39

typedef int         RET_STATUS;
#define RET_OK      0
#define RET_FAIL    1

typedef enum powr_phase_e {
    PWR_PHS_START = 0,
    PWR_PHS_PWR1220_PWRCTL20=PWR_PHS_START,
    PWR_PHS_XP1R8V,
    PWR_PHS_PWR1220_PWRCTL5,
    PWR_PHS_PWR1220_PWRCTL11_9,
    PWR_PHS_XP0R9V,
    PWR_PHS_XP12V_USER,
    PWR_PHS_END,
} powr_phase_e;

static char *powr_phase_str[PWR_PHS_END] = {
    "PWR_PHS_PWR1220_PWRCTL20",
    "PWR_PHS_XP1R8V",
    "PWR_PHS_PWR1220_PWRCTL5",
    "PWR_PHS_PWR1220_PWRCTL11_9",
    "PWR_PHS_XP0R9V",
    "PWR_PHS_XP12V_USER",
};

struct _powctrl_ctrl_s;
typedef struct _powctrl_ctrl_s {
    RET_STATUS (*check_pwrgood)(struct _powctrl_ctrl_s* pwrctrl);
    RET_STATUS (*check_valid)();     /*查看一下当前前面上电状态是否完成，未完成不允许下一步动作*/
    RET_STATUS (*pwrctrl)(struct _powctrl_ctrl_s* pwrctrl,bool up_down);
    powr_phase_e pwr_phase;
    char    *gpio_desc[2];             /*system中GPIO文件名*/
    uint8_t     gpio_offset[2];
} powctrl_ctrl_s;

extern powctrl_ctrl_s pwr_ctrl_arry[PWR_PHS_END];

RET_STATUS init_pwrphase_ctrl();