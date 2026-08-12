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

#define MAX_NUM_LC03_PSUS     10
#define MAX_NUM_LC03_TMEPS    24
#define MAX_NUM_LC03_FANS     36

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
    .match_id="LC03",
    .slot=0,
    .hw="v02",
    .sw="00234",
    .sn="123dsun",
    .cpuId="BMC",
    .online=1,
  },{
    .match_id="NPSM",
    .slot=0,
    .hw="NPSM1",
    .sw="sw npsm",
    .cpuId="NPS",
  },{
    .match_id="DPIM",
    .slot=1,
    .hw="DPIM",
    .sw="sw dpi",
    .cpuId="DPI",
  },
  {0},
};

struct psu_device LC03_psus[MAX_NUM_LC03_PSUS] = {
  { .match_id=MATCH_ID_PSU_BOADRD, .name="adm1278", .path="/sys/class/hwmon/hwmon5", .pre_io=-1, .offset=1, },
  {0},
};
struct temp_device LC03_temps[MAX_NUM_LC03_TMEPS]={
  // {"max1617","tmp-3.2b","/sys/class/hwmon/hwmon8",0,.offset=1},
  {MATCH_ID_TEMPERATURE_BOARD,"tmp-3.49","/sys/class/hwmon/hwmon0",0,.offset=1},
  {MATCH_ID_TEMPERATURE_BOARD,"tmp-3,4c","/sys/class/hwmon/hwmon1",0,.offset=1},
  {MATCH_ID_TEMPERATURE_BOARD,"tmp-3.4d","/sys/class/hwmon/hwmon2",0,.offset=1},
  {MATCH_ID_TEMPERATURE_BOARD,"tmp-3.4e","/sys/class/hwmon/hwmon3",0,.offset=1},
  {MATCH_ID_TEMPERATURE_BOARD,"tmp-3.4f","/sys/class/hwmon/hwmon4",0,.offset=1},
  {0},
};
struct fan_device LC03_fans[MAX_NUM_LC03_FANS]={
  {0},
};

chipMsgSrcInfo_st chipInfo[] = {
  {
    .match_id = "barefoot", 
    .name = "barefoot",
    .path_temperature = "/sys/class/hwmon/hwmon8/temp2_input", 
    .path_voltage = "/sys/bus/i2c/devices/i2c-11/11-0047/hwmon/hwmon6/in1_input", 
    .path_current = "/sys/bus/i2c/devices/i2c-11/11-0047/hwmon/hwmon6/curr1_input", 
    .path_pwoer = "/sys/bus/i2c/devices/i2c-11/11-0047/hwmon/hwmon6/power1_input",
  },
  {
    .match_id = MACTH_ID_NPSM0,
    .name = "NPS0", 
  },
  {
    .match_id = MACTH_ID_NPSM1,
    .name = "NPS1", 
  },
  {
    .match_id = MACTH_ID_NPSM0,
    .name = "TCAM0", 
  },
  {
    .match_id = MACTH_ID_NPSM1,
    .name = "TCAM1", 
  },
  {0},
}; 

