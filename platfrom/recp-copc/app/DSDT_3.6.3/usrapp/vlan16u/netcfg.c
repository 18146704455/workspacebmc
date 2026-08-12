/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-10-22 15:54:48
 * @LastEditors: smith.zj
 * @LastEditTime: 2020-04-02 16:36:05
 */
#include "netcfg.h"
#include "common_log.h"
#include <string.h>
#include <stdarg.h>

char *QMOD_STR[NET_8021Q_NONE+1] = {
	"NET_8021Q_DISABLE",
	"NET_8021Q_FALLBACK",
	"NET_8021Q_CHECK",
	"NET_8021Q_SECURE",
    "NET_8021Q_NONE"      /*保持默认配置*/
};

static net_ctrl_func_s gnet_ctrl;

void DbgPrint(char* format, ...)
{
    va_list argP;
    char dbgStr[1000] = "";

    va_start(argP, format);

    vsprintf(dbgStr, format, argP);

        printf("%s", dbgStr);

    return;
}

uint16_t LC01_CALC_VLAN_BRO(uint16_t base,uint16_t id,uint8_t slot)
{
    PRINT("%s slot %d\n",__func__,slot);
    return VID_BRO_LC;
}

uint16_t LC01_CALC_VLAN_BRO_LC_CPU(uint16_t base,uint16_t id,uint8_t slot)
{
    PRINT("%s slot %d\n",__func__,slot);
    return VID_BRO_LC_CPU;
}

uint16_t CALC_VLAN_BRO_LC_NPS(uint16_t base,uint16_t id,uint8_t slot)
{
    PRINT("%s slot %d\n",__func__,slot);
    return VID_BRO_LC_NPS;
}

uint16_t CALC_UNI_CPU(uint16_t base,uint16_t id,uint8_t slot)
{
    PRINT("%s slot %d\n",__func__,slot);
    return VID_UNI_LC_CPU(slot);
}

uint16_t CALC_UNI_NPS0(uint16_t base,uint16_t id,uint8_t slot)
{
    PRINT("%s slot %d\n",__func__,slot);
    return VID_UNI_LC_NPS0(slot);
}

uint16_t CALC_UNI_NPS1(uint16_t base,uint16_t id,uint8_t slot)
{
    PRINT("%s slot %d\n",__func__,slot);
    return VID_UNI_LC_NPS1(slot);
}


static calc_vlan calc_vlan_func_array[MAX_CALC_VLAN_FUNC_NUM+1] = {
    NULL,
    LC01_CALC_VLAN_BRO,
    LC01_CALC_VLAN_BRO_LC_CPU,
    CALC_VLAN_BRO_LC_NPS,
    CALC_UNI_CPU,
    CALC_UNI_NPS0,
    CALC_UNI_NPS1,
};


void dump_entry(VTU_ENTRY *pentry)
{
	DbgPrint("0x%-5x0x%-5x0x%-5x%x %x %x %x %x %x %x %x %x %x %x   %-3x%--8x%-5x%-8x%-5x%-10x%-9x%-9x%-9x%-9x%-9x%-9x\n", pentry->fid, pentry->vid , pentry->sid,
			pentry->memberTagP[0], pentry->memberTagP[1], pentry->memberTagP[2], pentry->memberTagP[3], pentry->memberTagP[4], pentry->memberTagP[5],
			pentry->memberTagP[6], pentry->memberTagP[7], pentry->memberTagP[8], pentry->memberTagP[9], pentry->memberTagP[10], pentry->vidPolicy, 
			pentry->vidExInfo.useVIDFPri, pentry->vidExInfo.vidFPri, pentry->vidExInfo.useVIDQPri, pentry->vidExInfo.vidQPri,
			pentry->vidExInfo.dontLearn, pentry->vidExInfo.filterUC, pentry->vidExInfo.filterBC, pentry->vidExInfo.filterMC,pentry->vidExInfo.routeDis,pentry->vidExInfo.mldSnoop,pentry->vidExInfo.igmpSnoop);
}

