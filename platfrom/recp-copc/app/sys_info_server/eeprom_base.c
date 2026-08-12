/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @LastEditors: smith.zj
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "eeprom_base.h"
#include "iniparser.h" 

static dictionary  *sgIni ;
static unsigned char sgboardid=0;

#define ROOT "root"
#define SPACE_ROOT  ROOT":"
#define SN_SPACE SPACE_ROOT"sn"
#define PN_SPACE SPACE_ROOT"pn"
#define NAME_SPACE SPACE_ROOT"board_name"
#define NAME_BOARDTYPE SPACE_ROOT"board_type"
#define NAME_BOARDID SPACE_ROOT"boardid"


const char *eeprom_parser_boardname()
{
    return iniparser_getstring(sgIni,NAME_SPACE,"");
}
int set_board_name(const char *board_name)
{
    return iniparser_set(sgIni,NAME_SPACE,board_name);
}

const char *eeprom_parser_boardtype()
{
    return iniparser_getstring(sgIni,NAME_BOARDTYPE,"");
}


/******************************************************************************
* Function:         int init_eeprom
*                   init in eeprom cfg
* Where:
*                   void - TODO
* Return:           0
* Error:            <0
*****************************************************************************/
int init_eeprom(char *eepromfile)
{

    sgIni=iniparser_load(eepromfile);
    iniparser_set(sgIni,ROOT,NULL);
    return sgIni==NULL ? -1 : 0;
}

void deinit_eeprom()
{
     iniparser_freedict(sgIni);
}



int eeprom_parser_sn(char *sn)
{
    const char *tmp = iniparser_getstring(sgIni,SN_SPACE,NULL);
    if(tmp && strcmp(tmp,(char *)sn)==0) {
        return -1;
    }
    return 0; 
}

int eeprom_parser_pn(char *pn)
{
    const char *tmp = iniparser_getstring(sgIni,PN_SPACE,NULL);
    if(tmp && strcmp(tmp,(char *)pn)==0) {
        return -1;
    }
    return 0; 
}


#if 0
int main(void)
{
    uint8_t tmp[1024]={0};
    init_eeprom("/sys/bus/i2c/devices/2-0050/eeprom");
    deal_HW((uint8_t*)"fw HW_BMC",HW_BMC,true);
    deal_HW(0,HW_NPS0,true);
    deal_HW((uint8_t*)"1",HW_NPS1,true);
    deal_HW((uint8_t*)"fw HW_TOMHOCK",HW_TOMHOCK,true);
    deal_HW(tmp,HW_BMC,false);
    DB_PRINT("%s\n",tmp);
    deal_HW(tmp,HW_NPS1,false);
    DB_PRINT("%s\n",tmp);

    return 0;
}
#endif
