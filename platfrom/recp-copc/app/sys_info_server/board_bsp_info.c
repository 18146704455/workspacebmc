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
#include "gpio.h"
#include "eeprom_base.h"
#include "common_log.h"
#include "board_bsp_info.h"

#define MAX_BOARDTYPE_LEN 128
#define MAX_SN_LEN  128
struct board_bsp_info {
  char boardType[MAX_BOARDTYPE_LEN];    //string
  uint16_t baordTypeId;
  uint16_t slot;
  char sn[MAX_SN_LEN];        //string
  char pn[MAX_SN_LEN];        //string
};

static struct board_bsp_info gBspBaseInfo;

static int _pick_boardtype_string(char *boardType)
{
  const char *_boardType  = eeprom_parser_boardtype();
  if(!_boardType) {
    pr_err("init eeprom file failed\n");
    return -1;
  } else {
    pr_log("boardtype:%s\n",_boardType);
    strncpy(boardType, _boardType, MAX_BOARDTYPE_LEN);
  }
  return 0;
}

static int _pick_boardtype_id(uint16_t *id)
{
  return 0;
}

static uint16_t _pick_board_slot()
{
  #define DEFAULT_SLOT_IO_VALUE   1
  static int slot_gpio[] = {389, 390};    // low->high
  int slot = 0, idx;
  for (idx = 0; idx < sizeof(slot_gpio) / sizeof(int); idx++) {
      slot |= gpio_get_value(slot_gpio[idx], DEFAULT_SLOT_IO_VALUE) << idx;
  }
  gBspBaseInfo.slot = ~slot & 0x3;
  pr_log("local slot:%u\n", gBspBaseInfo.slot);
  return 0;
}

static int _pick_board_sn(char *sn)
{
  return 0;
}

static int _pick_board_pn(char *pn)
{
  return 0;
}

int init_bsp_info(char *eepromfile)
{
  /*
  *读取EEPROM信息
  *包含如下：
  *1 板类型
  *2 板ID
  *3 序列号
  */
  if(eepromfile == NULL) {
    eepromfile = DEF_EEPROM_FILE;
  }

  DB_PRINT(DB_LOG, "init_eeprom");
  if(init_eeprom(eepromfile) != 0) {
    pr_err("init eeprom file failed\n");
    pr_log("init eeprom file failed\n");
    return -1;
  } else {
    pr_log("init eeprom %s success\n", eepromfile);
  }

  _pick_boardtype_string(gBspBaseInfo.boardType);
  _pick_boardtype_id(&gBspBaseInfo.baordTypeId);
  _pick_board_sn(gBspBaseInfo.sn);
  _pick_board_pn(gBspBaseInfo.pn);

  /*
  *获取实时信息
  *槽位号
  */
  _pick_board_slot(gBspBaseInfo.slot);
return 0;
}


int get_boardtype_string(char *boardType)
{
  strncpy(boardType, gBspBaseInfo.boardType, MAX_BOARDTYPE_LEN);
  return 0;
}

uint16_t get_boardtype_id()
{
  return gBspBaseInfo.baordTypeId;
}

uint16_t get_board_slot()
{
  return gBspBaseInfo.slot;
}

int get_board_sn(char *sn)
{
  strncpy(sn, gBspBaseInfo.sn,MAX_SN_LEN);
  return 0;
}

int get_board_pn(char *pn)
{
  strncpy(pn, gBspBaseInfo.pn, MAX_SN_LEN);
  return 0;
}


int ctrl_led_status(uint8_t submoduleSlot, SERVICE_STATUS_E serviceStatus)
{
#define SUBMODULE_LED 0x60
#define POWER_LED_BIT 7

#define HW_TOPO0 0
#define HW_TOPO1 1
#define HW_TOPO2 2
#define HW_TOPO3 3
#define HW_TOPO4 4
#define HW_TOPO5 5

  uint8_t val;
  uint8_t power_val;
  uint8_t pld_reg;
  uint8_t submoduleHwSlot;
  
  if(submoduleSlot >= 6) {
    return -1;
  }

  uint8_t submoduleHwTopology[6] = {HW_TOPO0,HW_TOPO3,HW_TOPO1,HW_TOPO4,HW_TOPO2,HW_TOPO5}; 
  
  submoduleHwSlot = submoduleHwTopology[submoduleSlot];

}
