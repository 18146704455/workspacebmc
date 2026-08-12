/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-10-17 13:45:01
 * @LastEditors: smith.zj
 * @LastEditTime: 2020-04-23 14:47:39
 */
#ifndef __NETCFG_H__
#define __NETCFG_H__

#include <stdint.h>
#include <stdbool.h>
#include "hyperion_vlan.h"

typedef uint32_t NET_PORT;
typedef int     RET_STATUS;

#ifndef NET_RET_OK
#define NET_RET_OK              (0x00)   /* Operation succeeded                   */
#endif
#ifndef NET_RET_FAIL
#define NET_RET_FAIL            (0x01)   /* Operation failed                    */
#endif
#ifndef NET_RET_BAD_PARAM
#define NET_RET_BAD_PARAM       (0x04)   /* Illegal parameter in function called  */
#endif
#ifndef NET_RET_NO_SUCH
#define NET_RET_NO_SUCH         (0x0D)   /* No such item                    */
#endif
#ifndef NET_RET_NOT_SUPPORTED
#define NET_RET_NOT_SUPPORTED   (0x10)   /* This request is not support           */
#endif
#ifndef NET_RET_ALREADY_EXIST
#define NET_RET_ALREADY_EXIST   (0x1B)   /* Tried to create existing item         */
#endif
#ifndef NET_RET_BAD_CPU_PORT
#define NET_RET_BAD_CPU_PORT    (0x20)   /* Input CPU Port is not valid physical port number */
#endif
#ifndef NET_RET_FEATURE_NOT_ENABLE
#define NET_RET_FEATURE_NOT_ENABLE  (0x40) /*The feature not been enabled when do operation*/
#endif

typedef enum {
    NET_FALSE = 0,
    NET_TRUE = 1
} NET_BOOL;

typedef enum
{
	NET_8021Q_DISABLE = 0,
	NET_8021Q_FALLBACK,
	NET_8021Q_CHECK,
	NET_8021Q_SECURE,
    NET_8021Q_NONE      /*保持默认配置*/
} NET_8021Q_MODE;

#define NET_DEFAULT_PVID    1
#define NET_PVID_NONE   0   /*保持默认PVID配置，不做修改*/
#define NET_MAX_SWITCH_PORTS 11

#define MAX_CALC_VLAN_FUNC_NUM 7

typedef struct _net_port_cfg_s {
    NET_PORT port;
    NET_PORT portVectormap[NET_MAX_SWITCH_PORTS];
    uint8_t     port_map_num;
    uint16_t    pvid;
    uint8_t     qmode;
} net_port_cfg_s;

typedef enum
{
	NET_MEMBER_EGRESS_UNMODIFIED = 0,
	NET_MEMBER_EGRESS_UNTAGGED,
	NET_MEMBER_EGRESS_TAGGED,
	NET_NOT_A_MEMBER,
}   NET_PORT_MEMBER_TAG;

typedef struct
{
	NET_BOOL           useVIDFPri;
	uint8_t             vidFPri;
	NET_BOOL           useVIDQPri;
	uint8_t             vidQPri;
	NET_BOOL           dontLearn;
	NET_BOOL           filterUC;
	NET_BOOL           filterBC;
	NET_BOOL           filterMC;
	NET_BOOL		   routeDis;
	NET_BOOL	       mldSnoop;
	NET_BOOL	       igmpSnoop;
} NET_VTU_EXT_INFO;

typedef struct
{
	uint16_t        vid;
	uint16_t        fid;
	uint8_t         sid;
	NET_PORT_MEMBER_TAG    memberTagP[NET_MAX_SWITCH_PORTS];
	NET_BOOL           vidPolicy;
	NET_VTU_EXT_INFO   vidExInfo;
} VTU_ENTRY;


typedef struct _net_service_vlan_cfg_s {
    uint8_t calc_vlan_funcid;
    VTU_ENTRY vlan_entry;
    bool  slot_relative;        /*板间通信报文vlan与所处槽位相关*/
} net_service_vlan_cfg_s;


#define NET_CFG_BASE_XPATH "//board[@id='%s']/"
#define NET_CFG_SMIADDR_XPATH NET_CFG_BASE_XPATH"phyaddr"
#define NET_CFG_PORTCFG_XPATH     NET_CFG_BASE_XPATH"netport/port"
#define NET_CFG_VLANENTRY_XPATH     NET_CFG_BASE_XPATH"vlanentry/entry"
#define XML_DESCRIPTOR_FILE "/etc/netcfg/netcfg.xml"

typedef RET_STATUS (*vlanEntryAdd)(void *dev, VTU_ENTRY *vtuEntry,void *usr_context);
typedef RET_STATUS (*setPortVlanPorts)(void *dev, NET_PORT port, NET_PORT *portVectormap, uint16_t num,void *usr_context);
typedef RET_STATUS (*setPortVlanDot1qMode)(void *dev, NET_PORT port, uint8_t qmode,void *usr_context);
typedef RET_STATUS (*setPortVid)(void *dev,NET_PORT port,uint16_t pvid,void *usr_context);
typedef uint16_t (*calc_vlan)(uint16_t base,uint16_t id,uint8_t slot);

typedef struct net_ctrl_func_s {
    vlanEntryAdd vtuadd;
    setPortVlanPorts setpvlanVectorMap;
    setPortVlanDot1qMode setPortDot1qmode;
    setPortVid  setpvid;
}net_ctrl_func_s;

void dump_port_cfg_array(net_port_cfg_s *port_cfg,uint8_t len);

void dump_entry(VTU_ENTRY *pentry);
void dump_vlan_entry_array(net_service_vlan_cfg_s *pentry,int len);
void dump_port_cfg_array(net_port_cfg_s *port_cfg,uint8_t len);
void register_netctrl_func(vlanEntryAdd func_vtuadd,setPortVlanPorts func_setpvlanVectorMap,\
    setPortVlanDot1qMode func_setPortDot1qmode,setPortVid  func_setpvid);

RET_STATUS set_vlan_entry(void *dev,net_service_vlan_cfg_s *vlan_cfg,uint16_t len,void *usr_context);
RET_STATUS cfg_net_port(void *dev,net_port_cfg_s *port_cfg,uint8_t len,void *usr_context);

RET_STATUS cfgNetPort(void *dev,net_port_cfg_s *port_cfg,uint8_t len,void *usr_context);
#endif