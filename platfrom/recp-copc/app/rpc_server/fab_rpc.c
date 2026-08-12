#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "cJSON.h"
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include "board_sensor_rpc_server.h"
#include "common_log.h"
#include "iniparser.h" 
#include <strings.h>


#ifdef DB_CURR_LEVEL 
#undef DB_CURR_LEVEL
#define DB_CURR_LEVEL DB_DEBUG
#endif



zlog_category_t *glog;


#define MAX_SERVER 	16
static unsigned long gid = 0;
static int grpc_fd[MAX_SERVER] = {0};
char grpc_ip[MAX_SERVER][25];


#define JRPC_HOST           "127.0.0.1"
#define JRPC_PORT           10234
static int gServerPort = 0;

static void setnonblock(int fd)
{  
    fcntl(fd,F_SETFL,fcntl(fd,F_GETFL) | O_NONBLOCK);  
}  

static void setaddress(const char* ip,int port,struct sockaddr_in* addr)
{  
    bzero(addr,sizeof(*addr));  
    addr->sin_family=AF_INET;  
    inet_pton(AF_INET,ip,&(addr->sin_addr));  
    addr->sin_port=htons(port);  
} 

static int new_tcp_client(const char* ip,int port){  

    int iret = 0;

    int fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);  
    if(-1 == fd) {
        DB_PRINT(DB_ERR,"socket:%s",strerror(errno));
        return -1;
    }
    
    struct sockaddr_in addr;  
    setaddress(ip,port,&addr);  
    iret = connect(fd,(struct sockaddr*)(&addr),sizeof(addr));  
    if(-1 == iret) {
        DB_PRINT(DB_ERR,"socket:%s",strerror(errno));
        return -1;
    }
    
    setnonblock(fd);  
    return fd;  
}

//todo 拔插动作未实现
cJSON *invoke_rpc(char *func,cJSON *params,int serverid)
{
    const char * out;
    cJSON *root,*retroot; 
    int iret,rbufpos = 0;
    int trytimes = 10;
    int id = 1,retid;
    char rbuff[1024] = {0};

    id = gid;
    
    gid++;
    
    if(grpc_fd[serverid] == 0 ||grpc_fd[serverid] == -1) {
        grpc_fd[serverid] = new_tcp_client(grpc_ip[serverid],gServerPort);
        if(-1 == grpc_fd[serverid]) {
            DB_PRINT(DB_ERR, "ERROR:can't create socket");    
            return NULL;
        }
    }

    if(func == NULL) {
        DB_PRINT(DB_ERR, "ERROR:can't invoke null function"); 
        return NULL;
    }

    /*构建调用json*/
    //未做检测，fix
    root = cJSON_CreateObject(); 
    cJSON_AddStringToObject(root, "jsonrpc", "2.0");
    cJSON_AddStringToObject(root, "method", func);
	
	if(params != NULL) {
		cJSON_AddItemToObject(root, "params", params);
	}
    
    cJSON_AddNumberToObject(root, "id", id);
    out = cJSON_Print(root);
    if(NULL == out) {
        DB_PRINT(DB_ERR, "ERROR:params json error");    
        cJSON_Delete(root); 
        return NULL;
    } else {
       DB_PRINT(DB_DEBUG, "%s",out); 
    }
   

    //发送至对端
    iret =  send(grpc_fd[serverid],out,strlen(out)+1,0);
    if(-1 == iret || (iret != strlen(out)+1)) {
        DB_PRINT(DB_ERR,"ERROR:send:%s",strerror(errno));
        cJSON_Delete(root); 
        return NULL;
    }
	

    cJSON_Free((void *)out);

    //接受处理返回
    usleep(5000);
    while(1) 
    {
        iret = recv(grpc_fd[serverid],rbuff+rbufpos,sizeof(rbuff),0);
        if(-1 == iret) {
            if(trytimes--) {
                usleep(5000);
                continue;
            } else {
                DB_PRINT(DB_ERR,"WAIT REPONSE TIMEOUT:socket:%s",strerror(errno));
                return NULL;
            }
        } else if(0 == iret){
			close(grpc_fd[serverid]);
			grpc_fd[serverid] = -1;
			return NULL;
		}

      //  DB_PRINT(DB_DEBUG, "Recv iret %d<---:\r%s",iret,rbuff); 
        retroot = cJSON_Parse(rbuff);
        if(NULL == retroot) {
            rbufpos+=iret;
            continue;
        } else {
            
            //DB_PRINT(DB_DEBUG, "Recv cjson<---:\r%s",cJSON_Print(retroot)); 
            //检测请求和返回id是否一致，不一致代表不匹配
            retid = cJSON_GetObjectItem(retroot,"id")->valueint;
            if(retid != id) {
                return NULL;
            }
        }
        
        break;
    }
    return retroot;
}


