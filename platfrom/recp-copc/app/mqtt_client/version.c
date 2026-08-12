#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "version.h"
#include "cpld.h"
 
#include "iniparser.h" 
#if 0
enum HWID {
    HW_BMC,
    HW_NPS0,
    HW_NPS1,
    HW_TOMHOCK,
    HW_COME,
    HW_MAX
};
enum SWID {
    SW_BMC,
    SW_NPS0,
    SW_NPS1,
    SW_CTR7340,
    SW_COME,
    SW_MAX
};
#define SN_SIZE 16
#define HW_SIZE 32
#define SW_SIZE 33
#endif
static dictionary  *sgIni ;
static unsigned char sgsoldid;
static unsigned char sgboardid=0;
static char EEPROM_FILE[PATH_SIZE]={0};
#define DEF_EEPROM_FILE "/sys/bus/i2c/devices/2-0050/eeprom"
//#define DEF_EEPROM_FILE "./eeprom.ini"
#define ROOT "root"
#define SPACE_ROOT  ROOT":"
#define SN_SPACE SPACE_ROOT"sn"
#define NAME_SPACE SPACE_ROOT"board_name"
const char * cpuid_str[]={
    [HW_BMC]="BMC",
    [HW_COME]="COME",
    [HW_NPS0]="NPS0",
    [HW_NPS1]="NPS1",
    [HW_TOMHOCK]="TOMHOCK",
};
const char * SW_SPACE[]={
    [SW_BMC] = SPACE_ROOT"SW_BMC",
    [SW_NPS0] = SPACE_ROOT"SW_NPS0",
    [SW_NPS1] = SPACE_ROOT"SW_NPS1",
    [SW_CTR7340] = SPACE_ROOT"SW_CTR7340",
    [SW_COME]   = SPACE_ROOT"SW_COME"
};
const char * HW_SPACE[]={

    [HW_BMC]= SPACE_ROOT"HW_BMC",
    [HW_NPS0] = SPACE_ROOT"HW_NPS0",
    [HW_NPS1] = SPACE_ROOT"HW_NPS1",
    [HW_TOMHOCK] = SPACE_ROOT"HW_TOMHOCK",
    [HW_COME] = SPACE_ROOT"HW_COME",
};
#if 0
/******************************************************************************
* Function:         uint8_t get_board_id
*                   get board id
* Where:
*                   void - TODO
* Return:           board id
* Error:            < 0
*****************************************************************************/
int8_t get_board_id(void){
    int fd;
    int value;
   if(sgboardid == 0)
   {
       fd = i2c_open("/dev/i2c-12",0x27);
       if(fd < 0) return -errno;
        value = i2c_smbus_read_byte_data(fd,0);
        if(value >= 0)
        {
            sgboardid = ((uint8_t) value) >>1;
        }

        i2c_close(fd);
      
   }
    
    return sgboardid;
}
int8_t get_slot_id(){
    return (int8_t)sgsoldid;
}
#endif
const char *get_board_name(){
    return iniparser_getstring(sgIni, NAME_SPACE, "");
}
int set_board_name(const char *board_name){
    return iniparser_set(sgIni, NAME_SPACE,board_name);
}
enum HWID cpuid2int(char *name)
{
    int i=0;
    for(i=0;i<HW_MAX;i++)
    {
        if(strcmp(cpuid_str[i],name)==0)
        {
            break;
        }
    }
    return i;
}
const char *cpuid2str(enum HWID id)
{
    if(id>=HW_MAX || id <0)
        return "unknown";
    return cpuid_str[id];
}
/******************************************************************************
* Function:         int init_eeprom
*                   init in eeprom cfg
* Where:
*                   void - TODO
* Return:           0
* Error:            <0
*****************************************************************************/
int init_eeprom(void){
    if(*EEPROM_FILE=='\0') sprintf(EEPROM_FILE,"%s",DEF_EEPROM_FILE);
    sgIni=iniparser_load(EEPROM_FILE);
    iniparser_set(sgIni,ROOT,NULL);
    cpld_read_byte(0x21,&sgsoldid);
    sgsoldid = sgsoldid&0xf;
    return sgIni==NULL ? -1 : 0;
}

