/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-10-21 17:03:12
 * @LastEditors: smith.zj
 * @LastEditTime: 2020-08-03 16:19:26
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include "i2cctrl.h"
#include "command.h"
#include <stddef.h>
#include "IR35215.h"

extern int main_loop();

/*==================================================pcie swtich=====================================================*/
int pex_set_i2c_slave_addr(struct cmd_tbl_s *cmd_tbl, int flag, int argc, char * const argv[])
{
    int iret = -1;
    unsigned int addr = 0;
    
    char *stop = NULL;

    if(argc != 2) {
        return CMD_RET_USAGE;
    }
    addr = strtoul(argv[1], &stop, 16);
    if (*stop != '\0' || errno) {
        return CMD_RET_USAGE;
    }

    iret = ir_set_pmbus_addr((unsigned char)addr);
    if(iret == -1) {
       printf("set slave-addr 0x%x failed\n",addr);
       return CMD_RET_FAILURE;
    } else {
        printf("set slave-addr 0x%x success\n",addr);
        return CMD_RET_SUCCESS;
    }
}
int cmd_read(int argc, char * const argv[])
{
    int iret = -1;
    uint16_t reg = 0;
    uint16_t value;
    char *stop = NULL;
    
    if(argc != 3) {
        return CMD_RET_USAGE;
    }

    printf("%d\n",__LINE__);
    reg = strtoul(argv[2], &stop, 16);
    if (*stop != '\0' || errno) {
        return CMD_RET_USAGE;
    }

    printf("%d reg %#x\n",__LINE__,reg);
    iret = ir35215_MFR_read(reg,&value);
    if(iret == -1) {
       printf("read %#x failed\n",reg);
       return CMD_RET_FAILURE;
    } else {
        printf("[%#x:%#x]\n",reg,value);
        return CMD_RET_SUCCESS;
    }
}

int cmd_write(int argc, char * const argv[])
{
    int iret = -1;
    uint16_t reg = 0;
    uint16_t value = 0;
    char *stop = NULL;

    if(argc != 4) {
        return CMD_RET_USAGE;
    }

    reg = strtoul(argv[2], &stop, 16);
    if (*stop != '\0' || errno) {
        return CMD_RET_USAGE;
    }

    value = strtoul(argv[3], &stop, 16);
    if (*stop != '\0' || errno) {
        return CMD_RET_USAGE;
    }

    iret = ir35215_MFR_write(reg,value);
    if(iret == -1) {
       printf("write [%#x:%#x] failed\n",reg,value);
        return CMD_RET_FAILURE;
    } else {
       printf("write [%#x:%#x] success\n",reg,value);
       return CMD_RET_SUCCESS;
    }
}


int pex_init(void* argv)
{
    int iret = -1;    
    iret = init_i2cctrl();
    if(iret == -1){
		printf("open %s failed\n", I2C_BUS);
        return CMD_RET_FAILURE;
	}
    else
    {
        printf("open %s success\n", I2C_BUS);
    }
    return CMD_RET_SUCCESS;
}

#if   0
int main(int argc, char* argv[])
{
//    cmd_process(0, --argc, ++argv,cmd_tables,ARRAY_SIZE(cmd_tables));
    main_loop();
	return 0;
}
#endif

void pex_print_usage(void)
{
  printf("\nUsage:\n");
  printf("IR [rd] [reg]\n");
  printf("IR [wr] [reg] [value]\n");
}

int main(int argc, char* argv[])
{
    if(pex_init(NULL) != CMD_RET_SUCCESS) {
        printf("init pex controller failed\n");
        return -1;
    }

    if( 0 == strcasecmp(argv[1],"wr")){
        return cmd_write(argc,argv);
    } else if (0 == strcasecmp(argv[1],"rd")) {
        return cmd_read(argc, argv);
    } else {
        printf("unknown cmd\n");
        pex_print_usage();
        return 0;
    }

    return 0;
}
