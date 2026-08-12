/*************************************************************************
  > File Name: mqtt_client.c
  > Author: 
  > Mail: 
  > Created Time: Mon 30 Dec 2019 07:59:06 PM CST
 ************************************************************************/
#include <mosquitto.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h> 
#include <string.h>
#include <unistd.h>
#include <cJSON.h>
#include "common_log.h"
#include "config.h"
#include "list.h"
#include "msg.h"
 
void message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    cJSON *root,*value;
    struct rpc_server *p;
    if(message->payloadlen){
        pr_debug("recv:%s %s\n", message->topic, message->payload);
    }else{
        pr_debug("%s (null)\n", message->topic);
    }

    //msg_check_topic(message->topic);
    if(message->payloadlen<=0)
    {
        return;
    }
    root = cJSON_Parse(message->payload);
    if(!root)
        return;
    //msg_check_version(root);
#if 0
    value = cJSON_GetObjectItem(root,"option");
    if(!value) goto error;
    if(!value->valuestring) goto error;
    if(strcasecmp(value->valuestring,"get")!=0) goto error;
#endif

    value = cJSON_GetObjectItem(root,"data_type");
    if(!value) goto error;
    if(!value->valuestring) goto error;
    if(!cJSON_GetObjectItem(root,value->valuestring)) goto error;

    list_for_each_entry(p,&g_config->server_root,node_root)
    {
        if(strcasecmp(value->valuestring,p->name)==0)
        {
            pr_debug("call server %s\n",p->name);
            p->call(cJSON_GetObjectItem(root,value->valuestring),p->data);
        }
    }
error:
    cJSON_Delete(root);
    return;

}

void connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    struct def *config;
    config=userdata;
    char tmp_buf[128];
    int ret;
    if(!result){
        /* Subscribe to broker information topics on successful connect. */
        sprintf(tmp_buf, "board/%s/%d", config->board_type, config->slot);
        ret = mosquitto_subscribe(mosq, NULL, tmp_buf, 0);
        if (MOSQ_ERR_SUCCESS != ret) {
            pr_log("mosquitto_subscribe %s failed %d", tmp_buf, ret);
        }
        sprintf(tmp_buf, "board/%s/broadcast", config->board_type);
        ret = mosquitto_subscribe(mosq, NULL, tmp_buf, 0);
        if (MOSQ_ERR_SUCCESS != ret) {
            pr_log("mosquitto_subscribe %s failed %d", tmp_buf, ret);
        }
        ret = mosquitto_subscribe(mosq, NULL, "frame/broadcast", 0);
        if (MOSQ_ERR_SUCCESS != ret) {
            pr_log("mosquitto_subscribe %s failed %d", "frame/broadcast", ret);
        }
    }else{
        pr_err("Connect %s failed after try", config->host);
        mosquitto_reconnect(mosq);
    }
}
static void disconnect_callback(struct mosquitto *mosq, void *userdata, int rc)
{
    struct def *config;
    config=userdata;
    pr_log("mosq disconnect %s after try",config->host);
    mosquitto_connect_async(mosq,config->host,config->port,config->keepalive);
}
static void subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    int i;

    pr_debug("Subscribed (mid: %d): %d", mid, granted_qos[0]);
    for(i=1; i<qos_count; i++){
        pr_debug(", %d", granted_qos[i]);
    }
    pr_debug("\n");
    pr_debug("mosq:%p",mosq);
}

void log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    /* Pring all log messages regardless of level. */
    //printf("%s\n", str);
}
void publish_callback(struct mosquitto *mosq, void *obj, int mid)
{
    //pr_debug("pub ok\n");
}
extern struct def *g_config;
int send_msg(char *data, size_t len)
{
    int ret;
    pr_debug("%s",data);
    ret = mosquitto_publish(g_config->mosq, NULL, g_config->default_topic, len, data, 0, false);
    if (MOSQ_ERR_SUCCESS != ret) {
        pr_log("mosquitto_publish %s to %s failed %d", data, g_config->default_topic, ret);
    }
    return 1;
}
int send_mqtt_msg(char *topic, char *data, size_t len)
{
    int ret;
    pr_debug("%s : %s ",topic, data);
    ret = mosquitto_publish(g_config->mosq, NULL, topic, len, data, 0, false);
    if (MOSQ_ERR_SUCCESS != ret) {
        pr_log("mosquitto_publish %s to %s failed %d", data, topic, ret);
    }
}
int init_mqtt(struct def *config)
{
    bool clean_session = false;
    int ret;
    ret = mosquitto_lib_init();
    if (MOSQ_ERR_SUCCESS != ret) {
        pr_log("mosquitto_lib_init failed %d", ret);
    }
    asprintf(&config->mosq_id,"%s %d %s",config->board_type,config->slot,config->sn);
    config->mosq = mosquitto_new(config->mosq_id, clean_session, config);
    if (NULL == config->mosq) {
        mosquitto_lib_cleanup();
        pr_log("mosquitto_new failed : %s", strerror(errno));
        return 1;
    } else {
        pr_log("mosquitto_new id : %s", config->mosq_id);
    }
    mosquitto_log_callback_set(config->mosq, log_callback);
    mosquitto_connect_callback_set(config->mosq, connect_callback);
    mosquitto_message_callback_set(config->mosq, message_callback);
    mosquitto_subscribe_callback_set(config->mosq, subscribe_callback);
    mosquitto_publish_callback_set(config->mosq, publish_callback);
    mosquitto_disconnect_callback_set(config->mosq, disconnect_callback);
    //mosquitto_threaded_set(config->mosq,true);
    //mosquitto_will_set(config->mosq,"dev/offline",29,"{\"board\":\"FAB\",\"solitid\":1}",0,false);
    
    ret = mosquitto_connect_async(config->mosq, config->host, config->port, config->keepalive);
    if (MOSQ_ERR_SUCCESS != ret) {
        mosquitto_destroy(config->mosq);
        mosquitto_lib_cleanup();
        pr_log("mosquitto_connect_async err %d.\n", ret);
        return 1;
    }

    //mosquitto_loop_forever(mosq, -1, 1);
    ret = mosquitto_loop_start(config->mosq);
    if (MOSQ_ERR_SUCCESS != ret) {
        mosquitto_destroy(config->mosq);
        mosquitto_lib_cleanup();
        pr_log("smosquitto_loop_start err %d.\n", ret);
        return 1;
    }
    return 0;
}
