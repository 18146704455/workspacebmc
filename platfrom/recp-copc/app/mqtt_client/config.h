/*************************************************************************
  > File Name: config.h
  > Author: 
  > Mail: 
  > Created Time: Sat 11 Jan 2020 03:23:27 PM CST
 ************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H
#include <stdbool.h>
#include <list.h>
#include "udp_server.h" 
struct def {
    struct list_head board_root;
    struct list_head timer_root;
    struct list_head psu_root;
    struct list_head temp_root;
    struct list_head server_root;
    struct list_head fan_root;
    struct list_head root;
    struct ev_loop *loop;
    //mqtt cfg
    char *host;
    char *mosq_id;
    int port;
    int keepalive;
    bool keep_msg;
    char *default_topic;
    struct mosquitto *mosq;
    //board
    int board_id;
    char *board_type;
    char *board_name;
    char *def_psu;
    char *sn;
    int slot;
    
    // udp_server
   struct udp_server_st *udp_s;
};
extern struct def *g_config;
#endif
