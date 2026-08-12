#include "version.h"
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
 
 
#define HWVERSION 0x1
#define SWVERSION 0x2 
#define CPUID 0x4
#define FLAGSN 0x8
#define FLAGBOARD_NAME 0x10

static char file_name[256]={0};
static char board_name[256]={0};
static char hw_str[HW_SIZE]={0};
static char sn_str[SN_SIZE]={0};
static char sw_str[SW_SIZE]={0};
static char cpuid=0;
static void print_help(void)
{
    fprintf(stderr,"version -w -H\"HW_VERSION\" -S\"SW_VERSION\" -i\"BMC/COME/NPS0/NPS1/TOMHOCK\" -N\"sn\" -V\"board_name\"\n");
    fprintf(stderr,"version -r -H -i\n");
    fprintf(stderr,"version -r -S -i\n");
    fprintf(stderr,"version -r -H -S -i\n");
    exit(-1);
}

int main(int argc, char *argv[])
{
    int opt=0;
    int flag = 0;
    int getset = -1;
    int i=0;
    while((opt=getopt(argc,argv,"hf:H::S::N::wri::V::"))!= -1)
    {
        switch(opt)
        {
            case 'f':
                {
                    snprintf(file_name,sizeof(file_name),"%s",optarg);
                    set_eeprom_file(file_name);
                    break;
                }
            case 'H':
                {
                    flag |= HWVERSION;
                    snprintf(hw_str,sizeof(hw_str),"%s",optarg);
                    break;
                }
            case 'S':
                {
                    flag |=SWVERSION;
                    snprintf(sw_str,sizeof(sw_str),"%s",optarg);
                    break;
                }
            case 'N':
                {
                    flag |= FLAGSN;
                    snprintf(sn_str,sizeof(sn_str),"%s",optarg);
                    break;
                }
            case 'i':
                {
                    flag |= CPUID;
                    cpuid= cpuid2int(optarg);
                    break;
                }
            case 'V':
                {
                    flag |= FLAGBOARD_NAME;
                    snprintf(board_name,sizeof(board_name),"%s",optarg);
                    break;
                }
            case 'w':
                getset=1;
                break;
            case 'r':
                getset=0;
                break;
            case 'h':
            default:
                print_help();
                break;
        }
    }
    init_eeprom();
    if(flag &FLAGSN)
    {
        deal_SN((uint8_t *)sn_str,getset);
        if(!getset)printf("sn\t[%s]\n",sn_str);

    }
    if(flag & FLAGBOARD_NAME)
    {
        if(getset) set_board_name(board_name);
        if(!getset) printf("board_name\t[%s]\n",get_board_name());
    }
    for(i=0;i < HW_MAX; i++)
    {
        if((flag & CPUID) && i!= cpuid)
            continue;
        if(flag & (HWVERSION || SWVERSION))
            if(!getset) printf("[%s]\n",cpuid2str(i));
        if(flag & HWVERSION)
        {
            deal_HW((uint8_t*)hw_str,i,getset);

            if(!getset)printf("hw\t[%s]\n",hw_str);
        }
        if(flag & SWVERSION)
        {
            deal_SW((uint8_t *)sw_str,i,getset);
            if(!getset)printf("sw\t[%s]\n",sw_str);
        }
    }
    if(getset && (save_eeprom() != 0))
    {
        printf("error write version\n");
    }

    return 0;
}
