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
#include "jsonrpc-c.h"
#include "common_log.h"

#ifdef DB_CURR_LEVEL 
#undef DB_CURR_LEVEL
#define DB_CURR_LEVEL DB_DEBUG
#endif

#define PORT 10234  // the port users will be connecting to

struct jrpc_server my_server;

#define FAN_NUM 	3
#define FAN_FIEL_BASENAME	"/sys/bus/i2c/drivers/fancpld/73-0033/"
#define FAN_FIEL_NAME1		 FAN_FIEL_BASENAME"fantray1_pwm"
#define FAN_FIEL_NAME2		 FAN_FIEL_BASENAME"fantray2_pwm"
#define FAN_FIEL_NAME3		 FAN_FIEL_BASENAME"fantray3_pwm"
static int fanfd[FAN_NUM] = {-1,-1,-1};
static char *fanfilename[3] = {
	FAN_FIEL_NAME1,
	FAN_FIEL_NAME2,
	FAN_FIEL_NAME3,
};

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


int set_fan_duty(char speed)
{
   char cmd[256] = {0};
   char real_speed = 0;
   int iret = 0 , i = 0;

    if(speed < 50 || speed > 100)  //keep mininum 50% duty cycle
        return;

    if(100 == speed) speed = 95; //max duty 0x19

    real_speed = 0x10 + (speed - 50) / 5;
    sprintf(cmd,"0x%x", real_speed);
    DB_PRINT(DB_DEBUG, "speed %s", cmd);

    for(i = 0;i < FAN_NUM; i++) {
    	iret = write(fanfd[i], cmd, strlen(cmd));
    	if(iret == -1){
    		DB_PRINT(DB_ERR, "ERROR:failed write fan device file %s %s", fanfilename[i],cmd);
    		iret = -1;
    		break;
    	}
    }

    return iret;
}


cJSON * rpc_set_fan_duty(jrpc_context * ctx, cJSON * params, cJSON *id) 
{
	char speed = cJSON_GetObjectItem(params, "speed")->valueint;
   	int ret = set_fan_duty(speed);
   	if(-1 == ret) {
   		DB_PRINT(DB_ERR, "ERROR:failed control fan speed");
   	}

    return cJSON_CreateNumber(ret);
}


int init_fanctl(void)
{
	int i = 0;
	for(i = 0;i < FAN_NUM;i++) {
		fanfd[i] = open(fanfilename[i],O_RDWR);
		if( -1 == fanfd[i] ) {
			DB_PRINT(DB_ERR, "ERROR:failed open fan device file %s", fanfilename[i]);
			return -1;
		}
	}

	return 0;
}

#if 1
int main(void) 
{
	int iret = 0;
	iret = init_fanctl();
	if(iret == -1) {
		DB_PRINT(DB_ERR, "ERROR:failed int fan device");
		return -1;
	}
    my_server.debug_level = 1;
    jrpc_server_init(&my_server, PORT);
    //my_server.debug_level=1;
    jrpc_register_procedure(&my_server, say_hello, "sayHello", NULL );
	jrpc_register_procedure(&my_server, rpc_set_fan_duty, "setFanDuty", NULL );
    jrpc_server_run(&my_server);
    jrpc_server_destroy(&my_server);

    return 0;
}

#endif

