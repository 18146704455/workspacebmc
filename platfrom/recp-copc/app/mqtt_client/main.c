/*************************************************************************
  > File Name: main.c
  > Author: 
  > Mail: 
  > Created Time: Fri 27 Dec 2019 05:15:48 PM CST
 ************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ev.h>
#include "common_log.h"
#include "mqtt_client.h"
#include "list.h"
#include "time_check.h"
#include "smi_translayer.h"
#include "config.h"
#include "board.h"
#include "fan.h"

ev_idle idle;
void init_globle(struct def *config)
{
    config->loop = ev_default_loop(0);
    INIT_LIST_HEAD(&config->root);
    INIT_LIST_HEAD(&config->timer_root);
    INIT_LIST_HEAD(&config->temp_root);
    INIT_LIST_HEAD(&config->psu_root);
    INIT_LIST_HEAD(&config->server_root);
    INIT_LIST_HEAD(&config->fan_root);
    INIT_LIST_HEAD(&config->board_root);
}
void  init_cfg_file(struct def *config)
{
    config->host=strdup("172.16.3.30");
    config->keepalive=60;
    config->port=1883;
    config->default_topic=strdup("frame/server");
}


static void idle_cb(EV_P_ ev_idle *w, int revents)
{
    usleep(10000);
    //pr_debug("idle");
    //public_board_msg(NULL);
}


struct def *g_config;
extern int init_power(struct def *);
extern int  init_temp(struct def *);
extern int init_func(struct def *);
extern int init_server(struct def *);

int main(int argc, char *argv[])
{
    struct def conf;
    memset(&conf,0,sizeof(struct def));
    g_config=&conf;
    init_globle(g_config);
    //init_bus
    //init_driver

    init_cfg_file(g_config);
    //init_log(g_config);
    g_config->udp_s=udp_init("","");
    printf("init_board start\n");
    init_board(g_config);
    printf("init_board end\n");

    printf("init_server start\n");
    init_server(g_config);
    printf("init_server end\n");
    printf("set_udp_loop start\n");
    set_udp_loop(g_config->udp_s,g_config->loop);
    printf("set_udp_loop end\n");

    printf("init_power start\n");
    init_fan(g_config);
    init_power(g_config);
    printf("init_power end\n");
    printf("init_temp start\n");
    init_temp(g_config);
    printf("init_temp end\n");
    // timer_check(g_config);

    if (init_mqtt(g_config)) {
        pr_log("init_mqtt falied");
        exit(EXIT_FAILURE);
    }

    ev_idle_init(&idle,idle_cb);
    ev_idle_start(g_config->loop, &idle);
    ev_run(g_config->loop,0);

    //unmapSmiMem();
    return 0;
}
