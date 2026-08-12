#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "rpc_func.h"
#define PORT 57789  
#define NET_BASE 0xac100336

static char *app_name = NULL;

// #define BMC_VLAN_NET 0xAC106701
static uint32_t server_ip[6]={ 0xAC106401,0xAC106501,0xAC106601,0xAC106701,0xAC106801,0xAC106901};

int involk_rpc_request(uint32_t dip, uint16_t dport, simple_rpc_request_data *data, simple_rpc_respond_data_s *rpcReponse)
{
	int sockfd;
	struct sockaddr_in server_addr;

	int		ret = 0;
	int actualLen = 0;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if ( sockfd < 0 ){	/*出错*/
		printf( "socket error\n");
		return -1;
	}

	/*just for test */
#if 0
	flags = fcntl(sockfd,F_GETFL,0);//获取建立的sockfd的当前状态（非阻塞）
	fcntl(sockfd,F_SETFL,flags|O_NONBLOCK);//将当前sockfd设置为非阻塞
#endif
	/*end*/

	/*设置服务器地址*/
	bzero(&server_addr, sizeof(server_addr));	/*清零*/
	server_addr.sin_family = AF_INET;		/*协议族*/

	server_addr.sin_addr.s_addr = htonl(dip);
	server_addr.sin_port = htons(dport);	/*服务器端口*/

	/*连接服务器*/
	errno = 0;
	ret = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
	if ( 0 != ret ) {
		printf("connect err:%s\n", strerror(errno));
		close(sockfd);
		return -1;
	}

	actualLen = write(sockfd, data, sizeof(simple_rpc_request_data));
	if ( actualLen < 0 ) {
		printf( "write sockfd:%d fail\n", sockfd);
		close(sockfd);
		return -1;
	}

	usleep(500000);
	actualLen = recv(sockfd, rpcReponse, sizeof(simple_rpc_respond_data_s), 0);
	if ( actualLen < 0 ) {
		printf( "write sockfd:%d fail\n", sockfd);
		close(sockfd);
		return -1;
	}

	close(sockfd);
	return 0;
}

#define RPC_FUNC_GET_SLOT_ID "slot_info"
#define RPC_FUNC_GET_BOARDTYPE "get_board_type"
#define RPC_FUNC_GET_SUBMODULE_SLOT_ID "submodule_slot_info"
#define RPC_FUNC_SET_SUBMODULE_SERVICE_STATUS "set_service_run_status"

/**
 * \brief Print help for this application
 */
void print_help(void)
{
	printf("\n Usage: %s [OPTIONS]\n\n", app_name);
	printf("  Options:\n");
	printf("   -h --help         Print this help\n");
	printf("   -s --slot_info    get board slot_id\n");
	printf("   -d --get_board_type  git board_type\n");
	printf("   -b --submodule_slot_info     get dpc_board submoduleslot_id\n");
	printf("   -r --set_service_run_status  set dpum_board service led status and link\n");
	printf("\n");
}


/*如果去除cpld读取槽位信息，并用cpu和Bmc之间的网卡通信获取shot，因为两者并未设置vlan网卡，所以获取会失效(回复的值来源不确定)*/
int main(int argc,char *argv[])
{
	static struct option long_options[] = {
		{"slot_info", no_argument, 0, 's'},
		{"slot_info_dpc", no_argument, 0, 'd'},
		{"submodule_slot_info", no_argument, 0, 'b'},
		{"help", no_argument, 0, 'h'},
		{"set_service_run_status", no_argument, 0, 'r'},
		{NULL, 0, 0, 0}
	};

	int i=0,ret=0;
	uint16_t subslot_slot;
	char buf[10]={3,2};
	simple_rpc_request_data rpcRequstSlot;
	simple_rpc_respond_data_s rpcReponse;
	rpcRequstSlot.dataLen = 0;
	int value, option_index = 0;
	char *log_file_name = NULL;
	int start_daemonized = 0;

	app_name = argv[0];

	/* Try to process all command line arguments */
	while ((value = getopt_long(argc, argv, "sdbrh", long_options, &option_index)) != -1) {
		switch (value) {
			case 's':
				strcpy(rpcRequstSlot.funcName, RPC_FUNC_GET_SLOT_ID);

					ret=involk_rpc_request(NET_BASE, PORT, &rpcRequstSlot, &rpcReponse);
					if(ret==0)
					{
						printf("rpcReponse:%s result:%d len:%d data0:%#x \n",\
						rpcReponse.funcName, rpcReponse.result, rpcReponse.dataLen, rpcReponse.data[0]);
						break;
					}

				break;
			case 'd':
				strcpy(rpcRequstSlot.funcName, RPC_FUNC_GET_BOARDTYPE);

				ret=involk_rpc_request(NET_BASE, PORT, &rpcRequstSlot, &rpcReponse);
				if(ret==0)
				{
					printf("rpcReponse:%s result:%d len:%d boardtype:%s\n",\
					rpcReponse.funcName, rpcReponse.result, rpcReponse.dataLen,  rpcReponse.data);

				}
				
				break;
			case 'b':
				strcpy(rpcRequstSlot.funcName, RPC_FUNC_GET_SUBMODULE_SLOT_ID);
				for(i=0;i<6;i++)
				{
					ret=involk_rpc_request(server_ip[i], PORT, &rpcRequstSlot, &rpcReponse);
					if(ret==0)
					{
						// printf("rpcReponse:%s result:%d len:%d data0:%#x data1:%#x\n",\
						rpcReponse.funcName, rpcReponse.result, rpcReponse.dataLen, rpcReponse.data[0],rpcReponse.data[1]);	
						break;
					}
				}
				break;
			case 'r':
				strcpy(rpcRequstSlot.funcName, RPC_FUNC_SET_SUBMODULE_SERVICE_STATUS);
				for(i=0;i<6;i++)
				{	
					strncpy(rpcRequstSlot.data,buf,sizeof(uint16_t));
					ret=involk_rpc_request(server_ip[i], PORT, &rpcRequstSlot, &rpcReponse);
					if(ret==0)
					{
						// printf("rpcReponse:%s result:%d len:%d data0:%#x data1:%#x\n",\
						rpcReponse.funcName, rpcReponse.result, rpcReponse.dataLen, rpcReponse.data[0],rpcReponse.data[1]);	
						break;
					}
				}
				break;
			case 't':
				strcpy(rpcRequstSlot.funcName, RPC_FUNC_GET_BOARDTYPE);
				ret=involk_rpc_request(NET_BASE, PORT, &rpcRequstSlot, &rpcReponse);
				if(ret==0)
				{
					printf("rpcReponse:%s result:%d len:%d boardtype:%s\n",\
					rpcReponse.funcName, rpcReponse.result, rpcReponse.dataLen,  rpcReponse.data);

				}
				
				break;
			case 'h':
				print_help();
				break;
			default:
				break;
		}
	}
	return rpcReponse.data[0];
}