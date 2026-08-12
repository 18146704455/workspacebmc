/*************************************************************************
  > File Name: LC02.c
  > Author: 
  > Mail: 
  > Created Time: Fri 17 Jan 2020 10:47:02 AM CST
 ************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "base.h"
#include "config.h" 
#include "psu.h"
#include "temp.h"
#include "fan.h"
#include "board.h"
#include "cpld.h"
#include "udp_call.h"
#include "version.h" 
#include "dynamical_devs.h"
#include "common_log.h" 
#include "time_check.h"
#include "msg.h"

#define MAX_NUM_COPC_PSUS     10
#define MAX_NUM_COPC_TMEPS    24
#define MAX_NUM_COPC_FANS     36

#define SUB_CPU_PRESENCE_REG    0x26
#define SUB_CPU_ONLINE_REG      0x25
#define SUB_CPU0_TYPE_BITFIELD  0x07
#define SUB_CPU1_TYPE_BITFIELD  0x70
#define SUB_CPU0_PRESENCE_BITFIELD    0x08
#define SUB_CPU1_PRESENCE_BITFIELD    0x80
#define SUB_CPU0_ONLINE_BITFIELD  0x80
#define SUB_CPU1_ONLINE_BITFIELD  0x40
#define SUB_CPU0_I2C_BUS        0
#define SUB_CPU1_I2C_BUS        4

#define SUB_CPU_TYPE_NPSM       6

#define NAME_PREFIX_NPS  "NPS"
#define NAME_PREFIX_TCAM "TCAM"
#define NAME_PREFIX_COME  "CPU"
#define MACTH_ID_NPSM0   "NPSM0"
#define MACTH_ID_NPSM1   "NPSM1"

extern struct fan_device *fans;
extern struct psu_device *psus;
extern struct temp_device *temps;
bool IsNpsmOnine(struct _DynamicalI2cChannelDev_st *dev);
int RegisterNpsm(struct _DynamicalI2cChannelDev_st *dev);
void UnregisterNpsm(struct _DynamicalI2cChannelDev_st *dev);

struct board_device boards[]={
  {
    .match_id="LC",
    .slot=0,
    .hw="v02",
    .sw="00234",
    .sn="123dsun",
    .cpuId="BMC",
    .online=1,
  },
  {0},
};

struct psu_device COPC_psus[MAX_NUM_COPC_PSUS] = {
  { .match_id=MATCH_ID_PSU_BOADRD, .name="adm1278", .path="/sys/bus/i2c/devices/0-0010/hwmon/hwmon1", .pre_io=-1, .offset=1, },
  {0},
};
struct temp_device COPC_temps[MAX_NUM_COPC_TMEPS]={
  // {MATCH_ID_TEMPERATURE_BOARD,"max1617-0,4c","/sys/bus/i2c/devices/0-004c/hwmon/hwmon2",0,.offset=1},
  {MATCH_ID_TEMPERATURE_BOARD,"tmp-0.4d","/sys/bus/i2c/devices/0-004d/hwmon/hwmon0",0,.offset=1},
  {0},
};
struct fan_device COPC_fans[MAX_NUM_COPC_FANS]={
  {0},
};

chipMsgSrcInfo_st chipInfo[] = {
  {
    .match_id = "barefoot", 
    .name = "barefoot",
    .path_temperature = "/sys/bus/i2c/devices/i2c-0/0-004c/hwmon/hwmon3/temp2_input", 
    .path_voltage = "/sys/bus/i2c/devices/i2c-0/0-0041/hwmon/hwmon2/in1_input", 
    .path_current = "/sys/bus/i2c/devices/i2c-0/0-0041/hwmon/hwmon2/curr1_input", 
    .path_pwoer = "/sys/bus/i2c/devices/i2c-0/0-0041/hwmon/hwmon2/power1_input",
  },
  {0},
}; 


#if 1
I2cChannelSubDev_st pca9548Devs[][7] = {
  {0},
};

DynamicalI2cChannelDev_st dyamicalI2cDevs[] = {
  {0},
};

#endif

#define GPIOF0 320 
#define GPIOG4 332
#define GPIOG6 334
#define GPIOG7 335
#define GPIOG5 333

#define DEFAULT_SLOT_IO_VALUE   1
static int slot_gpio[] = {GPIOF0, GPIOG4,GPIOG5,GPIOG6};    // low->high

#define COPC_EEPROM_FILE "/sys/bus/i2c/devices/i2c-0/0-0050/eeprom"

#if 1
int get_slot_id() {
    int slot = 0, idx;
    for (idx = 0; idx < sizeof(slot_gpio) / sizeof(int); idx++) {
        slot |= gpio_get_value(slot_gpio[idx], DEFAULT_SLOT_IO_VALUE) << idx;
        // printf("slot=%d\n",slot);
    }
    // slot = ~slot & 0x3;
    // printf("@slot=%d\n",slot);
    return slot;
}
#else
int get_slot_id()
{
    uint8_t buf[10]={};
    uint8_t value;
    buf[0]=gpio_get_value(GPIOG6,0);
    buf[1]=gpio_get_value(GPIOG5,0);
    buf[2]=gpio_get_value(GPIOG4,0);
    buf[3]=gpio_get_value(GPIOF0,0);
    value = ((0<<buf[0]) |(1<<buf[1])|(2<<buf[2])|(3<<buf[3]));
    printf("slot=%d%d%d%d\nvalue=%d\n",buf[0],buf[1],buf[2],buf[3],value);
    return value;
}
int get_slot_id()
{
    uint8_t value;
    cpld_read_byte(0x21,&value);
    return value;
}
#endif

bool IsNpsmOnine(struct _DynamicalI2cChannelDev_st *dev) {
  uint8_t _presence=1;
  uint8_t _ol = 0;
  uint8_t _type = 0;
  switch(dev->bus) {
    case SUB_CPU0_I2C_BUS:
      //toDo 类型检测
      _presence = GetCpldBitField(SUB_CPU_PRESENCE_REG, SUB_CPU0_PRESENCE_BITFIELD);
      _type = GetCpldBitField(SUB_CPU_PRESENCE_REG, SUB_CPU0_TYPE_BITFIELD);
      _ol = GetCpldBitField(SUB_CPU_ONLINE_REG, SUB_CPU0_ONLINE_BITFIELD);
      break;
    case SUB_CPU1_I2C_BUS:
      _presence = GetCpldBitField(SUB_CPU_PRESENCE_REG, SUB_CPU1_PRESENCE_BITFIELD);
      _type = GetCpldBitField(SUB_CPU_PRESENCE_REG, SUB_CPU1_TYPE_BITFIELD) >> 4;
      _ol = GetCpldBitField(SUB_CPU_ONLINE_REG, SUB_CPU1_ONLINE_BITFIELD);
      break;
    default:
      break;
  }
  // pr_log("bus %d: CPU type %X presence %d, online %d", dev->bus, _type, _presence, _ol);
  if (SUB_CPU_TYPE_NPSM == _type && !_presence && _ol) {
    return true;
  }
  return false;
}

int RegisterNpsm(struct _DynamicalI2cChannelDev_st *dev) {
  pr_log("register %s at %d address %d", dev->compatible, dev->bus, dev->address);
  RegisterMultiChannelI2cDev(dev->compatible, dev->match_id, dev->bus, dev->address, dev->sub_node);
  return 0;
}

void UnregisterNpsm(struct _DynamicalI2cChannelDev_st *dev) {
  pr_log("unregister bus %d address %d", dev->bus, dev->address);
  UnregisterMultiChannelI2cDev(dev->match_id, dev->bus, dev->address, dev->sub_node);
}

void RegisterDynamicalI2cDev() {
  DynamicalI2cChannelDev_st *_dev;
  for (_dev = dyamicalI2cDevs; _dev && _dev->check_ol_func; _dev++) {
    if (_dev->check_ol_func(_dev)) {
      if (_dev->b_need_reg && _dev->register_dev_func) {
        if (!_dev->register_dev_func(_dev)) {
          _dev->b_need_reg = false;
        }
      }
    } else {
      if (!_dev->b_need_reg) {
        if (_dev->unregister_dev_func) {
          _dev->unregister_dev_func(_dev);
        }
        //toDo 报告模块掉线错误
        _dev->b_need_reg = true;
      }
    }
  }
}

TIMER_FUNC(ListenRemovableDev) {
  RegisterDynamicalI2cDev();
  set_next_timer(loop, w, NULL, 5);
}


void BuildChipMsg(cJSON *root) {
  chipMsgSrcInfo_st *_msg;
  cJSON *_info = BuildMsgArray(root, "chips");
  for (_msg = chipInfo; _msg && _msg->name; _msg++) {
    if (_msg->path_temperature && _msg->path_voltage && _msg->path_current && _msg->path_pwoer) {
      addChipMsg(_info, _msg);
    }
  }
}


void init_board(struct def *config)
{
    psus=COPC_psus;
    temps=COPC_temps;
    fans=COPC_fans;

    set_eeprom_file(COPC_EEPROM_FILE);
    init_eeprom();
    config->sn=calloc(SN_SIZE,sizeof(char));
    boards[0].sw=calloc(SW_SIZE,sizeof(char));
    boards[0].hw=calloc(SW_SIZE,sizeof(char));
    deal_HW(boards[0].hw,SW_BMC,0);
    deal_SW(boards[0].sw,SW_BMC,0);
    deal_SN(config->sn,0);

    config->board_name=strdup(get_board_name());
    config->board_type="LC";
    config->slot=get_slot_id();
    config->def_psu="adm1278";
    reg_udp_server_call(g_config->udp_s,UDP_FRAME_MASTER_ACK,frame_master_ack);
    printf("board_name=%s,config->board_type=%s,slot=%d,config->def_psu=%s,config->def_sn=%s\n",config->board_name,config->board_type,config->slot,config->def_psu,config->sn);
    struct board_device *dev;
    for(dev=boards;dev&&dev->match_id;dev++)
    {
        reg_board(config,dev);
    }
    send_broadcast(UDP_FRAME_MASTER_REQ,"",0);
    //动态注册NPSM
    reg_dev_check(NULL, ListenRemovableDev);
}
