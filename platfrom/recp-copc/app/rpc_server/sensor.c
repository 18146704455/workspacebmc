#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "common_log.h"
#include <stdint.h>
#include "board_sensor_rpc_server.h"

#include "stdbool.h"
#include "cpld.h"
#include "sensor_path.h"
 

#ifdef DB_CURR_LEVEL 
#undef DB_CURR_LEVEL
#define DB_CURR_LEVEL DB_DEBUG
#endif


int default_sensor_ops_get(sensor_per_param_str *sensor_param, uint32_t *val);
#if 0
#define FAN_RESET_REG	0x20
#define FAN_PRSNT_REG   0x3
#define FAN_RESET_BIT	0x5
#define FAN_PRSNT_BIT   (1<<2)



void reset_fanboard(void)
{
	unsigned char reg = 0;
	int ret;
	ret = cpld_read_byte(FAN_RESET_REG, &reg);
	if(ret != 0) {
		DB_PRINT(DB_ERR, "ERROR:read cpld failed");
		return;
	}
	reg = (reg & ~(1 << FAN_RESET_BIT));
	ret = cpld_write_byte(FAN_RESET_REG, reg);
	if(ret != 0) {
		DB_PRINT(DB_ERR, "ERROR:write cpld failed");
		return;
	}
	
	reg |= (1 << FAN_RESET_BIT);
	ret = cpld_write_byte(FAN_RESET_REG, reg);
	if(ret != 0) {
		DB_PRINT(DB_ERR, "ERROR:write cpld failed");
		return;
	}
	
	
}


static int __sensor_fan_set(sensor_per_param_str *sensor_param, uint32_t real_speed)
{
	char cmd[32] = {'\0'};
	char val[32] = {'\0'};
	long real_val;
   	int iret = 0;
	bool need_reset = false;
	char *stop = NULL;



	iret = lseek(sensor_param->fd, 0, SEEK_SET);
	if(iret < 0) {
		DB_PRINT(DB_ERR, "ERROR:failed lseek device  file %s:%s", sensor_param->file_name,strerror(errno));
		return -1;
	}

    iret = write(sensor_param->fd, cmd, strlen(cmd));
	if( iret == -1 ) {
   		DB_PRINT(DB_ERR, "ERROR:failed write fan device file %s %s:%s", sensor_param->dev->sensor_name,cmd,strerror(errno));
    	iret = -1;
	}

	//check val，if not equal,rest fan todo
	if( iret != -1 ) {
		 iret = lseek(sensor_param->fd, 0, SEEK_SET);
		if(iret < 0) {
			DB_PRINT(DB_ERR, "ERROR:failed lseek device  file %s:%s", sensor_param->file_name,strerror(errno));
			return -1;
		}
		
		iret = read(sensor_param->fd, val, 4);
		if( iret == -1 ) {
			DB_PRINT(DB_ERR, "ERROR:failed get device %s param %s:%s", sensor_param->dev->sensor_name,sensor_param->file_name,strerror(errno));
		}
		

		if(iret > 0) {
			val[iret] = '\0';
			real_val = strtol(val, &stop, 16);
	        if (*stop != '\0') {
	            DB_PRINT(DB_ERR, "ERROR:failed conver %s to hex",val);
	        }
			if(real_speed != real_val) {
				need_reset = true;
			}
		}		
	}
	
	if(iret > 0 && need_reset) {
		reset_fanboard();
		iret = write(sensor_param->fd, cmd, strlen(cmd));
		if( iret == -1 ) {
	   		DB_PRINT(DB_ERR, "ERROR:failed write fan device file %s %s:%s", sensor_param->dev->sensor_name,cmd,strerror(errno));
	    	iret = -1;
		}
	}
	
    return iret;
}
int sensor_fan_max31790_set(sensor_per_param_str *sensor_param, uint32_t speed)
{
    char cmd[256] = {0};
    int iret = 0;
    if(speed < 50 || speed > 100)  //keep mininum 50% duty cycle
        return -1;
                                    //0-255 mininum is 150 ; you can change in driver
    speed = (speed*10) >> 2;

	if(sensor_param->mode != O_RDWR) {
		DB_PRINT(DB_ERR, "ERROR:device %s %s  don not have write permisson", sensor_param->dev->sensor_name,sensor_param->file_name);
        iret = -1;
	}

	iret = lseek(sensor_param->fd, 0, SEEK_SET);
	if(iret < 0) {
		DB_PRINT(DB_ERR, "ERROR:failed lseek device  file %s:%s", sensor_param->file_name,strerror(errno));
		return -1;
	}

	
	sprintf(cmd,"%d", speed);
    DB_PRINT(DB_DEBUG, "set %s", cmd);
    iret = write(sensor_param->fd, cmd, strlen(cmd));
    if(iret == -1){
        DB_PRINT(DB_ERR, "ERROR:failed set  device  %s param %s %s", sensor_param->dev->sensor_name,sensor_param->file_name,cmd);
        iret = -1;
    }


    return iret;

}
int sensor_fan_max31790_get(sensor_per_param_str *sensor_param, uint32_t *val)
{
    int iret = 0;
    unsigned char tmp=0;
    iret = cpld_read_byte(FAN_PRSNT_REG,&tmp);
    if( iret <0 || (tmp & FAN_PRSNT_BIT) != 0)
    {
        *val = 0;
        return 0;
    }

    iret = default_sensor_ops_get(sensor_param,val);
    if(iret == 0)
    {
        *val=(*val << 2)/10 ;
    }
    return iret;

}

