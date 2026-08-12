#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "self_common.h"
#include <stdbool.h>

#define SWICH88E6155_MDIO_BUS    0
//#define SWICH88E6155_MDIO_ADDR    0xf
//#define SWICH88E6155_MDIO_BUS    1
#define SWICH88E6155_MDIO_ADDR    0x11

#define COMMAND_REGISTER                0
#define DATA_REGISTER       1


#define COMMAND_REGISTER_BSY_BIT        15     //总线空闲位，读写前先查看该位
#define COMMAND_REGISTER_MODE_BIT       12    //选定C22 or C45
#define COMMAND_REGISTER_OP_BIT         10    //操作位，10读，01写
#define COMMAND_REGISTER_DEVADDR_BIT    5    
#define COMMAND_REGISTER_REGADDR_BIT    0

#define COMMAND_READ        2
#define COMMAND_WRITE       1

#define COMMAD_READ_VAL    0x9800 
#define COMMAD_WRITE_VAL   0x9400   


#define BUS_BUSY_CHECK(command_val) ((command_val) & (1  << COMMAND_REGISTER_BSY_BIT))



extern int phy_read(char busno, char phyaddr, char regaddr, uint16_t* val);
extern int phy_write(char busno, char phyaddr, char regaddr, uint16_t val);

//检测busy是否为0
int check_completion()
{
    uint16_t command_reg;
    int32_t time_out = 0;
    int ret =0, timeout = 0;
    while(timeout++ < 10) {
        ret = phy_read(SWICH88E6155_MDIO_BUS, SWICH88E6155_MDIO_ADDR, COMMAND_REGISTER,&command_reg);
        if(ret != 0) {
            DB_PRINT(DB_ERR, "mdio read failed...");
        }
        if(BUS_BUSY_CHECK(command_reg)) {
            break;
        }
        usleep(1000);
    }
    if(time_out >= 10){
        return -1;
    } else {
        return 0;
    }
}

int fabric_switch_read(uint8_t dev,uint8_t reg,uint16_t* val)
{
    uint16_t command_reg = COMMAD_READ_VAL;
    int ret = 0;
#if 1
    if(check_completion() == -1) {
        DB_PRINT(DB_ERR, "BUS BUSY....");
        return -1;
    }
#endif
    command_reg |= ((reg & 0x1f) << COMMAND_REGISTER_REGADDR_BIT);
    command_reg |= ((dev & 0x1f) << COMMAND_REGISTER_DEVADDR_BIT);

    ret = phy_write(SWICH88E6155_MDIO_BUS, SWICH88E6155_MDIO_ADDR, COMMAND_REGISTER, command_reg);
    if(ret != 0) {
        DB_PRINT(DB_ERR, "mdio write failed...");
        return -1;
    }

    if(check_completion() == -1) {
        DB_PRINT(DB_ERR, "check completion timeout....");
        return -1;
    }

    ret = phy_read(SWICH88E6155_MDIO_BUS, SWICH88E6155_MDIO_ADDR, DATA_REGISTER, val);
    if(ret != 0) {
        DB_PRINT(DB_ERR, "mdio read failed...");
        return -1;
    }

    return 0;
}

int fabric_switch_write(uint8_t dev,uint8_t reg,uint16_t val)
{
    uint16_t command_reg = COMMAD_WRITE_VAL;
    int ret = 0;

    if(check_completion() == -1) {
        DB_PRINT(DB_ERR, "BUS BUSY....");
        return -1;
    }

    ret = phy_write(SWICH88E6155_MDIO_BUS, SWICH88E6155_MDIO_ADDR, DATA_REGISTER, val);
    if(ret != 0) {
        DB_PRINT(DB_ERR, "mdio write failed...");
        return -1;
    }

    command_reg |= ((reg & 0x1f) << COMMAND_REGISTER_REGADDR_BIT);
    command_reg |= ((dev & 0x1f) << COMMAND_REGISTER_DEVADDR_BIT);

    ret = phy_write(SWICH88E6155_MDIO_BUS, SWICH88E6155_MDIO_ADDR, COMMAND_REGISTER, command_reg);
    if(ret != 0) {
        DB_PRINT(DB_ERR, "mdio write failed...");
        return -1;
    }

    if(check_completion() == -1) {
        DB_PRINT(DB_ERR, "check completion timeout....");
        return -1;
    }

    return 0;
}


#define GLOBAL_REGISTER_DEV 0x1b
#define STATS_OPERATION_REG	0x1d
#define STATS_COUNTER_REG_LOW 0x1f
#define STATS_COUNTER_REG_HIH 0x1e
int fabric_counter(uint8_t port, uint16_t mode, bool clean,uint32_t *cnt)
{
	uint16_t cmd = 0;
	int iret = 0;
	uint16_t val = 0;

	if(port > 9) {
		DB_PRINT(DB_ERR, "invalid port....");
		return -1;
	}

	if(clean) {
		cmd = 0xac00;
		cmd |= port;
		iret = fabric_switch_write(GLOBAL_REGISTER_DEV, STATS_OPERATION_REG,cmd);
		if(-1 == iret) {
			DB_PRINT(DB_ERR, "fabric_switch_write failed....");
			return -1;
		}
	}

	//选定端口
	cmd = 0xdc00 | port;
	iret = fabric_switch_write(GLOBAL_REGISTER_DEV, STATS_OPERATION_REG,cmd);
	if(-1 == iret) {
		DB_PRINT(DB_ERR, "fabric_switch_write failed....");
		return -1;
	}


	//选定类型
	cmd = 0xcc00  | mode;
	iret = fabric_switch_write(GLOBAL_REGISTER_DEV, STATS_OPERATION_REG,cmd);
	if(-1 == iret) {
		DB_PRINT(DB_ERR, "fabric_switch_write failed....");
		return -1;
	}

	//读取计数低位
	cmd = 0xcc00  | mode;
	iret = fabric_switch_read(GLOBAL_REGISTER_DEV, STATS_COUNTER_REG_LOW,&val);
	if(-1 == iret) {
		DB_PRINT(DB_ERR, "fabric_switch_write failed....");
		return -1;
	}
	*cnt = val & 0xffff; 

	//读取计数高位
	iret = fabric_switch_read(GLOBAL_REGISTER_DEV, STATS_COUNTER_REG_HIH,&val);
	if(-1 == iret) {
		DB_PRINT(DB_ERR, "fabric_switch_write failed....");
		return -1;
	}

	*cnt |= ((val & 0xffff) << 16); 

	return 0;
}