/******************************************************************************
* Function:         int save_eeprom
* Description:      save eeprom
* Where:
*                   void - TODO
* Return:           0
* Error:            <0
*****************************************************************************/
int save_eeprom(void){
#define EEPROM_SIZE 8*1024
    int tmp=128,offset;
    FILE *fp= fopen(EEPROM_FILE,"w");
    if(fp)
    {
        iniparser_dump_ini(sgIni,fp);
        offset=EEPROM_SIZE - ftell(fp);
        while(offset--)
        {
            tmp--;
            if(tmp==0 || offset==1)
            {
                fwrite("\n",1,1,fp);
                tmp=128;
            }else{
                fwrite(" ",1,1,fp);
            }
        }
        fclose(fp);
        return 0;
    }
    return -1;
    
}
/******************************************************************************
* Function:         int check_
* Description:      check sn
* Where:
*                   uint8_t *sn - TODO
* Return:           1
* Error:            <0
*****************************************************************************/
static int check_sn(uint8_t *sn){
    //now don`t how to check 
        const char *tmp = iniparser_getstring(sgIni,SN_SPACE,NULL);
        if(tmp && strcmp(tmp,(char *)sn)==0)
        {
            return -1;
        }
   return 1; 
}
int set_eeprom_file(char *file_name)
{
    return snprintf(EEPROM_FILE,sizeof(EEPROM_FILE),"%s",file_name);
}
#if 0
static int check_fw(enum HWID cpuid)
{
    //return cpuid == iniparser_getint(sgIni,HWID_SPACE,-1) ? 1 :-1;
    return 1;
}
#endif
/******************************************************************************
* Function:         int deal_SN
*                   get/set SN
* Where:
*                   uint8_t *sn - TODO
*                   bool getset - TODO get false ; set true
* Return:           0
* Error:            <0
*****************************************************************************/
int deal_SN(uint8_t *sn,bool getset){
    const char *tmp;
    if (getset) 
    {
        if(!check_sn(sn))
        {
            return -1;
        }
        return iniparser_set(sgIni,SN_SPACE,(const char *)sn);
    } else {
        *sn='\0';
        tmp = iniparser_getstring(sgIni,SN_SPACE,"");
        if(*tmp!='\0')
        {
            snprintf((char *)sn,SN_SIZE,"%s",tmp);
            return 0;
        }else{
            return -1;
        }
    }
}

/******************************************************************************
* Function:         int deal_FW
*                   set fw
* Where:
*                   uint8_t *fw - TODO
*                   enum HWID cpuid - TODO
*                   bool getset - TODO
* Return:           0
* Error:            <0
*****************************************************************************/
int deal_HW(uint8_t *fw, enum HWID cpuid, bool getset)
{
    const char *tmp;
    tmp = iniparser_getstring(sgIni,HW_SPACE[cpuid],"");
    if(getset && strcmp(tmp,(char*)fw) != 0)
    {
        return iniparser_set(sgIni,HW_SPACE[cpuid],(const char*) fw);
    }
    if(!getset)
    {
        *fw='\0';
        if(*tmp == '\0')
        {
            return -1;
        }
        snprintf((char*)fw,HW_SIZE,"%s",tmp);
        return 0;

    }
    return -1;
}
/******************************************************************************
* Function:         int deal_SW
* Description:      
* Where:
*                   uint8_t *sw - TODO
*                   enum SWID swid - TODO
*                   bool getset - TODO
* Return:           
* Error:            
*****************************************************************************/
int deal_SW(uint8_t *sw, enum SWID swid,bool getset)
{
    const char *tmp;
    tmp = iniparser_getstring(sgIni,SW_SPACE[swid],"");
    if(getset && strcmp(tmp,(char*)sw) != 0)
    {
        return iniparser_set(sgIni,SW_SPACE[swid],(const char*) sw);
    }
    if(!getset)
    {
        *sw='\0';
        if(*tmp == '\0')
        {
            return -1;
        }
        snprintf((char *)sw,SW_SIZE,"%s",tmp);
        return 0;

    }
    return -1;
    
}

#if 0
int main(void)
{
    uint8_t tmp[1024]={0};
    init_eeprom();
    deal_HW((uint8_t*)"fw HW_BMC",HW_BMC,true);
    deal_HW(0,HW_NPS0,true);
    deal_HW((uint8_t*)"1",HW_NPS1,true);
    deal_HW((uint8_t*)"fw HW_TOMHOCK",HW_TOMHOCK,true);
    deal_HW(tmp,HW_BMC,false);
    printf("%s\n",tmp);
    deal_HW(tmp,HW_NPS1,false);
    printf("%s\n",tmp);

    return 0;
}
#endif
