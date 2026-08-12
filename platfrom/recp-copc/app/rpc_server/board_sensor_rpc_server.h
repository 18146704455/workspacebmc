#ifndef __BOARD_SENSOR_RPC_SERVER_H__
#define __BOARD_SENSOR_RPC_SERVER_H__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "jsonrpc-c.h"


struct sensor_per_param_str;
struct sensor_dev_str;

#define RPC_ERR         -1
#define	RPC_REMOTE_FUNC_ERR		-2
#define RPC_LOCAL_SPACE_SHORT	-3

#define RPC_FUNC_OK			0


#define CONFIG_FILE_PATH	"/etc/sysconfig/fan_rpc.ini"
//#define CONFIG_FILE_PATH	"fan_rpc.ini"

#define SECTION_NETWORK		"network"
#define SETCTION_NETWORK_KEY_PORT	"port"
#define SETCTION_SERVERIP		"serverip"

#define DEFAULT_SERVER_PORT	10234
	
#define RPC_FUNC_ERR_KEY       		"err"
#define	RPC_SENSOR_NAME_KEY			"name"
#define	RPC_SENSOR_NUM_KEY			"sensornum"
#define	RPC_SENSOR_PARAM_NUM_KEY	"num"

#define	RPC_SENSOR_PARAM_NAME_KEY	"param_name"
#define	RPC_SENSOR_PARAM_VAL_KEY	"param_val"

#define RPC_FUNC_GET_MAIN_POWER_NAME	"gMainPower"
#define RPC_FUNC_GET_POWERCHIP_NAME		"gPowerChip"
#define RPC_FUNC_GET_CHIP_POWER_NAME	"gChipPower"
#define RPC_FUNC_GET_TEMP_NAME			"gTemp"
#define RPC_FUNC_GET_FAN_NAME			"gFan"
#define RPC_FUNC_SET_FAN_NAME			"sFan"
#define RPC_FUNC_GET_LC_MAX1617_NAME    "gmax1617"
#define RPC_FUNC_GET_LC_PWR1220_NAME    "gpwr1220"
#define RPC_FUNC_GET_LC_TPS53667_NAME   "gtps53667"

#define RPC_FUNC_GET_FW                 "gVersion"
#define RPC_FUNC_SET_FW                 "sVersion"



//传感参数设置方法
typedef int (*sensor_set_ops)(struct sensor_per_param_str *sensor_param, uint32_t val);
typedef int (*sensor_get_ops)(struct sensor_per_param_str *sensor_param, uint32_t *val);
typedef struct sensor_ops_str {
    sensor_set_ops set;
    sensor_get_ops get;
} sensor_ops_str;


//传感器每个传感参数结构
typedef struct sensor_per_param_str {
    struct sensor_dev_str *dev;
    char file_name[30];
    int fd;
    uint32_t  param_val;
	mode_t mode;			//设备文件权限
}sensor_per_param_str;

//每个传感器结构
typedef struct sensor_dev_str {
    char base_dir[80];                      //设备文件目录
    char sensor_name[20];                   //sensor名
    uint8_t sensor_param_num;                  //sensor参数个数
    sensor_per_param_str *param_info;      //每种传感参数的指针
    sensor_ops_str sensor_ops;
} sensor_dev_str;

typedef struct _sensor_per_data_str {
	char label[30];			//25个字节标签信息
	uint32_t val;			//值
}sensor_per_data_str;

typedef struct _sensor_data_str {
	char name[30];			//sensro名字
	char label[20];			//sensor标签
	char num;
	sensor_per_data_str data[20];	//支持最多20个参数
}sensor_data_str;

#endif
