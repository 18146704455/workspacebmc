/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-09-27 15:43:11
 * @LastEditors: smith.zj
 * @LastEditTime: 2019-09-27 15:53:09
 */
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include "common_log.h"
#include <stdio.h>
#include <unistd.h>
#include "cpld.h"
#include <stdbool.h>
#include "board_ctrl.h"
#include <stdint.h>
#include <unistd.h>

static char nps_cpld_reset_reg_bit_val[NPS_ID_INVALID][NPS_CTRL_INVALID] = {
    {0x2,0x1},
    {0x8,0x4},
    {0xa,0x5},
};

static int nps_ctrl_reset(NPS_iD nps_id,NPS_RST_CTRL cmd,bool release_rst)
{
    int iret;
    int reg = NPS_CPLD_RESET_REG;
    unsigned char val  = 0; 
    unsigned char orgval = 0;
    unsigned char nps_ctrl_val;

    DB_PRINT(DB_DEBUG, "CTRL npsid %d NPS_RST_CTRL %d rst %s ",nps_id,cmd,release_rst?"release":"reset");
    if(nps_id < NPS_ID_0 || nps_id >= NPS_ID_INVALID ) {
        DB_PRINT(DB_ERR, "UNKNOWN NPS_ID  %d",nps_id);
        return -1;
    }
    if(cmd < NPS_CTRL_HW_RST || cmd >= NPS_CTRL_INVALID ) {
        DB_PRINT(DB_ERR, "UNKNOWN NPS_RST_CTRL CMD %d",cmd);
        return -1;
    }
    //读取原有配置
    iret = cpld_read_byte(reg,(unsigned char*)&orgval);
    if(iret == -1) {
        DB_PRINT(DB_ERR,"ERR:get org enable status failed");
        return -1;
    }

    //release情况下 提前解复位内存颗粒
    if(release_rst) {
        iret = cpld_write_byte(reg,orgval | 0xf0);
        if(iret == -1) {
            DB_PRINT(DB_ERR,"ERR:write cpld error");
            return -1;
        }
    }

    //读取原有配置
    iret = cpld_read_byte(reg,(unsigned char*)&orgval);
    if(iret == -1) {
        DB_PRINT(DB_ERR,"ERR:get org enable status failed");
        return -1;
    }

    nps_ctrl_val = nps_cpld_reset_reg_bit_val[nps_id][cmd];
    if(release_rst) {
        val = orgval | nps_ctrl_val;
    } else {
        //这里需要控制内存颗粒复位
        val = orgval & (~nps_ctrl_val) & (0xf);
    }

    DB_PRINT(DB_DEBUG,"org val 0x%x now val 0x%x",orgval,val);
    iret = cpld_write_byte(reg,val);
    if(iret == -1) {
        DB_PRINT(DB_ERR,"ERR:write cpld error");
        return -1;
    }

    return 0;
}


int nps_release_po_rst(NPS_iD nps_id)
{
    return nps_ctrl_reset(nps_id,NPS_CTRL_PO_RST,true);
}
int nps_release_hw_rst(NPS_iD nps_id)
{
    return nps_ctrl_reset(nps_id,NPS_CTRL_HW_RST,true);
}

int nps_reset_po_rst(NPS_iD nps_id)
{
    return nps_ctrl_reset(nps_id,NPS_CTRL_PO_RST,false);
}

int nps_reset_hw_rst(NPS_iD nps_id)
{
    return nps_ctrl_reset(nps_id,NPS_CTRL_HW_RST,false);
}

int nps_reset_all_po_rst()
{
    return nps_ctrl_reset(NPS_ID_ALL,NPS_CTRL_PO_RST,false);
}

int nps_reset_all_hw_rst()
{
    return nps_ctrl_reset(NPS_ID_ALL,NPS_CTRL_HW_RST,false);
}

int nps_release_all_po_rst()
{
    return nps_ctrl_reset(NPS_ID_ALL,NPS_CTRL_PO_RST,true);
}

int nps_release_all_hw_rst()
{
    return nps_ctrl_reset(NPS_ID_ALL,NPS_CTRL_HW_RST,true);
}

