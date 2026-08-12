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


int echo_func(int socketFd, char *usrData, int dataLen,  simple_rpc_respond_data_s *respondData, void *cookie)
{
  printf("get echo_func\n");
}
/*
#define ROOT_BOARD "[root]"
#define COME_BOARD "[COME]"
#define FABT_BOARD "[FABT]"
#define FCB_BOARD "[FCB]"
#define FABH_BOARD "[FABH]"
*/
#if 0
#define SLOT_ID_GPIO_H "/sys/class/gpio/gpio390/value"
#define SLOT_ID_GPIO_L "/sys/class/gpio/gpio389/value"
#else
/*copc单板cpu的ip分配暂时不通过该程序*/
#define SLOT_ID_GPIO_BIT0 "/sys/class/gpio/gpio320/value"
#define SLOT_ID_GPIO_BIT1 "/sys/class/gpio/gpio332/value"
#define SLOT_ID_GPIO_BIT2 "/sys/class/gpio/gpio333/value"
#define SLOT_ID_GPIO_BIT3 "/sys/class/gpio/gpio334/value"
#endif
static uint8_t gslot = 5;
int get_slot_id()
{
  int ret=0;
  int fd;
  int rv;
  //uint16_t slot_ascii;
  char slot_ascii[2]={0};
  int slot_g=0;
  ret = system("cpld rd 0x21 | cut -d ' ' -f 2 > /tmp/net.log");
  if(ret !=0 ||ret ==127)
  {
    pr_log("Description Failed to obtain the slot by executing the system function\n");
    return -1;
  }
  if((fd = open(SLOT_ID_LOG, O_RDWR, 0666)) < 0)  {
    pr_log("Open file %s failure: %s\n", SLOT_ID_LOG, strerror(errno));
    return -1;
  }

  lseek(fd, 0, SEEK_SET);
  if ((rv = read(fd, &slot_ascii, sizeof(slot_ascii))) < 0) {
    pr_log("Read data from fd[%d] failure: %s\n", fd, strerror(errno));
    close(fd);
  }
  close(fd);
  sscanf(slot_ascii,"%x",&slot_g);
  gslot=slot_g;
  pr_log(" slot_g=%d\n",slot_g);
  return slot_g;
}


int rpc_get_slot_info(int socketFd, char *usrData, int dataLen, simple_rpc_respond_data_s *respondData, void *cookie)
{
  printf("get rpc_get_slot_info\n");
  get_slot_id();
  respondData->result = 0;
  respondData->dataLen = 1;
  *(uint16_t *)respondData->data = gslot; 
}


enum {
    ERRCODE0_CPU_OK=0,
    ERRCODE1_RESTART_CPU=1,
    ERRCODE1_CPU_OK=2,
    ERRCODE1_CP_OK=3,
    ERRCODE1_DP_OK=4
};

int rpc_deal_come_status(int socketFd, char *usrData, int dataLen, simple_rpc_respond_data_s *respondData, void *cookie)
{
  printf("get rpc_deal_come_status\n");


  printf("come status:%d\n", usrData[0]);

  if(ERRCODE1_RESTART_CPU == usrData[0]) {
    system("nps.sh");
  }
  
  respondData->result = 0;
  respondData->dataLen = 1;
  *(uint16_t *)respondData->data = 0; 
}

/*
int rpc_get_board_type(int socketFd, char *usrData, int dataLen, simple_rpc_respond_data_s *respondData, void *cookie)
{
  printf("rpc_get_board_type\n");
  FILE *fptr;
  char *tmp;

  fptr = fopen("/sys/bus/i2c/devices/i2c-2/i2c-product0/eeprom", "r");
  char myString[100];
  if(fptr != NULL) {
    while(fgets(myString, 100, fptr)) {
          //解析eeprom每行参数，塞入数据返回结构
      if(strcmp(myString,"[root]") == 0)
      {
          tmp = strtok(myString,"=");
          while (tmp != NULL) {
            if(strcmp(tmp,"board_name") == 0)
            {
              tmp = strtok(NULL, "="); // 注意传递NULL来获取下一个子串
              printf("%s\n", tmp);
            }else if(strcmp(tmp,"board_type") == 0) {
              tmp = strtok(NULL, "="); // 注意传递NULL来获取下一个子串
              printf("%s\n", tmp);
            }else{
              tmp = strtok(NULL, "="); // 注意传递NULL来获取下一个子串
            }
          }
      }else if(strcmp(myString,"[FABH]") == 0){
          tmp = strtok(myString,"=");
          while (tmp != NULL) {
            if(strcmp(tmp,"board_name") == 0)
            {
              tmp = strtok(NULL, "="); // 注意传递NULL来获取下一个子串
              printf("%s\n", tmp);
            }else{
              tmp = strtok(NULL, "="); // 注意传递NULL来获取下一个子串
            }
          }
      }else if(strcmp(myString,"[COME]") == 0){
          tmp = strtok(myString,"=");
          while (tmp != NULL) {
            if(strcmp(tmp,"mac") == 0)
            {
              tmp = strtok(NULL, "="); // 注意传递NULL来获取下一个子串
              printf("%s\n", tmp);
            }else{
              tmp = strtok(NULL, "="); // 注意传递NULL来获取下一个子串
            }
          }
      }else{
          tmp = strtok(myString,"=");
          while (tmp != NULL) {
              tmp = strtok(NULL, "="); // 注意传递NULL来获取下一个子串
              printf("%s\n", tmp);
          }
      }
    }
  } else {
        printf("open eeprom failed");
  }
  fclose(fptr);
}
*/

static simple_rpc_func_s gSysServerInfoRpcFunc[] = {
    {"echo", echo_func},
    {"slot_info", rpc_get_slot_info},
    {"deal_come_status", rpc_deal_come_status},
};

int deal_rpc_request(int socketFd, char *buf, int len)
{
  int i = 0;
  simple_rpc_respond_data_s respondData;

  printf("data len:%d\n", len);
  if(len < MAX_SIMPLE_RPC_FUN_NAME_LEN) {
    printf("data len invalid\n");
  }

  simple_rpc_request_data *requestData = (simple_rpc_request_data *)buf;
  requestData->funcName[MAX_SIMPLE_RPC_FUN_NAME_LEN-1] = '\0';
  
  for(i = 0; i < NELEMS(gSysServerInfoRpcFunc);i++) {
    if(strcmp(requestData->funcName, gSysServerInfoRpcFunc[i].name) == 0) {
      printf("get func:%s\n", gSysServerInfoRpcFunc[i].name);
      strncpy(respondData.funcName, gSysServerInfoRpcFunc[i].name, MAX_SIMPLE_RPC_FUN_NAME_LEN);
      gSysServerInfoRpcFunc[i].callback(socketFd, requestData->data, requestData->dataLen, &respondData, NULL);
      send(socketFd, &respondData, sizeof(simple_rpc_respond_data_s), 0);  
    }
  }

  return -1;
}

