
#ifndef _INCLUDE_DYNAMICAL_DEVS_H_
#define _INCLUDE_DYNAMICAL_DEVS_H_

#include <stdint.h>
#include <stdbool.h>
#include "msg.h"

#define SIZE_OF_DRIVER_PATH_LEN 32
#define REG_SENSOR_INFO_MAX     3

typedef struct _DynamicalI2cChannelDev_st {
  char *match_id;                 //匹配名称
  char *compatible;               //驱动匹配名称
  uint8_t bus;                    //设备总线
  uint8_t address;                //设备地址
  bool b_need_reg;                //是否被注册
  void *sub_node;
  bool (*check_ol_func)(struct _DynamicalI2cChannelDev_st *dev);      //在位检测函数
  int (*register_dev_func)(struct _DynamicalI2cChannelDev_st *dev);   //注册函数
  void (*unregister_dev_func)(struct _DynamicalI2cChannelDev_st *dev); //解注册函数
} DynamicalI2cChannelDev_st;

typedef struct _RegisterSensorInfo_st {
  char *name_prefix;        //名字前缀
  void *node;               //挂载节点
  int (*reg_sensor_info)(char *macth_id, char *driver_path, struct _RegisterSensorInfo_st *sensorinfo);   //注册函数函数
  int (*unreg_sensor_info)(char *macth_id, struct _RegisterSensorInfo_st *sensorinfo); //解注册函数函数
  uint8_t type;             //传感器类型
  uint8_t offset;           //传感器通道偏移
} RegisterSensorInfo_st;

typedef struct _I2cChannelSubDev_st {
  char *compatible;         //驱动匹配名称
  char driver_path[SIZE_OF_DRIVER_PATH_LEN];//挂在后访问路径
  uint8_t channel;          //子设备通道号
  uint8_t bus;              //子设备总线
  uint8_t address;          //子设备地址
  RegisterSensorInfo_st sensor_info[REG_SENSOR_INFO_MAX];
} I2cChannelSubDev_st;

uint8_t GetCpldBitField(uint32_t addr, uint8_t bitfield);
int RegisterMsgChipInfo(char *macth_id, char *driver_path, struct _RegisterSensorInfo_st *sensorinfo);
int UnregisterMsgChipInfo(char *macth_id, struct _RegisterSensorInfo_st *sensorinfoe);
void RegisterMultiChannelI2cDev(char *compatible, char *macth_id, uint8_t bus, uint8_t address, I2cChannelSubDev_st *devs);
void UnregisterMultiChannelI2cDev(char *macth_id, uint8_t bus, uint8_t address, I2cChannelSubDev_st *devs);

#endif