void dump_vlan_entry_array(net_service_vlan_cfg_s *pentry,int len)
{
	int i;
	printf("\n------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("FID    VID    SID    MEMBERTAGP              VP USEFPRI FPRI USEQPRI QPRI DONTLEARN FILTERUC FILTERBC FILTERMC routeDis MLDSnoop IGMPSnoop \n");
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
	
	for(i = 0;i < len;i++) {
		dump_entry(&pentry[i].vlan_entry);
	}
}

void dump_port_cfg(net_port_cfg_s cfg)
{
    int j;
	int tmp = cfg.port_map_num;
	printf("port:%d pvid %d qmode:%s portbasevlan(%d):\n",cfg.port,cfg.pvid,QMOD_STR[cfg.qmode],cfg.port_map_num);
	if(cfg.port_map_num != 0) {
		printf("\t");
	}

    for(j = 0;j < tmp;j++) {
        printf("%d ",cfg.portVectormap[j]);
    }
	if(cfg.port_map_num != 0) {
		printf("\n");
	}
}

void dump_port_cfg_array(net_port_cfg_s *port_cfg,uint8_t len)
{
	int i;
   for(i = 0;i < len;i++) {
       dump_port_cfg(port_cfg[i]);
   }
}

void register_netctrl_func(vlanEntryAdd func_vtuadd,setPortVlanPorts func_setpvlanVectorMap,\
    setPortVlanDot1qMode func_setPortDot1qmode,setPortVid  func_setpvid)
{
    gnet_ctrl.setPortDot1qmode = func_setPortDot1qmode;
    gnet_ctrl.setpvid = func_setpvid;
    gnet_ctrl.setpvlanVectorMap = func_setpvlanVectorMap;
    gnet_ctrl.vtuadd = func_vtuadd;
}


RET_STATUS set_vlan_entry(void *dev,net_service_vlan_cfg_s *vlan_cfg,uint16_t len,void *usr_context)
{
	RET_STATUS status;
	int i = 0;

    if(gnet_ctrl.vtuadd == NULL) {
        DB_PRINT(DB_ERR,"NO vtuadd callbackfunc,you need register_netctrl_func first\n");
    }

	for(i = 0;i < len;i++) {
		if(vlan_cfg[i].slot_relative) {
            if(vlan_cfg[i].calc_vlan_funcid >= MAX_CALC_VLAN_FUNC_NUM || vlan_cfg[i].calc_vlan_funcid == 0) {
                DB_PRINT(DB_ERR,"ERROR:calc vlan funcid over range:%d (1~%d)",vlan_cfg[i].calc_vlan_funcid,MAX_CALC_VLAN_FUNC_NUM);
                return NET_RET_FAIL;
            } 
			if(calc_vlan_func_array[vlan_cfg[i].calc_vlan_funcid] != NULL) {
				memset(&vlan_cfg[i].vlan_entry.vidExInfo,0,sizeof(vlan_cfg[i].vlan_entry.vidExInfo));
                /*todo can add base*/
				vlan_cfg[i].vlan_entry.vid = calc_vlan_func_array[vlan_cfg[i].calc_vlan_funcid](0,0,*(uint8_t *)usr_context);
				vlan_cfg[i].vlan_entry.fid = vlan_cfg[i].vlan_entry.fid;
                PRINT("calc vlan %d\n",vlan_cfg[i].vlan_entry.vid);
			}
		}
    	
		if ((status = gnet_ctrl.vtuadd(dev, &vlan_cfg[i].vlan_entry, usr_context)) != NET_RET_OK)
		{
			DB_PRINT(DB_FATAL,"msdVlanEntryAdd returned fail.\n");
            DB_PRINT(DB_FATAL,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
		    dump_entry(&vlan_cfg[i].vlan_entry);
			return status;
		} 
	}
	return NET_RET_OK;
}

RET_STATUS cfgNetPort(void *dev,net_port_cfg_s *port_cfg,uint8_t len,void *usr_context)
{
        int i = 0;
    RET_STATUS status = NET_RET_OK;

    if(gnet_ctrl.setpvlanVectorMap == NULL || gnet_ctrl.setPortDot1qmode == NULL || gnet_ctrl.setpvid == NULL ) {
        DB_PRINT(DB_ERR,"MISS port config callbackfunc,you need register_netctrl_func first\n");
        return NET_RET_FAIL;
    }

    for(i = 0;i < len;i++) {
        /*
        *    1) set portbased vlan
        */  
       if(port_cfg[i].port_map_num != 0)  {
            status = gnet_ctrl.setpvlanVectorMap(dev,port_cfg[i].port,port_cfg[i].portVectormap,port_cfg[i].port_map_num,usr_context);
            if(status != NET_RET_OK) {
                DB_PRINT(DB_FATAL,"gvlnSetPortVlanPorts port %d returned fail.\n",i);
                return status;
            }
       }     


        if(NET_8021Q_NONE != port_cfg[i].qmode) {
            /*
            *    2) Enable 802.1Q for CPU port as mode
            */
            if((status = gnet_ctrl.setPortDot1qmode(dev, port_cfg[i].port, port_cfg[i].qmode,usr_context)) != NET_RET_OK) 
            {
                DB_PRINT(DB_ERR,"Set Port %d VlanDot1qMode return Failed\n",i);
                return status;
            } 
        }

        if(0 != port_cfg[i].pvid) {
            /*
            *    3) set port default vlan id
            */
            if((status = gnet_ctrl.setpvid(dev,port_cfg[i].port,port_cfg[i].pvid,usr_context)) != NET_RET_OK) {
                DB_PRINT(DB_ERR,"Set Port %d Vid %d returned fail.\n",port_cfg[i].port,port_cfg[i].pvid);
                return status;
            }
        }
    }
    return NET_RET_OK;
}

RET_STATUS cfg_net_port(void *dev,net_port_cfg_s *port_cfg,uint8_t len,void *usr_context)
{
    int i = 0;
    RET_STATUS status = NET_RET_OK;

    if(gnet_ctrl.setpvlanVectorMap == NULL || gnet_ctrl.setPortDot1qmode == NULL || gnet_ctrl.setpvid == NULL ) {
        DB_PRINT(DB_ERR,"MISS port config callbackfunc,you need register_netctrl_func first\n");
        return NET_RET_FAIL;
    }

    for(i = 0;i < len;i++) {
        /*
        *    1) set portbased vlan
        */  
       if(port_cfg[i].port_map_num != 0)  {
            status = gnet_ctrl.setpvlanVectorMap(dev,port_cfg[i].port,port_cfg[i].portVectormap,port_cfg[i].port_map_num,usr_context);
            if(status != NET_RET_OK) {
                DB_PRINT(DB_FATAL,"gvlnSetPortVlanPorts port %d returned fail.\n",i);
                return status;
            }
       }     


        if(NET_8021Q_NONE != port_cfg[i].qmode) {
            /*
            *    2) Enable 802.1Q for CPU port as mode
            */
            if((status = gnet_ctrl.setPortDot1qmode(dev, port_cfg[i].port, port_cfg[i].qmode,usr_context)) != NET_RET_OK) 
            {
                DB_PRINT(DB_ERR,"Set Port %d VlanDot1qMode return Failed\n",i);
                return status;
            } 
        }

        if(0 != port_cfg[i].pvid) {
            /*
            *    3) set port default vlan id
            */
            if((status = gnet_ctrl.setpvid(dev,port_cfg[i].port,port_cfg[i].pvid,usr_context)) != NET_RET_OK) {
                DB_PRINT(DB_ERR,"Set Port %d Vid %d returned fail.\n",port_cfg[i].port,port_cfg[i].pvid);
                return status;
            }
        }
    }
    return NET_RET_OK;
}