I2cChannelSubDev_st pca9548Devs[][7] = {
  {
    {
      .compatible = "tps53667",
      .channel = 0,
      .address = 0x60,
      .sensor_info[0].name_prefix = NAME_PREFIX_NPS,
      .sensor_info[0].node = chipInfo,
      .sensor_info[0].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[0].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[0].type = SENSOR_CHIP_VOL,
      .sensor_info[0].offset = 1,
      .sensor_info[1].name_prefix = NAME_PREFIX_NPS,
      .sensor_info[1].node = chipInfo,
      .sensor_info[1].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[1].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[1].type = SENSOR_CHIP_CUR,
      .sensor_info[1].offset = 1,
      .sensor_info[2].name_prefix = NAME_PREFIX_NPS,
      .sensor_info[2].node = chipInfo,
      .sensor_info[2].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[2].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[2].type = SENSOR_CHIP_PWR,
      .sensor_info[2].offset = 1,
    },
    {
      .compatible = "tps53667",
      .channel = 1,
      .address = 0x60,
      .sensor_info[0].name_prefix = NAME_PREFIX_TCAM,
      .sensor_info[0].node = chipInfo,
      .sensor_info[0].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[0].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[0].type = SENSOR_CHIP_VOL,
      .sensor_info[0].offset = 1,
      .sensor_info[1].name_prefix = NAME_PREFIX_TCAM,
      .sensor_info[1].node = chipInfo,
      .sensor_info[1].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[1].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[1].type = SENSOR_CHIP_CUR,
      .sensor_info[1].offset = 1,
      .sensor_info[2].name_prefix = NAME_PREFIX_TCAM,
      .sensor_info[2].node = chipInfo,
      .sensor_info[2].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[2].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[2].type = SENSOR_CHIP_PWR,
      .sensor_info[2].offset = 1,
    },
    {
      .compatible = "24c64",
      .channel = 2,
      .address = 0x51,
    },
    {
      .compatible = "max1617",
      .channel = 5,
      .address = 0x18,
      .sensor_info[0].name_prefix = NAME_PREFIX_NPS,
      .sensor_info[0].node = chipInfo,
      .sensor_info[0].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[0].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[0].type = SENSOR_CHIP_TMP,
      .sensor_info[0].offset = 2,
    },
    {
      .compatible = "max1617",
      .channel = 6,
      .address = 0x29,
      .sensor_info[0].name_prefix = NAME_PREFIX_TCAM,
      .sensor_info[0].node = chipInfo,
      .sensor_info[0].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[0].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[0].type = SENSOR_CHIP_TMP,
      .sensor_info[0].offset = 2,
    },
    {0},
  },
  {
    {
      .compatible = "tps53667",
      .channel = 0,
      .address = 0x60,
      .sensor_info[0].name_prefix = NAME_PREFIX_NPS,
      .sensor_info[0].node = chipInfo,
      .sensor_info[0].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[0].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[0].type = SENSOR_CHIP_VOL,
      .sensor_info[0].offset = 1,
      .sensor_info[1].name_prefix = NAME_PREFIX_NPS,
      .sensor_info[1].node = chipInfo,
      .sensor_info[1].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[1].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[1].type = SENSOR_CHIP_CUR,
      .sensor_info[1].offset = 1,
      .sensor_info[2].name_prefix = NAME_PREFIX_NPS,
      .sensor_info[2].node = chipInfo,
      .sensor_info[2].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[2].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[2].type = SENSOR_CHIP_PWR,
      .sensor_info[2].offset = 1,
    },
    {
      .compatible = "tps53667",
      .channel = 1,
      .address = 0x60,
      .sensor_info[0].name_prefix = NAME_PREFIX_TCAM,
      .sensor_info[0].node = chipInfo,
      .sensor_info[0].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[0].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[0].type = SENSOR_CHIP_VOL,
      .sensor_info[0].offset = 1,
      .sensor_info[1].name_prefix = NAME_PREFIX_TCAM,
      .sensor_info[1].node = chipInfo,
      .sensor_info[1].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[1].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[1].type = SENSOR_CHIP_CUR,
      .sensor_info[1].offset = 1,
      .sensor_info[2].name_prefix = NAME_PREFIX_TCAM,
      .sensor_info[2].node = chipInfo,
      .sensor_info[2].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[2].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[2].type = SENSOR_CHIP_PWR,
      .sensor_info[2].offset = 1,
    },
    {
      .compatible = "24c64",
      .channel = 2,
      .address = 0x51,
    },
    {
      .compatible = "max1617",
      .channel = 5,
      .address = 0x18,
      .sensor_info[0].name_prefix = NAME_PREFIX_NPS,
      .sensor_info[0].node = chipInfo,
      .sensor_info[0].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[0].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[0].type = SENSOR_CHIP_TMP,
      .sensor_info[0].offset = 2,
    },
    {
      .compatible = "max1617",
      .channel = 6,
      .address = 0x29,
      .sensor_info[0].name_prefix = NAME_PREFIX_TCAM,
      .sensor_info[0].node = chipInfo,
      .sensor_info[0].reg_sensor_info = RegisterMsgChipInfo,
      .sensor_info[0].unreg_sensor_info = UnregisterMsgChipInfo,
      .sensor_info[0].type = SENSOR_CHIP_TMP,
      .sensor_info[0].offset = 2,
    },
    {0},
  },
};

DynamicalI2cChannelDev_st dyamicalI2cDevs[] = {
  {
    .match_id = MACTH_ID_NPSM0,
    .compatible = "pca9548",
    .bus = SUB_CPU0_I2C_BUS,
    .address = 0x70,
    .b_need_reg = true,
    .check_ol_func = IsNpsmOnine,
    .register_dev_func = RegisterNpsm,
    .unregister_dev_func = UnregisterNpsm,
    .sub_node = pca9548Devs[0],
  },
  {
    .match_id = MACTH_ID_NPSM1,
    .compatible = "pca9548",
    .bus = SUB_CPU1_I2C_BUS,
    .address = 0x70,
    .b_need_reg = true,
    .check_ol_func = IsNpsmOnine,
    .register_dev_func = RegisterNpsm,
    .unregister_dev_func = UnregisterNpsm,
    .sub_node = pca9548Devs[1],
  },
  {0},
};

int get_slot_id()
{
    uint8_t value;
    cpld_read_byte(0x21,&value);
    return value;
}

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
    psus=LC03_psus;
    temps=LC03_temps;
    fans=LC03_fans;
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
    struct board_device *dev;
    for(dev=boards;dev&&dev->match_id;dev++)
    {
        reg_board(config,dev);
    }
    send_broadcast(UDP_FRAME_MASTER_REQ,"",0);
    //动态注册NPSM
    reg_dev_check(NULL, ListenRemovableDev);
}