int rpc_get_data(int boardid,char *rpc_remote_func, sensor_data_str *data,int num)
{
    cJSON *retroot,*result,*param_name_array,*param_val_array;
	cJSON *param_name,*param_val,*sensor_name = NULL;
	int err,iret = 0,i = 0,j = 0;
	int param_num = 0;
	int sensor_num = 0;
	char param_val_key[25];

	if(rpc_remote_func == NULL ) {
		DB_PRINT(DB_ERR, "ERROR:miss remote func name ");
		return -1;
	}
	if(data == NULL) {
		DB_PRINT(DB_ERR, "ERROR:can't use null pinter");
		return -1;
	}
	
    retroot = invoke_rpc(rpc_remote_func,NULL,boardid);
	if(NULL == retroot) {
		DB_PRINT(DB_ERR, "ERROR:failed invoke %s",rpc_remote_func);
		return RPC_ERR;
	}

	result = cJSON_GetObjectItem(retroot,"result");
    if(NULL == result) {
        iret = RPC_ERR;
		goto FAILED_GET_RESULT;
    }

	err = cJSON_GetObjectItem(result, RPC_FUNC_ERR_KEY)->valueint;
    if(RPC_FUNC_OK != err) {
         DB_PRINT(DB_ERR, "ERROR:remote func %s exec error ",rpc_remote_func);
		 iret = RPC_REMOTE_FUNC_ERR;
		goto FAILED_GET_RESULT;
    } 
    
    param_num = cJSON_GetObjectItem(result, RPC_SENSOR_PARAM_NUM_KEY)->valueint;
	DB_PRINT(DB_DEBUG,"param_num %d",param_num);

	sensor_num = cJSON_GetObjectItem(result, RPC_SENSOR_NUM_KEY)->valueint;
	if(sensor_num > num) {
		DB_PRINT(DB_DEBUG,"sensor num %d",sensor_num);
		iret = RPC_LOCAL_SPACE_SHORT;
		goto FAILED_LOCAL_SPACE_SHORT;
	}

	sensor_name = cJSON_GetObjectItem(result, RPC_SENSOR_NAME_KEY);
	if(sensor_name == NULL){
		DB_PRINT(DB_DEBUG,"sensor name miss");
	}
	
	for(j = 0;j < sensor_num;j++) {
		if(sensor_name != NULL) {
			DB_PRINT(DB_DEBUG,"sensor name %s",sensor_name->valuestring);
			strcpy(data[j].name,sensor_name->valuestring);
			
		}
	
		data[j].num = param_num;
		param_name_array = cJSON_GetObjectItem(result, RPC_SENSOR_PARAM_NAME_KEY);
		if(NULL == param_name_array) {
			DB_PRINT(DB_ERR, "ERROR:miss param name key");
			iret = RPC_REMOTE_FUNC_ERR;
			goto FAILED_MISS_PARAM_NAME;
		}
		
		sprintf(param_val_key,"%s%d",RPC_SENSOR_PARAM_VAL_KEY,j);
		param_val_array = cJSON_GetObjectItem(result, param_val_key);
		if(NULL == param_val_array) {
			iret = RPC_REMOTE_FUNC_ERR;
			DB_PRINT(DB_ERR, "ERROR:miss param name key");
			goto FAILED_MISS_PARAM_VAL;
		}
	
		
		for(i = 0;i < param_num;i++) {
			param_name = cJSON_GetArrayItem(param_name_array,i); 
			param_val = cJSON_GetArrayItem(param_val_array,i);
			DB_PRINT(DB_DEBUG,"%s->%d",param_name->valuestring,param_val->valueint);
			strcpy(data[j].data[i].label,param_name->valuestring);
			data[j].data[i].val = param_val->valueint;
		}
	}
	iret = sensor_num;
FAILED_LOCAL_SPACE_SHORT:   
FAILED_MISS_PARAM_VAL:	
//	cJSON_Delete(param_val);
FAILED_MISS_PARAM_NAME:
//	cJSON_Delete(param_name);
FAILED_GET_RESULT:
//	cJSON_Delete(result);
	cJSON_Delete(retroot); 	 

	return iret;
}

