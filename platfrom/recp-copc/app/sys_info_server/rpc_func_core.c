/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @LastEditors: smith.zj
 */
#include "rpc_func.h"
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "board_bsp_info.h"
#include "common_log.h"


/*****************************/
#define NET_BASE 100

int echo_func(int socketFd, char *usrData, int dataLen,  simple_rpc_respond_data_s *respondData, void *cookie)
{
  pr_debug("get echo_func\n");
}

#if 0
#define SLOT_ID_GPIO_H "/sys/class/gpio/gpio390/value"
#define SLOT_ID_GPIO_L "/sys/class/gpio/gpio389/value"
#else
#define SLOT_ID_GPIO_BIT0 "/sys/class/gpio/gpio320/value"
#define SLOT_ID_GPIO_BIT1 "/sys/class/gpio/gpio332/value"
#define SLOT_ID_GPIO_BIT2 "/sys/class/gpio/gpio333/value"
#define SLOT_ID_GPIO_BIT3 "/sys/class/gpio/gpio334/value"
#endif


#define SLOT_ID_LOG "/tmp/net.log"

int rpc_get_slot_info(int socketFd, char *usrData, int dataLen, simple_rpc_respond_data_s *respondData, void *cookie)
{
  pr_debug("get rpc_get_slot_info\n");
  respondData->result = 0;
  respondData->dataLen = 1;
  *(uint16_t *)respondData->data = get_board_slot(); 
}

/*****************************/
/*
ps:
project one >>>>>>>>>>>>>>
  1.sys_info_server 运行在bmc上，sys_info_client 运行在comh上 
  2.bmc的ip读取寄存器0x21获取slot
  3.bmc上server向交换机发送六种携带submodule_slot和slot组合的报文
  4.交换芯片在对应vlan域内广播六种报文，每个端口接收一种报文   -------解决不掉vlan域和报文内容绑定的关系(应该可以通过单个vlan网卡分别发送，变相绑定)
  5.comh解读报文并设置对应IP

project two >>>>>>>>>>>>>>
  1.sys_info_server 运行在bmc上，sys_info_client 运行在comh上 
  2.bmc的ip读取寄存器0x21获取slot
  3.client和server建立连接，server获取每个comh的初始ip（每个comh上六个vlan子网卡只有一个联通）
  4.server端操作comhip(这个就能找到对应的submodule)和所在槽位号构建新的comhIP,并广播
  5.每个comh解读报文并设置对应ip
*/

int rpc_get_submodule_info(int socketFd, char *usrData, int dataLen, simple_rpc_respond_data_s *respondData, void *cookie)
{
  int ret =0;
  uint16_t subslot_slot;
  struct sockaddr_in guest;
  socklen_t guest_len = sizeof(guest);
  respondData->result = 0;
  respondData->dataLen = 1;
  /*******************************/
  ret = getpeername(socketFd,(struct sockaddr *)&guest,&guest_len);
  if(ret != 0) {
    pr_err("server get submodule slot info failed");
    return -1;
  }
  pr_debug("client_ipaddr=0x%x\n",guest.sin_addr.s_addr);
  uint16_t subm = (((guest.sin_addr.s_addr)<<8)>>24);
  subm = (subm-NET_BASE);
  *(uint16_t *)respondData->data = subm;
  pr_debug("rpc_get_submodule_info subslot_slot=%d\n",subm);
  return subm;
}

int rpc_get_boardid(int socketFd, char *usrData, int dataLen, simple_rpc_respond_data_s *respondData, void *cookie)
{
  int ret =0;
  
  pr_debug("rpc_get_boardid \n");
  respondData->result = 0;
  *(uint16_t *)respondData->data = get_boardtype_id(respondData->data);

  respondData->dataLen = 2;
  
  return 0;
}

int rpc_get_boardtype(int socketFd, char *usrData, int dataLen, simple_rpc_respond_data_s *respondData, void *cookie)
{
  int ret =0;
  pr_debug("rpc_get_boardtype \n");
  
  respondData->result = get_boardtype_string(respondData->data);

  if (respondData->result == 0) {
    respondData->dataLen = strlen(respondData->data) + 1;
  } else {
    respondData->dataLen = 0;
  }
  
  return 0;
}


static int rpc_set_service_run_status(int socketFd, char *usrData, int dataLen, simple_rpc_respond_data_s *respondData, void *cookie)
{

  char buf[10]={0};
  strncpy(buf,usrData,2);
  uint8_t subslot=buf[0];
  uint16_t status=buf[1];
  pr_debug("subslot=%d,status=%d\n",subslot,status);

  respondData->result = ctrl_led_status(subslot, status);
  respondData->dataLen = 0;
  
  return 0;
}

int rpc_deal_come_status(int socketFd, char *usrData, int dataLen, simple_rpc_respond_data_s *respondData, void *cookie)
{ 
  pr_debug("come status:%d\n", usrData[0]);

  if(ERRCODE1_RESTART_CPU == usrData[0]) {
    system("nps.sh");
  }
  
  respondData->result = 0;
  respondData->dataLen = 1;
  *(uint16_t *)respondData->data = 0; 
}

static simple_rpc_func_s gSysServerInfoRpcFunc[] = {
    {"echo", echo_func},
    {"slot_info", rpc_get_slot_info},
    {"deal_come_status", rpc_deal_come_status},
    {"submodule_slot_info",rpc_get_submodule_info},
    {"set_service_run_status",rpc_set_service_run_status},
    {"get_board_type",rpc_get_boardtype},
    {"get_board_id",rpc_get_boardid},
};


int deal_rpc_request(int socketFd, char *buf, int len)
{
  int i = 0;
  simple_rpc_respond_data_s respondData;

  DB_PRINT(DB_DEBUG, "data len:%d\n", len);
  if(len < MAX_SIMPLE_RPC_FUN_NAME_LEN) {
    pr_err("data len invalid\n");
  }

  simple_rpc_request_data *requestData = (simple_rpc_request_data *)buf;
  requestData->funcName[MAX_SIMPLE_RPC_FUN_NAME_LEN-1] = '\0';

  for(i = 0; i < NELEMS(gSysServerInfoRpcFunc);i++) {
    if(strcmp(requestData->funcName, gSysServerInfoRpcFunc[i].name) == 0) {
      DB_PRINT(DB_DEBUG, "get func:%s\n", gSysServerInfoRpcFunc[i].name);
      strncpy(respondData.funcName, gSysServerInfoRpcFunc[i].name, MAX_SIMPLE_RPC_FUN_NAME_LEN);
      gSysServerInfoRpcFunc[i].callback(socketFd, requestData->data, requestData->dataLen, &respondData, NULL);
      send(socketFd, &respondData, sizeof(simple_rpc_respond_data_s), 0);  
    }
  }

  return -1;
}

