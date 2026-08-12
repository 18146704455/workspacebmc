/*************************************************************************
  > File Name: mqtt_client.h
  > Author: 
  > Mail: 
  > Created Time: Sat 11 Jan 2020 03:40:46 PM CST
 ************************************************************************/

#ifndef _MQTT_CLIENT_H
#define _MQTT_CLIENT_H
#include <stdio.h>
  
void send_msg(char *data, size_t len);
int init_mqtt(struct def *);
#endif
