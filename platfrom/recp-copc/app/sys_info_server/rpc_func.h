/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @LastEditors: smith.zj
 */
#ifndef __RPC_FUNC_H__
#define __RPC_FUNC_H__
#include <stdint.h>

#define NELEMS(_array)	(uint32_t)(sizeof(_array) / sizeof(_array[0]))

#define MAX_SIMPLE_RPC_FUN_NAME_LEN 56
#define MAX_SIMPLE_RPC_DATA_LEN    512

enum {
    ERRCODE0_CPU_OK=0,
    ERRCODE1_RESTART_CPU=1,
    ERRCODE1_CPU_OK=2,
    ERRCODE1_CP_OK=3,
    ERRCODE1_DP_OK=4
};

typedef enum _SERVICE_STATUS_E {
    SERVICE_STATUS_NOTOK = 0,
    SERVICE_STATUS_SYSTEM_OK = 1, //快闪
    SERVICE_STATUS_CP_OK,	//慢闪
    SERVICE_STATUS_DP_OK,	//慢闪
    SERVICE_STATUS_RUN_OK, //常亮
} SERVICE_STATUS_E;


typedef struct  _simple_rpc_request_data {
  char funcName[MAX_SIMPLE_RPC_FUN_NAME_LEN];
  int dataLen;
  char data[MAX_SIMPLE_RPC_DATA_LEN];
} simple_rpc_request_data;

typedef struct  _simple_rpc_respond_data_s {
  char funcName[MAX_SIMPLE_RPC_FUN_NAME_LEN];
  int result;
  int dataLen;
  char data[MAX_SIMPLE_RPC_DATA_LEN];
} simple_rpc_respond_data_s;

typedef int (*rpc_func)(int socketFd, char *usrData, int dataLen, simple_rpc_respond_data_s *result, void *cookie);
typedef struct _simple_rpc_func_s {
  char *name;
  rpc_func callback;
} simple_rpc_func_s;

int deal_rpc_request(int socketFd, char *buf, int len);

#endif
