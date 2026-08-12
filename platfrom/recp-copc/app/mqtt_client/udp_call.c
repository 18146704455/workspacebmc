/*
 *ack %2 == 0 must
 */
#include <stdlib.h>
#include <string.h>
#include "udp_call.h"
#include "config.h"
#include "common_log.h"

void frame_master_ack(struct sockaddr_in *client,void *msg, int len)
{
    FUNC_START();
    char *new_host=NULL;
    
    asprintf(&new_host,"%s",(char*)inet_ntoa(client->sin_addr));
    if(strcmp(new_host,g_config->host)!=0)
    {
        asprintf(&g_config->host, "%s",new_host);
        //mosquitto_disconnect(g_config->mosq);
        pr_log("now link to %s",new_host);
        mosquitto_connect_async(g_config->mosq,g_config->host,g_config->port,g_config->keepalive);
    }
    free(new_host);
    FUNC_END();
}


