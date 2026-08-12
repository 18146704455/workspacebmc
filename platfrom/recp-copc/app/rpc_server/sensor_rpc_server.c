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
#include "board_sensor_rpc_server.h"
#include "iniparser.h" 
#include "version.h"
#include "cpld.h" 
#include "sensor.h"
#include "time_check.h"
#include "led_set.h"
 
 


#ifdef DB_CURR_LEVEL 
#undef DB_CURR_LEVEL
#define DB_CURR_LEVEL DB_DEBUG
#endif


zlog_category_t *glog;

#if 0
#define SENSOR_TMP75_NUM	4
#define SENSOR_TPS53667_NUM	1
#define SENSOR_FAN_NUM 1
extern int  init_sensor(void ); 
extern sensor_dev_str	sensor_adm1278;
extern sensor_dev_str 	sensor_pwr1014;
extern sensor_dev_str	sensor_tmp75[SENSOR_TMP75_NUM];
extern sensor_dev_str	sensor_tps53667[SENSOR_TPS53667_NUM];
extern sensor_dev_str	sensor_fan;
extern sensor_dev_str   sensor_max1617[SENSSOR_MAX1617_NUM];
extern sensor_dev_str   sensor_pwr1220[SENSSOR_PWR1220_NUM];
#endif
static int gServerPort = 0;
static struct jrpc_server my_server;

cJSON * say_hello(jrpc_context * ctx, cJSON * params, cJSON *id) 
{

    printf("params %d | id %d\n",params->type,id->type);
    


    return cJSON_CreateString("Hello!");
}

cJSON * exit_server(jrpc_context * ctx, cJSON * params, cJSON *id) 
{
    jrpc_server_stop(&my_server);
    return cJSON_CreateString("Bye!");
}

//整板12v输入电源，todo no err check
cJSON * rpc_server_get_sensordata(jrpc_context * ctx, cJSON * params, cJSON *id,sensor_dev_str* sensor,int sensor_num)
{
	int i = 0,j;
	int ret;
	//int val = 0;
    uint32_t val = 0;
	const char *param_name[25] = {0};
	int param_val[25] = {0};
	cJSON *root;
	cJSON *json_param_name;
	cJSON *json_param_val;
	char param_val_key[25];

    root = cJSON_CreateObject(); 
	cJSON_AddNumberToObject(root, RPC_SENSOR_NUM_KEY, sensor_num);
    cJSON_AddNumberToObject(root, RPC_SENSOR_PARAM_NUM_KEY, sensor[0].sensor_param_num);
	cJSON_AddStringToObject(root, RPC_SENSOR_NAME_KEY, sensor[0].sensor_name);
	

	for(j = 0; j < sensor_num;j++) {
		for(i = 0; i < sensor[j].sensor_param_num; i++) {
			ret = sensor[j].sensor_ops.get(&sensor[j].param_info[i], &val);
			if(-1 == ret) {
				DB_PRINT(DB_ERR, "ERROR:failed control fan speed");
				break;
			} else {
				param_name[i] = sensor[j].param_info[i].file_name;
				param_val[i] = val;
			}
		}
		
		if(ret != -1) {
			//第一次需要把参数名赋值
			if(j == 0) {
				json_param_name = cJSON_CreateStringArray((const char **)&param_name,i);
				cJSON_AddItemToObject(root, RPC_SENSOR_PARAM_NAME_KEY,json_param_name);
			}
			sprintf(param_val_key,"%s%d",RPC_SENSOR_PARAM_VAL_KEY,j);
			json_param_val = cJSON_CreateIntArray(param_val,i);
			cJSON_AddItemToObject(root,param_val_key,json_param_val);
		}
	}
	cJSON_AddNumberToObject(root, RPC_FUNC_ERR_KEY, ret);
	return root;
}


//整板12v输入电源，todo no err check
cJSON * rpc_server_get_mainpower(jrpc_context * ctx, cJSON * params, cJSON *id)
{
	return rpc_server_get_sensordata(ctx,params,id,sensor_adm1278,SENSOR_ADM1278_NUM);
}