int sensor_fan_set(sensor_per_param_str *sensor_param, uint32_t speed)
{
    if(speed < 50 || speed > 100)  //keep mininum 50% duty cycle
        return -1;

    if(100 == speed) speed = 95; //max duty 0x19

    speed = 0x10 + (speed - 50) / 5;
    return __sensor_fan_set(sensor_param, speed);
}
#endif
int sensor_max1617_get(sensor_per_param_str *sensor_param, uint32_t *val)
{
    int iret = 0;
    iret = default_sensor_ops_get(sensor_param,val);
    *val=(*val)/1000;
    return iret;
}
int sensor_pwr1220_get(sensor_per_param_str *sensor_param, uint32_t *val)
{
    int iret = 0,readlen;
    char cval[24]={0};
    char *stop=NULL;
    iret = lseek(sensor_param->fd, 0, SEEK_SET);
    if( iret < 0){
		DB_PRINT(DB_ERR, "ERROR:failed lseek device  file %s:%s", sensor_param->file_name,strerror(errno));
		return -1;
    }
    readlen = read(sensor_param->fd, cval, sizeof(cval));
    if(readlen < 0)
    {
         DB_PRINT(DB_ERR, "ERROR:failed get device  %s param %s:%s", sensor_param->dev->sensor_name,sensor_param->file_name,strerror(errno));
         return -1;

    }
    *val= (uint32_t)strtoul(cval, &stop,0);
    return readlen;

}
int default_sensor_ops_set(sensor_per_param_str *sensor_param, uint32_t val)
{
    int iret = 0;
    char cmd[256] = {0};

	if(sensor_param->mode != O_RDWR) {
		DB_PRINT(DB_ERR, "ERROR:device %s %s don not have write permisson", sensor_param->dev->sensor_name,sensor_param->file_name);
        iret = -1;
	}
	
	sprintf(cmd,"0x%x", val);
    DB_PRINT(DB_DEBUG, "set %s", cmd);
    iret = write(sensor_param->fd, cmd, strlen(cmd));
    if(iret == -1){
        DB_PRINT(DB_ERR, "ERROR:failed set  device  %s param %s %s", sensor_param->dev->sensor_name,sensor_param->file_name,cmd);
        iret = -1;
    }

    return iret;
}

