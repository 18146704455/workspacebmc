#ifndef __SENSOR_H
#define __SENSOR_H 

#define SENSOR_ADM1278_NUM 1
#define SENSOR_TMP75_NUM  4
#define SENSOR_FAN_NUM 1
#define SENSOR_MAX1617_NUM 8
#define SENSOR_PWR1220_NUM 1
#define SENSOR_TPS53667_NUM 6
int init_sensor(void);
sensor_dev_str	sensor_adm1278[SENSOR_ADM1278_NUM];
sensor_dev_str 	sensor_pwr1014;
sensor_dev_str	sensor_tmp75[SENSOR_TMP75_NUM];
sensor_dev_str	sensor_fan;
sensor_dev_str  sensor_max1617[SENSOR_MAX1617_NUM];
sensor_dev_str  sensor_tps53667[SENSOR_TPS53667_NUM];
sensor_dev_str  sensor_pwr1220[SENSOR_PWR1220_NUM];

#endif /* ifndef __SENSOR_H */
