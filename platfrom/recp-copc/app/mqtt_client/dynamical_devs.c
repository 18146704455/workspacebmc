
#include <string.h>
#include "cpld.h"
#include "dynamical_devs.h"
#include "sys_file.h"
#include "common_log.h" 


uint8_t GetCpldBitField(uint32_t addr, uint8_t bitfield) {
  uint8_t val=0;
  cpld_read_byte(addr, &val);
  return (val & bitfield);
}


/**
 * [RegisterMsgChipInfo 注册Chip信息，会根据信息申请内存存放访问路径，配合UnregisterMsgChipInfo小心使用]
 * @param  match_id    [配合sensorinfo->name_prefix，确定需要注册到sensorinfo->node的位置]
 * @param  driver_path [驱动访问路径]
 * @param  sensorinfo  [传感器设备的一些信息]
 * @return             [description]
 */
int RegisterMsgChipInfo(char *match_id, char *driver_path, struct _RegisterSensorInfo_st *sensorinfo) {
  chipMsgSrcInfo_st *_chipinfo;
  for(_chipinfo = sensorinfo->node; _chipinfo && _chipinfo->match_id; _chipinfo++) {
    if ( 0 == strcmp(_chipinfo->match_id, match_id) \
      && 0 == strncmp(_chipinfo->name, sensorinfo->name_prefix, strlen(sensorinfo->name_prefix))) {
      if (SetChipMsgPath(_chipinfo, sensorinfo->type, driver_path, sensorinfo->offset)) {
        return -1;
      }
    }
  }
  return 0;
}

/**
 * [UnregisterMsgChipInfo 解注册Chip信息，会根据信息申请内存存放访问路径，配合UnregisterMsgChipInfo小心使用]
 * @param  match_id    [配合sensorinfo->name_prefix，确定需要注册到sensorinfo->node的位置]
 * @param  sensorinfo  [传感器设备的一些信息]
 * @return            [description]
 */
int UnregisterMsgChipInfo(char *match_id, struct _RegisterSensorInfo_st *sensorinfo) {
  chipMsgSrcInfo_st *_chipinfo;
  for(_chipinfo = sensorinfo->node; _chipinfo && _chipinfo->match_id; _chipinfo++) {
    if ( 0 == strcmp(_chipinfo->match_id, match_id) \
      && 0 == strncmp(_chipinfo->name, sensorinfo->name_prefix, strlen(sensorinfo->name_prefix))) {
      FreeChipMsgPath(_chipinfo, sensorinfo->type);
    }
  }
  return 0;
}


void RegisterMultiChannelI2cDevToBus(char *compatible, uint8_t bus, uint8_t address, I2cChannelSubDev_st *devs) {
  I2cChannelSubDev_st *_dev;
  //注册pca9548
  register_i2c_device(compatible, bus, address);
  //挂载通道设备
  for(_dev = devs; _dev && _dev->compatible; _dev++) {
    if (!get_sub_channel_i2c_bus(bus, address, _dev->channel, &_dev->bus)) {
      //注册i2c子设备
      register_i2c_device(_dev->compatible, _dev->bus, _dev->address);
      if (!get_i2c_dev_hwmon_path(_dev->bus, _dev->address, _dev->driver_path)) {
        //注册失败,没获取到驱动路径
        ;
      }
    } else {
      //没找到通道
      ;
    }
  }
}

void UnregisterMultiChannelI2cDevToBus(uint8_t bus, uint8_t address, I2cChannelSubDev_st *devs) {
  I2cChannelSubDev_st *_dev;
  //卸载通道设备
  for(_dev = devs; _dev && _dev->compatible; _dev++) {
    unregister_i2c_device(_dev->bus, _dev->address);
    memset(_dev->driver_path, 0, SIZE_OF_DRIVER_PATH_LEN);
  }
  //卸载pca9548
  unregister_i2c_device(bus, address);
}

void RegisterMultiChannelI2cDevSensors(char *macth_id, I2cChannelSubDev_st *devs) {
  I2cChannelSubDev_st *_dev;
  int _idx;
  for(_dev = devs; _dev && _dev->compatible; _dev++) {
    if (0 != strlen(_dev->driver_path)) {
      //注册传感器信息
      for (_idx = 0; _idx <REG_SENSOR_INFO_MAX; _idx++) {
        if (_dev->sensor_info[_idx].name_prefix && _dev->sensor_info[_idx].node && _dev->sensor_info[_idx].reg_sensor_info) {
          _dev->sensor_info[_idx].reg_sensor_info(macth_id, _dev->driver_path, &_dev->sensor_info[_idx]);
        }
      }
    }
  }
}

void UnregisterMultiChannelI2cDevSensors(char *macth_id, I2cChannelSubDev_st *devs) {
  I2cChannelSubDev_st *_dev;
  int _idx;
  for(_dev = devs; _dev && _dev->compatible; _dev++) {
    //解注册传感器信息
    if (0 != strlen(_dev->driver_path)) {
      for (_idx = 0; _idx <REG_SENSOR_INFO_MAX; _idx++) {
        if (_dev->sensor_info[_idx].name_prefix && _dev->sensor_info[_idx].node && _dev->sensor_info[_idx].unreg_sensor_info) {
          _dev->sensor_info[_idx].unreg_sensor_info(macth_id, &_dev->sensor_info[_idx]);
        }
      }
    }
  }
}

//挂载pca9548设备
void RegisterMultiChannelI2cDev(char *compatible, char *macth_id, uint8_t bus, uint8_t address, I2cChannelSubDev_st *devs) {
  UnregisterMultiChannelI2cDevToBus(bus, address, devs);
  RegisterMultiChannelI2cDevToBus(compatible, bus, address, devs);
  RegisterMultiChannelI2cDevSensors(macth_id, devs);
}

void UnregisterMultiChannelI2cDev(char *macth_id, uint8_t bus, uint8_t address, I2cChannelSubDev_st *devs) {
  UnregisterMultiChannelI2cDevSensors(macth_id, devs);
}