static int init_log()
{
	int zlog_fd;
	printf("init zlog\n");
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
}


int rpc_get_mainpower(int boardid)
{
	int iret = 0;
	sensor_data_str data;
	iret = rpc_get_data(boardid,RPC_FUNC_GET_MAIN_POWER_NAME,&data,1);
	if(iret < 0) {
		DB_PRINT(DB_ERR, "ERROR:CALL remote func %s failed",RPC_FUNC_GET_MAIN_POWER_NAME);
		return -1;
	}
	return 0;
}

int rpc_get_powerchip(int boardid)
{
		int iret = 0;
		sensor_data_str data;
		iret = rpc_get_data(boardid,RPC_FUNC_GET_POWERCHIP_NAME,&data,1);
		if(iret != RPC_FUNC_OK) {
			DB_PRINT(DB_ERR, "ERROR:CALL remote func %s failed",RPC_FUNC_GET_POWERCHIP_NAME);
			return -1;
		}
		return 0;
}

int rpc_get_corechip_power(int boardid)
{
	int iret = 0,i;
	sensor_data_str data[2];
	iret = rpc_get_data(boardid,RPC_FUNC_GET_CHIP_POWER_NAME,data,2);
	if(iret < 0) {
		DB_PRINT(DB_ERR, "ERROR:CALL remote func %s failed",RPC_FUNC_GET_CHIP_POWER_NAME);
		return -1;
	}

	return 0;
}

int rpc_get_temp(int boardid)
{
	int iret = 0;
	sensor_data_str data[4];
	iret = rpc_get_data(boardid,RPC_FUNC_GET_TEMP_NAME,data,4);
	if(iret < 0) {
		DB_PRINT(DB_ERR, "ERROR:CALL remote func %s failed",RPC_FUNC_GET_CHIP_POWER_NAME);
		return -1;
	}

	return 0;
}

int rpc_set_fan(int boardid, int speed)
{
		cJSON *root, *retroot;
		root = cJSON_CreateObject();
		cJSON_AddNumberToObject(root,"speed", speed);
		retroot = invoke_rpc("sFan",root,boardid);
		if(NULL == retroot) {
			DB_PRINT(DB_ERR, "ERROR:failed invoke remote mdio read");
			return -1;
		} else {
			printf("result:%s\n",cJSON_GetObjectItem(retroot,"result")->valuestring);
		}
		return 0;
}


int rpc_sayHello(int boardid)
{
			cJSON  *retroot;
			retroot = invoke_rpc("sayHello",NULL,boardid);
			if(NULL == retroot) {
				DB_PRINT(DB_ERR, "ERROR:failed invoke remote mdio read");
				return -1;
			} else {
				printf("result:%s\n",cJSON_GetObjectItem(retroot,"result")->valuestring);
			}
	return 0;
}