int default_sensor_ops_get(sensor_per_param_str *sensor_param, uint32_t *val)
{
    int iret = 0,readlen;
	char cval[25] ={0};
    *val=0;
	iret = lseek(sensor_param->fd, 0, SEEK_SET);
	if(iret < 0) {
		DB_PRINT(DB_ERR, "ERROR:failed lseek device  file %s:%s", sensor_param->file_name,strerror(errno));
		return -1;
	}
	
    readlen = read(sensor_param->fd, cval, 25);
    if( readlen == -1 ) {
         DB_PRINT(DB_ERR, "ERROR:failed get device  %s param %s:%s", sensor_param->dev->sensor_name,sensor_param->file_name,strerror(errno));
		 iret = -1;
    } else {
		cval[readlen] = '\0';
		*val = atoi(cval);
	}
	
    return iret;  
}

//todo 有没有只写文件？
static int init_sensro_param(sensor_dev_str *dev, char **param_file)
{
	int i = 0;
	char file_name[256];
	//todo fix need check num
	dev->param_info = (sensor_per_param_str *)malloc(sizeof(sensor_per_param_str) * dev->sensor_param_num);
	if( dev->param_info == NULL) {
		DB_PRINT(DB_ERR, "ERROR:%s",strerror(errno));
		return -1;
	}

	for( i = 0; i < dev->sensor_param_num;i++) {
		dev->param_info[i].dev = dev;
			
		strcpy(dev->param_info[i].file_name, param_file[i]);
		sprintf(file_name,"%s/%s",dev->base_dir,dev->param_info[i].file_name);
		dev->param_info[i].fd = open(file_name,O_RDWR);
	    if( -1 == dev->param_info[i].fd && errno == 13) {
	        dev->param_info[i].fd = open(file_name,O_RDONLY);
			if( -1 == dev->param_info[i].fd) {
				DB_PRINT(DB_ERR, "ERROR:failed open fan device file(O_RDONLY)  %s for %s:(%d)%s", file_name,dev->sensor_name,errno,strerror(errno));
				goto FAILED;
			} else {
				dev->param_info[i].mode = O_RDONLY;
			}
	    } else if(-1 == dev->param_info[i].fd) {
			DB_PRINT(DB_ERR, "ERROR:failed open fan device file(O_RDWR)  %s for %s:(%d)%s", file_name,dev->sensor_name,errno,strerror(errno));
			goto FAILED;
		}else {
	        DB_PRINT(DB_DEBUG, "INFO:open device %s READ WRITE", file_name);
			dev->param_info[i].mode = O_RDWR;
	    }
	}
	
	return 0;
	
FAILED:
	free(dev->param_info);

	return -1;
}


int init_sensor_dev_str(sensor_dev_str *dev,char *base_dir, char *name, uint8_t parma_num, sensor_set_ops set_ops, sensor_get_ops get_ops, char **param_file)
{
    if(dev == NULL || base_dir == NULL || name == NULL) {
        DB_PRINT(DB_ERR, "param must not be NULL");
        return -1;
    }

    strcpy(dev->base_dir,base_dir);
    strcpy(dev->sensor_name,name);
    dev->sensor_param_num = parma_num;

    if(set_ops != NULL) {
        dev->sensor_ops.set = set_ops; 
    } else {
        dev->sensor_ops.set = default_sensor_ops_set;
    }


    if(get_ops != NULL) {
        dev->sensor_ops.get = get_ops;
    } else {
        dev->sensor_ops.get = default_sensor_ops_get;
    }

	return init_sensro_param(dev,param_file);
}



inline void destroy_sensor_dev(sensor_dev_str *dev)
{
	if(dev->param_info != NULL) {
		free(dev->param_info);
	}
}



