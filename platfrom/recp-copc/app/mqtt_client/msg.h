/*************************************************************************
  > File Name: msg.h
  > Author: 
  > Mail: 
  > Created Time: Thu 16 Jan 2020 02:42:15 PM CST
 ************************************************************************/

#ifndef _MSG_H
#define _MSG_H
#include <cJSON.h>
#include "list.h" 
typedef cJSON* (*rpc_function)(cJSON *params,void *data);
struct rpc_server{
    const char *name;
    struct list_head node_root;
    rpc_function call;
    char *help;
    void *data;
};

enum _SENSOR_DEV_TYPE {
  SENSOR_NONE = 0,
  SENSOR_TMP,
  SENSOR_VOL,
  SENSOR_CUR,
  SENSOR_PWR,
  SENSOR_BOARD_TMP,
  SENSOR_BOARD_VOL,
  SENSOR_BOARD_CUR,
  SENSOR_BOARD_PWR,
  SENSOR_CHIP_TMP,
  SENSOR_CHIP_VOL,
  SENSOR_CHIP_CUR,
  SENSOR_CHIP_PWR,
};

typedef struct _chipMsgSrcInfo_st{
  char *match_id;
  char *name;
  char *path_temperature;
  char *path_voltage;
  char *path_current;
  char *path_pwoer;
} chipMsgSrcInfo_st;

/**
 * [SetChipMsgPath 申请内存并根据信息生成对应的路径]
 * @param  msgsrc      [目的结构体]
 * @param  type        [传感器信息类型]
 * @param  driver_path [驱动目录]
 * @param  offset      [信息通道偏移]
 * @return             [description]
 */
int SetChipMsgPath(chipMsgSrcInfo_st *msgsrc, enum _SENSOR_DEV_TYPE type, char *driver_path, int offset);
/**
 * [FreeChipMsgPath 释放SetChipMsgPath申请的内存]
 * @param  msgsrc [目的结构体]
 * @param  type   [传感器信息类型]
 * @return        [description]
 */
void FreeChipMsgPath(chipMsgSrcInfo_st *msgsrc, enum _SENSOR_DEV_TYPE type);


cJSON *BuildMsgArray(cJSON *root, const char *arrayname);
int addChipMsg(cJSON *root, chipMsgSrcInfo_st *chipmsg);


#endif