static int load_cfg_file()
{
	dictionary  *ini ;	
    ini = iniparser_load(CONFIG_FILE_PATH);
	char *port_string = NULL;

	int serverip_num = 0,i;
	char *section_key[MAX_SERVER];
	char *tmp_ip_string;
	char **pret;
	
	
	if (ini == NULL) {
		DB_PRINT(DB_ERR, "ERROR:failed load config file %s",CONFIG_FILE_PATH);
        return -1;
    }

	gServerPort = iniparser_getint(ini, SECTION_NETWORK":"SETCTION_NETWORK_KEY_PORT,DEFAULT_SERVER_PORT);
	DB_PRINT(DB_INFO, "gServerPort %d", gServerPort);

	serverip_num = iniparser_getsecnkeys(ini,SETCTION_SERVERIP);
	DB_PRINT(DB_INFO, "serverip_num %d", serverip_num);
	if(serverip_num > MAX_SERVER) {
		DB_PRINT(DB_INFO, "MAX server num is %d,config is %d", MAX_SERVER,serverip_num);
		serverip_num = MAX_SERVER;
	} else if(serverip_num == 0) {
		DB_PRINT(DB_ERR, "ERROR:get no serverip");
		return -1;
	}

	for(i = 0;i < MAX_SERVER;i++) {
		section_key[i] = (char *)malloc(25);
		if(section_key[i] == NULL) {
			DB_PRINT(DB_ERR, "ERROR:malloc for config string falied:%s",strerror(errno));
			return -1;
		}
	}
	
	pret = iniparser_getseckeys(ini,SETCTION_SERVERIP, (const char **)section_key);
	if(pret != NULL) {
		for(i = 0;i < serverip_num;i++) {
			//todo cp is better?
			DB_PRINT(DB_DEBUG, "key[%d]:%s",i,(char *)section_key[i]);
			tmp_ip_string = iniparser_getstring(ini, (const char *)section_key[i],NULL);
			if(tmp_ip_string != NULL) {
				strcpy(grpc_ip[i],tmp_ip_string);
				DB_PRINT(DB_DEBUG, "IP[%d]:%s",i,(char *)grpc_ip[i]);
			}
		}
	} else {
		DB_PRINT(DB_ERR, "ERROR:get no serverip keys");
	}
	
	return 0;
}

int main()
{
	int iret;
	iret = load_cfg_file();
	if(iret != 0) {
		DB_PRINT(DB_ERR, "ERROR:load config failed");
		return -1;
	}

	rpc_set_fan(0,95);
	rpc_get_mainpower(0);
	rpc_get_powerchip(0);
	rpc_get_corechip_power(0);
	rpc_get_temp(0);

#if 0
    cJSON *root, *retroot,*result;
    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root,"speed", 80);

    retroot = invoke_rpc("sayHello",root);
    if(NULL == retroot) {
        DB_PRINT(DB_ERR, "ERROR:failed invoke remote mdio read");
        return -1;
    } else {
        printf("result:%s\n",cJSON_GetObjectItem(retroot,"result")->valuestring);
    }

	retroot = invoke_rpc("gMainPower",NULL);
    if(NULL == retroot) {
        DB_PRINT(DB_ERR, "ERROR:failed invoke gMainPower");
        
    } else {
       // printf("result:%s\n",cJSON_GetObjectItem(retroot,"result")->valuestring);
    }
	
	cJSON_AddNumberToObject(root,"speed", 90);
	retroot = invoke_rpc("sFan",root);
    if(NULL == retroot) {
        DB_PRINT(DB_ERR, "ERROR:failed invoke remote mdio read");
        return -1;
    } else {
        printf("result:%s\n",cJSON_GetObjectItem(retroot,"result")->valuestring);
    }
#endif

    return 0;
}