//整板12v输入电源，todo no err check
cJSON * rpc_server_get_powerchip(jrpc_context * ctx, cJSON * params, cJSON *id)
{
	return rpc_server_get_sensordata(ctx,params,id,&sensor_pwr1014,1);
}
cJSON * rpc_set_fw(jrpc_context *ctx, cJSON * params, cJSON *id)
{
    int ret = 0;
    cJSON *tmp=NULL;
    int cpuid=0;
    tmp=cJSON_GetObjectItem(params,"cpuId");
    if(!tmp || !(tmp->string))
    {
        ret = -1;
        goto err;
    }
    cpuid=cpuid2int(tmp->string);
    tmp=cJSON_GetObjectItem(params,"hardwareVersion");
    if(tmp && tmp->valuestring)
    {
        ret += deal_HW((uint8_t *)tmp->valuestring,cpuid,1);
    }
    tmp=cJSON_GetObjectItem(params,"softwareVersion");
    if(tmp && tmp->string)
    {
        ret += deal_SW((uint8_t *)tmp->valuestring,cpuid,1);
    }
err:
    return cJSON_CreateNumber(ret); 
}

cJSON * rpc_get_fw(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    int i = 0;
    int tmp_flag=0;
    cJSON *root;
    cJSON *version;
    cJSON *tmp;
    uint8_t str_tmp[256]={0};
    root = cJSON_CreateObject();
    if(!root)
    {
        goto err;
    }
    cJSON_AddStringToObject(root, "boardType","LC");
    cJSON_AddStringToObject(root, "boardName",(const char *)get_board_name());
    

    cJSON_AddNumberToObject(root, "slotId",get_slot_id());
    if(deal_SN(str_tmp,0)==0)
    {
        cJSON_AddStringToObject(root, "serialNumber", (const char *)str_tmp);

    }
    version=cJSON_CreateArray();
    if(!version)
    {
        goto err;
    }
    for( i=0; i<HW_MAX;i++)
    {
        tmp = cJSON_CreateObject();
        tmp_flag = 0;
        if(!tmp)
        {
            goto err;
        }
        if(deal_HW(str_tmp,i,0)==0)
        {
            cJSON_AddStringToObject(tmp,"hardwareVersion", (const char *)str_tmp);
            tmp_flag |=1;
        }
        if(deal_SW(str_tmp,i,0)==0)
        {
            cJSON_AddStringToObject(tmp,"softwareVersion", (const char *)str_tmp);
            tmp_flag |=2;
        }
        if(tmp_flag == 0)
        {
            cJSON_Delete(tmp);
            continue;
        }
        cJSON_AddStringToObject(tmp,"cpuId",cpuid2str(i));
        cJSON_AddItemToArray(version,tmp);
    }
    cJSON_AddItemToObject(root,"versions", version);
err:
    return root;
}

//核心芯片电源
cJSON * rpc_server_get_chip_power(jrpc_context * ctx, cJSON * params, cJSON *id)
{
	return rpc_server_get_sensordata(ctx,params,id, sensor_tps53667, SENSOR_TPS53667_NUM);
}

//温度
cJSON * rpc_server_get_temp(jrpc_context * ctx, cJSON * params, cJSON *id)
{
	return rpc_server_get_sensordata(ctx,params,id,sensor_tmp75,SENSOR_TMP75_NUM);
}

cJSON * rpc_server_get_fan(jrpc_context * ctx, cJSON * params, cJSON *id)
{
    return rpc_server_get_sensordata(ctx,params,id,&sensor_fan,SENSOR_FAN_NUM);
}

cJSON * rpc_server_get_lc_max1617(jrpc_context * ctx, cJSON * params, cJSON *id)
{
    return rpc_server_get_sensordata(ctx, params,id, sensor_max1617,SENSOR_MAX1617_NUM);
}
cJSON * rpc_server_get_lc_pwr1220(jrpc_context * ctx, cJSON * params, cJSON *id)
{
    return rpc_server_get_sensordata(ctx, params,id, sensor_pwr1220,SENSOR_PWR1220_NUM);
}
cJSON * rpc_server_get_lc_tps53667(jrpc_context * ctx, cJSON *params, cJSON *id)
{
    return rpc_server_get_sensordata(ctx, params, id, sensor_tps53667, SENSOR_TPS53667_NUM);
}
cJSON * rpc_server_get_lc_adm1278(jrpc_context * ctx, cJSON *params, cJSON *id)
{
    return rpc_server_get_sensordata(ctx, params, id, sensor_adm1278, SENSOR_ADM1278_NUM);
}