/***************TCAM控制*******************/
static char tcam_ctrl_bit[TCAM_ID_INVALID][TCAM_CTRL_INVALID]={
    {0x1,0x2},
    {0x4,0x8},
    {0x5,0xa}
};
int tcam_ctrl_reset(TCAM_ID tcam_id,TCAM_RST_CTRL cmd, bool release_rst)
{
    int reg;
    uint8_t value;
    int iret;
    char tmp;

    DB_PRINT(DB_DEBUG, "CTRL tcamid %d TCAM_RST_CTRL %d rst %s ",tcam_id,cmd,release_rst?"release":"reset");
    if(tcam_id < TCAM_ID_0 || tcam_id >= TCAM_ID_INVALID ) {
        DB_PRINT(DB_ERR, "UNKNOWN TCAM_ID  %d",tcam_id);
        return -1;
    }
    if(cmd < TCAM_CTRL_C_RST || cmd >= TCAM_CTRL_INVALID ) {
        DB_PRINT(DB_ERR, "UNKNOWN TCAM_RST_CTRL CMD %d",cmd);
        return -1;
    }

    reg = TCAM_CPLD_RESET_REG; 
    iret = cpld_read_byte(reg, &value);
    if(iret == -1) {
        DB_PRINT(DB_ERR,"ERR:read cpld error");
        return -1;
    }

    tmp=tcam_ctrl_bit[tcam_id][cmd];
    value &= ~tmp ;
    if(release_rst)
    {
        value |= tmp;
    }
    iret = cpld_write_byte(reg,value);
    if(iret == -1)
    {
        DB_PRINT(DB_ERR,"ERR:write cpld error");
        return -1;
    }
    return 0;
}
#if 0
int tcam_reset_ctrl(TCAM_ID tcam_id, RST_CTRL s_rst, RST_CTRL c_rst )
{
    reg=TCAM_CPLD_RESET_REG;
    cpld_read_byte(reg, &value);
    if(s_rst != CTRL_KEEP)
    {
        value &= ~(tcam_cpld_reset_reg_bit_val[tcam_id][])
    }
}
#endif
/***************gearbox控制*******************/
static char gearbox_ctrl_bit[GEARBOX_ID_INVALID][GEARBOX_CTRL_INVALID]={
    {0x1},
    {0x2},
    {0x3},
};

int gearbox_reset_ctrl(GEARBOX_ID gearbox_id,GEARBOX_RST_CTRL cmd, bool release_rst)
{

    int reg = GEARBOX_CPLD_RESET_REG;
    int iret;
    char tmp;
    uint8_t value;
    DB_PRINT(DB_DEBUG, "CTRL gearboxid %d GEARBOX_RST_CTRL %d rst %s ",gearbox_id,cmd,release_rst?"release":"reset");
    if(gearbox_id < GEARBOX_ID_0 || gearbox_id >= GEARBOX_ID_INVALID ) {
        DB_PRINT(DB_ERR, "UNKNOWN GEARBOX_ID  %d",gearbox_id);
        return -1;
    }
    if(cmd < GEARBOX_CTRL_RST || cmd >= GEARBOX_CTRL_INVALID ) {
        DB_PRINT(DB_ERR, "UNKNOWN GEARBOX_RST_CTRL CMD %d",cmd);
        return -1;
    }
    iret = cpld_read_byte(reg,&value);
    if(iret == -1)
    {
        DB_PRINT(DB_ERR, "ERR:read cpld error");
        return -1;
    }
    tmp = gearbox_ctrl_bit[gearbox_id][cmd];
    value &= ~tmp;
    if(release_rst)
    {
        value |= tmp;
    }
    iret = cpld_write_byte(reg,value);
    if(iret == -1)
    {
        DB_PRINT(DB_ERR, "ERR:write cpld error");
        return -1;
    }

    return 0;

}

/*****************COMe CPU控制***********************/
int cpu_ctrl(CPU_CTRL cmd)
{
    int value,iret;
    uint32_t reg = CPU_CPLD_CTRL_REG;
#define CPU_BIT(x) (1<<x)
    if(cmd <0 || cmd > CPU_CTRL_INVALID)
    {
        DB_PRINT(DB_ERR, "UNKNOWN CPU_CTRL CMD %d",cmd);
    }
    value = CPU_BIT(cmd);
    if(cmd == CPU_CTRL_RESTART)
    {
        value = CPU_BIT(CPU_CTRL_STOP) | CPU_BIT(CPU_CTRL_START);
    }
    if(value & CPU_BIT(CPU_CTRL_STOP))
    {
        iret = cpld_write_byte(reg,0);
        if(iret == -1)
        {
            DB_PRINT(DB_ERR, "ERR:write cpld reg [0x%x] error",reg);
            return -1;
        }
    }
    if(value & CPU_BIT(CPU_CTRL_START))
    {
        iret = cpld_write_byte(reg,1);
        if(iret == -1)
        {
            DB_PRINT(DB_ERR, "ERR:write cpld reg [0x%x] error",reg );
            return -1;
        }
    }
#undef CPU_BIT
    return 0;
}
int board_init()
{
    int iret=0;
    iret |= nps_reset_all_po_rst();
    iret |= nps_reset_all_hw_rst();

    iret |= tcam_reset_all_s_rst();
    iret |= tcam_reset_all_c_rst();

    iret |= cpu_ctrl(CPU_CTRL_STOP);

    iret |= gearbox_reset_all_res();

    system("payload off");
    system("payload on");
    
    iret |= cpu_ctrl(CPU_CTRL_RESTART);
    sleep(6);
    iret |= nps_release_all_hw_rst();
    usleep(300000);
    iret |= nps_release_all_po_rst();

    return iret;
}
