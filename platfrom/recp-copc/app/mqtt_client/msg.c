/*************************************************************************
  > File Name: msg.c
  > Author: 
  > Mail: 
  > Created Time: Mon 30 Dec 2019 08:37:33 PM CST
 ************************************************************************/
#include <sys/time.h>
#include "config.h"
 
#include "base.h"
#include "fan.h"
#include "mqtt_client.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "cJSON.h"
#include "list.h"
#include "psu.h"
#include "temp.h"
#include "common_log.h"
#include "msg.h"
#include "fan.h"
#include  "board.h"

/**
 * [SetChipMsgPath 申请内存并根据信息生成对应的路径]
 * @param  msgsrc      [目的结构体]
 * @param  type        [传感器信息类型]
 * @param  driver_path [驱动目录]
 * @param  offset      [信息通道偏移]
 * @return             [description]
 */
int SetChipMsgPath(chipMsgSrcInfo_st *msgsrc, enum _SENSOR_DEV_TYPE type, char *driver_path, int offset) {
  char *format = NULL;
  char **dest = NULL;
  switch(type) {
    case SENSOR_CHIP_TMP:
      dest = &msgsrc->path_temperature;
      format = "%s/temp%d_input";
      break;
    case SENSOR_CHIP_VOL:
      dest = &msgsrc->path_voltage;
      format = "%s/in%d_input";
      break;
    case SENSOR_CHIP_CUR:
      dest = &msgsrc->path_current;
      format = "%s/curr%d_input";
      break;
    case SENSOR_CHIP_PWR:
      dest = &msgsrc->path_pwoer;
      format = "%s/power%d_input";
      break;
    default:
      break;
  }
  if (dest && format) {
    if (*dest) {
      free(*dest);
    }
    asprintf(dest, format, driver_path, offset);
    // printf("%x, %x, %x, %x --%x: %s\n", &msgsrc->path_temperature, &msgsrc->path_voltage, &msgsrc->path_current, &msgsrc->path_pwoer, dest, *dest);
    return 0;
  }
  return -1;
}
/**
 * [FreeChipMsgPath 释放SetChipMsgPath申请的内存]
 * @param  msgsrc [目的结构体]
 * @param  type   [传感器信息类型]
 * @return        [description]
 */
void FreeChipMsgPath(chipMsgSrcInfo_st *msgsrc, enum _SENSOR_DEV_TYPE type) {
  char **dest = NULL;
  switch(type) {
    case SENSOR_CHIP_TMP:
      dest = &msgsrc->path_temperature;
      break;
    case SENSOR_CHIP_VOL:
      dest = &msgsrc->path_voltage;
      break;
    case SENSOR_CHIP_CUR:
      dest = &msgsrc->path_current;
      break;
    case SENSOR_CHIP_PWR:
      dest = &msgsrc->path_pwoer;
      break;
    default:
      break;
  }
  if (dest) {
    // printf("%x, %x, %x, %x free %x: %s\n", &msgsrc->path_temperature, &msgsrc->path_voltage, &msgsrc->path_current, &msgsrc->path_pwoer, dest, *dest);
    if (*dest) {
      free(*dest);
    }
    *dest = NULL;
  }
}


int reg_server(struct def* config, rpc_function call,const char *name, void *data)
{
    struct rpc_server *p=malloc(sizeof(struct rpc_server));
    if(p)
    {
        p->name=name;
        p->call=call;
        p->data=data;
        list_add(&p->node_root,&config->server_root);
    }

}

static uint32_t time_stamp()
{

    struct timeval time;
    gettimeofday(&time,NULL);
    return time.tv_sec*100+time.tv_usec/10000;
}
cJSON*create_msg(struct device *dev)
{
    char *p=NULL;
    asprintf(&p,"%s %d",g_config->board_type,g_config->slot);
    cJSON *root=cJSON_CreateObject();
    cJSON_AddStringToObject(root,"version","v0.01");
    cJSON_AddNumberToObject(root,"timeStamp", time_stamp());
    if(p)
        cJSON_AddStringToObject(root,"id",p);
    free(p);
    return root;


}
int public_fan_msg(struct device *dev)
{
    char *p=NULL;
    char tmp_buf[128];
    cJSON *root=create_msg(dev);
    //cJSON_AddStringToObject(root,"alarm","");
    //cJSON_AddNumberToObject(root,"timeStamp",time_stamp());
    cJSON_AddStringToObject(root,"data_type","FAN");
    cJSON *fan_info=cJSON_CreateObject();
    cJSON_AddStringToObject(fan_info,"type",get_fan_type(dev));
    cJSON_AddNumberToObject(fan_info,"onLine",get_fan_online(dev));
    sprintf(tmp_buf,"%d",get_fan_slot(dev));
    cJSON_AddStringToObject(fan_info,"slotId",tmp_buf);
    sprintf(tmp_buf,"%d",get_fan_rpm(dev));
    cJSON_AddStringToObject(fan_info,"rpm",tmp_buf);
    sprintf(tmp_buf,"%d",get_fan_pwm(dev));
    cJSON_AddStringToObject(fan_info,"pwm",tmp_buf);
    sprintf(tmp_buf,"%d",get_fan_maxrpm(dev));
    cJSON_AddStringToObject(fan_info,"maxRpm",tmp_buf);
    cJSON_AddItemToObject(root,"FAN",fan_info);
    p=cJSON_PrintUnformatted(root);

    send_msg(p,strlen(p)+1);
    cJSON_Free(p);
    cJSON_Delete(root);
    return 0;
}
void build_psu(cJSON *root, char *match_id)
{
    char tmp_buf[128];
    struct psu_device *p;
    list_for_each_entry(p,&g_config->psu_root,node_root)
    {
        if(strcmp(p->match_id , match_id)==0)
        {
            sprintf(tmp_buf,"%u",power_in(&p->dev));
            cJSON_AddStringToObject(root,"voltage",tmp_buf);
            sprintf(tmp_buf,"%u",power_curr(&p->dev));
            cJSON_AddStringToObject(root,"current",tmp_buf);
            sprintf(tmp_buf,"%lu",power_power(&p->dev));
            cJSON_AddStringToObject(root,"power",tmp_buf);
            break;
        }

    }
}