#define ARRAY_SIZE(a) (sizeof((a)) / sizeof(a[0]))
int init_sensor(void)
{
    int i = 0;
    int iret = 0;

    DB_PRINT(DB_ERR, "init sensor %s", "adm1278");
    for(i=0; i< SENSOR_ADM1278_NUM; i++)
    {
        iret = init_sensor_dev_str(&sensor_adm1278[i], sensor_adm1278_basedir[i], "adm1278", ARRAY_SIZE(sensor_adm1278_param_name),NULL,NULL,sensor_adm1278_param_name);
        if(iret != 0) {
            DB_PRINT(DB_ERR, "ERROR:failed init sensor %s", "adm1278");
        } else {
            DB_PRINT(DB_INFO, "init sensor %s success", "adm1278");
        }
    }
#if 0
    DB_PRINT(DB_ERR, "init sensor %s", "powr1014");
    iret = init_sensor_dev_str(&sensor_pwr1014, sensor_pwr1014_basedir, "powr1014", ARRAY_SIZE(sensor_pwr1014_param_name),NULL,NULL,sensor_pwr1014_param_name);
    if(iret != 0) {
        DB_PRINT(DB_ERR, "ERROR:failed init sensor %s", "powr1014");
    } else {
        DB_PRINT(DB_INFO, "init sensor %s success", "powr1014");
    } 

    DB_PRINT(DB_ERR, "init sensor %s", "tmp75");
    for(i = 0; i < SENSOR_TMP75_NUM; i++) {
        iret = init_sensor_dev_str(&sensor_tmp75[i], sensor_tmp75_basedir[i], "tmp75", ARRAY_SIZE(sensor_tmp75_param_name),NULL,NULL,sensor_tmp75_param_name);
        if(iret != 0) {
            DB_PRINT(DB_ERR, "ERROR:failed init sensor %s", "tmp75");
        } else {
            DB_PRINT(DB_INFO, "init sensor %s success", "tmp75");
        }
    }

#endif
    DB_PRINT(DB_ERR, "init sensor %s", "tps53667");
    for(i = 0; i < SENSOR_TPS53667_NUM; i++) {
        iret = init_sensor_dev_str(&sensor_tps53667[i], sensor_tps53667_basedir[i], "tps53667", ARRAY_SIZE(sensor_tps53667_param_name),NULL,NULL,sensor_tps53667_param_name);
        if(iret != 0) {
            DB_PRINT(DB_ERR, "ERROR:failed init sensor %s", "tps53667");
        } else {
            DB_PRINT(DB_INFO, "init sensor %s success", "tps53667");
        }
    }
#if 0
    DB_PRINT(DB_ERR, "init sensor %s", "fan");
    iret = init_sensor_dev_str(&sensor_fan, sensor_fan_basedir, "fan", ARRAY_SIZE(sensor_fan_param_name),sensor_fan_max31790_set,sensor_fan_max31790_get,sensor_fan_param_name);
    if(iret != 0) {
        DB_PRINT(DB_ERR, "ERROR:failed init sensor %s", "fan");
    }  else {
        DB_PRINT(DB_INFO, "init sensor %s success", "fan");
    }
#endif
    DB_PRINT(DB_ERR, "init sensor %s", "max1617");
    for(i = 0;i < SENSOR_MAX1617_NUM; i++){
        iret = init_sensor_dev_str(&sensor_max1617[i],sensor_max1617_basedir[i], "max1617", ARRAY_SIZE(sensor_max1617_param_name),NULL,sensor_max1617_get,sensor_max1617_param_name);
    }
    DB_PRINT(DB_ERR, "init sensor %s", "pwr1220");
    for(i = 0;i < SENSOR_PWR1220_NUM; i++){
        iret = init_sensor_dev_str(&sensor_pwr1220[i],sensor_pwr1220_basedir[i], "pwr1220", ARRAY_SIZE(sensor_pwr1220_param_name),NULL,sensor_pwr1220_get,sensor_pwr1220_param_name);
    }

    return 0;
}


