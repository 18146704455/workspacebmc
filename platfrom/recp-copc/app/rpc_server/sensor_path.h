#ifndef __SENSOR_PATH_H
#define __SENSOR_PATH_H 
#include "sensor.h"
#define SENSOR_BASE_DIR "/sys/class/hwmon/"
static char *sensor_adm1278_basedir[SENSOR_ADM1278_NUM] = 
{
    SENSOR_BASE_DIR"hwmon0",
};
static char *sensor_adm1278_param_name[] = {
    "in1_input",
    "curr1_input",
    "power1_input",
    "temp1_input",
};
#if 0
static char *sensor_pwr1014_basedir = SENSOR_BASE_DIR"/hwmon5";
static char *sensor_pwr1014_param_name[] = {
    "in0_input",
    "in1_input",
    "in2_input",
    "in3_input",
    "in4_input",
    "in5_input",
    "in6_input",
    "in7_input",
    "in8_input",
    "in9_input",
    "in10_input",
    "in11_input",
};

static char *sensor_tmp75_basedir[SENSOR_TMP75_NUM]   = {
  SENSOR_BASE_DIR"hwmon1",
  SENSOR_BASE_DIR"hwmon2",
  SENSOR_BASE_DIR"hwmon3",
  SENSOR_BASE_DIR"hwmon4",
};
static char *sensor_tmp75_param_name[] = {
    "temp1_input"
};
#endif
#if 0
static char *sensor_fan_basedir = "/sys/bus/i2c/drivers/fancpld/73-0033";
static char *sensor_fan_param_name[] = {
	"fantray1_pwm",
	"fantray2_pwm",
	"fantray3_pwm"
};
#else
//max31790
#if 0
static char *sensor_fan_basedir = SENSOR_BASE_DIR"hwmon6";
static char *sensor_fan_param_name[]={
    "pwm1",
    "pwm2",
};
#endif
#endif

static char *sensor_max1617_basedir[SENSOR_MAX1617_NUM] = {
    SENSOR_BASE_DIR"hwmon1",
    SENSOR_BASE_DIR"hwmon2",
    SENSOR_BASE_DIR"hwmon3",
    SENSOR_BASE_DIR"hwmon4",
    SENSOR_BASE_DIR"hwmon5",
    SENSOR_BASE_DIR"hwmon6",
    SENSOR_BASE_DIR"hwmon7",
    SENSOR_BASE_DIR"hwmon8",
};
static char *sensor_max1617_param_name[]={
    "temp1_input",
    "temp2_input",
};

static char *sensor_pwr1220_basedir[SENSOR_PWR1220_NUM] = {
    SENSOR_BASE_DIR"hwmon9",
};
static char *sensor_pwr1220_param_name[]={
    "in0_input",
    "in1_input",
    "in2_input",
    "in3_input",
    "in4_input",
    "in5_input",
    "in6_input",
    "in7_input",
    "in8_input",
    "in9_input",
    "in11_input",
    "in12_input",
};
#if 0
//2s 
#define SENSOR_TPS53667_NUM 1
static char *sensor_tps53667_basedir[SENSOR_TPS53667_NUM]   = {
  SENSOR_BASE_DIR"hwmon8",
};
static char * sensor_tps53667_param_name[] = {
    "in1_input",
    "in2_input",
    "curr1_input",
    "power1_input",
    "temp1_input",
};
#else

static char *sensor_tps53667_basedir[SENSOR_TPS53667_NUM]={
    SENSOR_BASE_DIR"hwmon10",
    SENSOR_BASE_DIR"hwmon11",
    SENSOR_BASE_DIR"hwmon12",
    SENSOR_BASE_DIR"hwmon13",
    SENSOR_BASE_DIR"hwmon14",
    SENSOR_BASE_DIR"hwmon15",
};
static char *sensor_tps53667_param_name[]={
    "in1_input",
    "in2_input",
    "curr1_input",
    "power1_input",
    "temp1_input",
};
#endif


#endif /* ifndef __SENSOR_PATH_H */

