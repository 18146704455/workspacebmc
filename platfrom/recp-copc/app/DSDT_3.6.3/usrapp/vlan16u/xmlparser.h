/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-10-23 14:53:15
 * @LastEditors: smith.zj
 * @LastEditTime: 2019-10-23 14:54:08
 */
#ifndef __XML_PARSER_H__
#define __XML_PARSER_H__
#include "netcfg.h"

RET_STATUS load_netdescriptor(const char *board_desc, uint16_t *smiaddr,net_port_cfg_s **port_cfg, uint16_t *port_cfg_len,\
            net_service_vlan_cfg_s **vlan_cfg,uint16_t *vlan_cfg_len);

#endif