cJSON *BuildMsgArray(cJSON *root, const char *arrayname) {
    cJSON *_array=cJSON_CreateArray();
    cJSON_AddItemToObject(root, arrayname, _array);
    return _array;
}

int addChipMsg(cJSON *root, chipMsgSrcInfo_st *chipmsg) {
    char _tmp_buf[128];
    cJSON *_info=cJSON_CreateObject();
    cJSON_AddItemToArray(root, _info);
    cJSON_AddStringToObject(_info, "name", chipmsg->name);
    sprintf(_tmp_buf,"%u", read_sysfs_int(chipmsg->path_temperature, 0));
    cJSON_AddStringToObject(_info, "temperature", _tmp_buf);
    sprintf(_tmp_buf,"%u", read_sysfs_int(chipmsg->path_voltage, 0));
    cJSON_AddStringToObject(_info, "voltage", _tmp_buf);
    sprintf(_tmp_buf,"%u", read_sysfs_int(chipmsg->path_current, 0));
    cJSON_AddStringToObject(_info, "current", _tmp_buf);
    sprintf(_tmp_buf,"%u", read_sysfs_int(chipmsg->path_pwoer, 0));
    cJSON_AddStringToObject(_info, "power", _tmp_buf);
}

int build_matched_max_temp(char *match_id) {
    FUNC_START();
    int max=0;
    int tmp;
    struct temp_device *p;
    list_for_each_entry(p,&g_config->temp_root,node_root) {
        if(strcmp(p->match_id , match_id)==0) {
            tmp=temp_cur(&p->dev);
            max=tmp>max? tmp : max;
        }
    }
    FUNC_END();
    return max;
}
int build_boards_ver(cJSON *root)
{
    struct board_device *p;
    list_for_each_entry(p, &g_config->board_root,node_root)
    {
        if(p->online != 1)  continue ;
        cJSON *ver=cJSON_CreateObject();
        cJSON_AddItemToArray(root,ver);
        cJSON_AddStringToObject(ver,"hardwareVersion",p->hw);
        cJSON_AddStringToObject(ver,"softwareVersion",p->sw);
        cJSON_AddStringToObject(ver,"cpuId",p->cpuId);

    }
}
int public_board_msg(struct device *dev)
{
    char *p=NULL;
    char tmp_buf[128];
    printf("public_board_msg creat\n");
    cJSON *root=create_msg(dev);
    cJSON_AddStringToObject(root,"data_type","board_info");
    cJSON *info=cJSON_CreateObject();
    cJSON_AddItemToObject(root,"board_info",info);
    cJSON_AddStringToObject(info,"boardName", g_config->board_name?g_config->board_name:"");
    cJSON_AddStringToObject(info,"boardType",g_config->board_type);
    sprintf(tmp_buf,"%d",g_config->slot);
    cJSON_AddStringToObject(info,"slotId",tmp_buf);
    cJSON_AddNumberToObject(info,"onLine",1);
    cJSON_AddStringToObject(info,"serialNumber",g_config->sn?g_config->sn:"");
	  BuildChipMsg(info);
    sprintf(tmp_buf,"%d",build_matched_max_temp(MATCH_ID_TEMPERATURE_BOARD));
    cJSON_AddStringToObject(info,"temperature",tmp_buf);
    build_psu(info, MATCH_ID_PSU_BOADRD);
    cJSON *vers=cJSON_CreateArray();
    cJSON_AddItemToObject(info,"versions",vers);
    build_boards_ver(vers);
    p=cJSON_PrintUnformatted(root);
    cJSON_Print(root);
    printf("public_board_msg send start\n");
    send_msg(p,strlen(p)+1);
    printf("public_board_msg send end\n");
    cJSON_Free(p);
    cJSON_Delete(root);

    return 0;
}
int public_psu_msg(struct device *dev)
{
    char *p=NULL;
}
int public_alarm_msg(char *alarm)
{
    return 0;
}
cJSON *set_fan(cJSON *params,void *data)
{
    struct fan_device *info;
    cJSON *value;
    struct def *config=(struct def *)data;
    value = cJSON_GetObjectItem(params,"pwm");
    if(value)
    {
        list_for_each_entry(info,&config->fan_root,note_root)
        {
            set_fan_pwm(&info->dev,value->valueint);
        }
    }
    return NULL;
}
cJSON *board_info(cJSON *params, void *data)
{
    printf("public_board_msg start\n");
    public_board_msg(NULL);
    printf("public_board_msg start\n");
    return NULL;
}
int init_server(struct def *config)
{
    //reg_server(config,set_fan,"fan",config);
    reg_server(config,board_info,"board_info",config);
    return 0;
}