//风扇控制
cJSON * rpc_server_ctrl_fan(jrpc_context * ctx, cJSON * params, cJSON *id)
{
	int i = 0;
	int ret;
		char speed = cJSON_GetObjectItem(params, "speed")->valueint;
	for(i = 0; i < sensor_fan.sensor_param_num; i++) {
		ret = sensor_fan.sensor_ops.set(&sensor_fan.param_info[i],(uint32_t)speed);
		if(-1 == ret) {
			DB_PRINT(DB_ERR, "ERROR:failed control fan speed");
			break;
		}
	}

    return cJSON_CreateNumber(ret);
}


static void load_cfg_file()
{
	dictionary  *ini ;	
    ini = iniparser_load(CONFIG_FILE_PATH);
	//char *port_string = NULL;
	
	if (ini == NULL) {
		DB_PRINT(DB_ERR, "ERROR:failed load config file %s",CONFIG_FILE_PATH);
        return;
    }

	gServerPort = iniparser_getint(ini, SECTION_NETWORK":"SETCTION_NETWORK_KEY_PORT,DEFAULT_SERVER_PORT);
	DB_PRINT(DB_INFO, "gServerPort %d", gServerPort);
}

static int init_log()
{
	int zlog_fd;
	printf("init zlog");
	zlog_fd = zlog_init(ZLOG_CFG_FILE);
    if (zlog_fd) {
        printf("init failed\n");
        return -1;
    }

	glog = zlog_get_category("rpc_server");
    if (!glog) {
        printf("get rpc_server log category fail\n");
        zlog_fini();
        return -2;
    }
    return 0;
}


int main(void)
{
	init_log();
	load_cfg_file();
	init_sensor();
    init_eeprom();

	my_server.debug_level = 5;
    jrpc_server_init(&my_server, gServerPort);
    //my_server.debug_level=1;
    jrpc_register_procedure(&my_server, say_hello, "sayHello", NULL );
	jrpc_register_procedure(&my_server, rpc_server_get_mainpower, RPC_FUNC_GET_MAIN_POWER_NAME, NULL );		//主电源
#if 0
	jrpc_register_procedure(&my_server, rpc_server_get_powerchip, RPC_FUNC_GET_POWERCHIP_NAME, NULL );		//主电源
	jrpc_register_procedure(&my_server, rpc_server_get_chip_power, RPC_FUNC_GET_CHIP_POWER_NAME, NULL );		//芯片电源
	jrpc_register_procedure(&my_server, rpc_server_get_temp, RPC_FUNC_GET_TEMP_NAME, NULL );				//温度
	jrpc_register_procedure(&my_server, rpc_server_ctrl_fan, RPC_FUNC_SET_FAN_NAME, NULL );				//风扇控制
	jrpc_register_procedure(&my_server, rpc_server_get_fan, RPC_FUNC_GET_FAN_NAME, NULL );				//温度
#endif
    jrpc_register_procedure(&my_server, rpc_get_fw, RPC_FUNC_GET_FW,NULL);
    jrpc_register_procedure(&my_server, rpc_set_fw, RPC_FUNC_SET_FW,NULL);

    jrpc_register_procedure(&my_server, rpc_server_get_lc_max1617,RPC_FUNC_GET_LC_MAX1617_NAME,NULL);
    jrpc_register_procedure(&my_server, rpc_server_get_lc_pwr1220,RPC_FUNC_GET_LC_PWR1220_NAME,NULL);
    jrpc_register_procedure(&my_server, rpc_server_get_lc_tps53667,RPC_FUNC_GET_LC_TPS53667_NAME,NULL);
    timer_check(my_server.loop);


    jrpc_server_run(&my_server);
    jrpc_server_destroy(&my_server);

    return 0;
}



