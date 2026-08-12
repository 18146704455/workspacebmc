/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-08-16 13:52:59
 * @LastEditors: smith.zj
 * @LastEditTime: 2020-03-24 19:44:52
 */
#include <Copyright.h>
#include <string.h>

#include "portable.h"
#include "common_log.h"
#include "netcfg.h"
#include "xmlparser.h"


GT_SYS_CONFIG   cfg;
GT_QD_DEV       diagDev;
GT_QD_DEV       *dev=&diagDev;

GT_STATUS qdStart(int cpuPort,uint32_t smiaddr);
GT_STATUS qdStart(int cpuPort,uint32_t smiaddr) /* devId is used for simulator only */
{
    GT_STATUS status;

    /*
     *  Register all the required functions to QuarterDeck Driver.
    */
    memset((char*)&cfg,0,sizeof(GT_SYS_CONFIG));
    memset((char*)&diagDev,0,sizeof(GT_QD_DEV));

    cfg.BSPFunctions.readMii   = gtBspReadMii;
    cfg.BSPFunctions.writeMii  = gtBspWriteMii;

    cfg.BSPFunctions.semCreate = osSemCreate;
    cfg.BSPFunctions.semDelete = osSemDelete;
    cfg.BSPFunctions.semTake   = osSemWait;
    cfg.BSPFunctions.semGive   = osSemSignal;


    cfg.initPorts = GT_TRUE;    /* Set switch ports to Forwarding mode. If GT_FALSE, use Default Setting. */
    /*cfg.initPorts = GT_FALSE;     Set switch ports to Forwarding mode. If GT_FALSE, use Default Setting. */
    cfg.cpuPortNum = cpuPort;

    cfg.mode.scanMode = SMI_MULTI_ADDR_MODE;    /* Scan 0 or 0x10 base address to find the QD */
    cfg.mode.baseAddr = smiaddr;

    if((status=qdLoadDriver(&cfg, dev)) != GT_OK)
    {
        DB_PRINT(DB_ERR,"qdLoadDriver return Failed %d\n",status);
        return status;
    }

    DB_PRINT(DB_ERR,"Device ID     : 0x%x\n",dev->deviceId);
    DB_PRINT(DB_ERR,"Base Reg Addr : 0x%x\n",dev->baseRegAddr);
    DB_PRINT(DB_ERR,"No of Ports   : %d\n",dev->numOfPorts);
    DB_PRINT(DB_ERR,"CPU Ports     : %d\n",dev->cpuPortNum);

    /*
     *  start the QuarterDeck
    */
    if((status=sysEnable(dev)) != GT_OK) {
        DB_PRINT(DB_ERR,"sysConfig return Failed\n");
        return status;
    }

    return GT_OK;
}
RET_STATUS setPortVlanDot1qMode_6155(void *dev, NET_PORT port, uint8_t qmode,void *usr_context);
RET_STATUS setPortVlanDot1qMode_6155(void *dev, NET_PORT port, uint8_t qmode,void *usr_context) 
{
    RET_STATUS status;
    if((status = gvlnSetPortVlanDot1qMode((GT_QD_DEV *)dev, port, qmode)) != NET_RET_OK) {
        DB_PRINT(DB_ERR,"Set Port %d VlanDot1qMode return Failed\n",port);
        return status;
    } 

	return NET_RET_OK;
}

RET_STATUS setPortVid_6155(void *dev,NET_PORT port,uint16_t pvid,void *usr_context);
RET_STATUS setPortVid_6155(void *dev,NET_PORT port,uint16_t pvid,void *usr_context)
{		
    RET_STATUS status;
    if((status = gvlnSetPortVid((GT_QD_DEV *)dev,port,pvid)) != GT_OK) {
        DB_PRINT(DB_ERR,"Set Port %d Vid %d returned fail.\n",port,pvid);
        return status;
    }

	return NET_RET_OK;
}

RET_STATUS setPortVlanPorts_6155(void *dev, NET_PORT port, NET_PORT *portVectormap, uint16_t num,void *usr_context);
RET_STATUS setPortVlanPorts_6155(void *dev, NET_PORT port, NET_PORT *portVectormap, uint16_t num,void *usr_context)
{
    RET_STATUS status;
    status = gvlnSetPortVlanPorts((GT_QD_DEV *)dev,port,portVectormap,num);
    if(status != NET_RET_OK) {
        DB_PRINT(DB_FATAL,"gvlnSetPortVlanPorts port %d returned fail.",port);
        return status;
    }

	return NET_RET_OK;
}


#define MEMBER_EGRESS_UNMODIFIED        0
#define NOT_A_MEMBER                    1
#define MEMBER_EGRESS_UNTAGGED          2
#define MEMBER_EGRESS_TAGGED            3

static int vlanMemberMAP[4] = {0,2,3,1};
RET_STATUS vlanEntryAdd_6155(void *dev, VTU_ENTRY *vtuEntry,void *usr_context);
RET_STATUS vlanEntryAdd_6155(void *dev, VTU_ENTRY *vtuEntry,void *usr_context)
{
    RET_STATUS status;
    int i;
    GT_VTU_ENTRY DsdtVtuEntry;
    memset(&DsdtVtuEntry, 0, sizeof(GT_VTU_ENTRY));
    DsdtVtuEntry.DBNum = vtuEntry->fid;
    DsdtVtuEntry.sid = vtuEntry->sid;
    DsdtVtuEntry.vid = vtuEntry->vid;
    DsdtVtuEntry.vidPolicy = vtuEntry->vidPolicy;
    DsdtVtuEntry.vidPolicy = vtuEntry->vidPolicy;
    DsdtVtuEntry.vidExInfo.useVIDFPri = vtuEntry->vidExInfo.useVIDFPri;
    DsdtVtuEntry.vidExInfo.useVIDQPri = vtuEntry->vidExInfo.vidFPri;
    DsdtVtuEntry.vidExInfo.vidFPri = vtuEntry->vidExInfo.vidFPri;
    DsdtVtuEntry.vidExInfo.vidNRateLimit = 0;
    DsdtVtuEntry.vidExInfo.vidQPri = vtuEntry->vidExInfo.vidQPri;    

    for(i = 0; i < 10;i++) {
        /*DSDT and UMSD have different vlan member tag discribe we should have a translter*/
        DsdtVtuEntry.vtuData.memberTagP[i] = vlanMemberMAP[vtuEntry->memberTagP[i]];
    }


    if((status = gvtuAddEntry(dev,&DsdtVtuEntry)) != GT_OK)
    {
        DB_PRINT(DB_ERR,"gvtuAddEntry returned fail.\n");
        return status;
    }
	return NET_RET_OK;
}

void dump_vlan_entry(GT_VTU_ENTRY *pvtuEntry, GT_U8 numOfPorts);
void dump_vlan_entry(GT_VTU_ENTRY *pvtuEntry, GT_U8 numOfPorts)
{
    int portIndex;
    GT_LPORT port;  
    PRINT("FID:%i, VID:%i SID:%i vidPriOverride:%s vidPriority:%u filter %u vidPolicy:%s\n",\
            pvtuEntry->DBNum,pvtuEntry->vid, pvtuEntry->sid, pvtuEntry->vidPriOverride?"true":"false", pvtuEntry->vidPriority,\
                 pvtuEntry->filter, pvtuEntry->vidPolicy?"true":"false");
    PRINT("EXT_INFO: useVIDFPri:%s vidFPri:%u useVIDQPri:%s vidQPri:%u vidNRateLimit:%s\n", pvtuEntry->vidExInfo.useVIDFPri ?"true":"false",\
        pvtuEntry->vidExInfo.vidFPri, pvtuEntry->vidExInfo.useVIDQPri?"true":"false", pvtuEntry->vidExInfo.vidQPri,\
             pvtuEntry->vidExInfo.vidNRateLimit?"true":"false");
    for(portIndex=0; portIndex < numOfPorts; portIndex++) {
        port = portIndex;
        PRINT("Tag%i:%#x  ",port,pvtuEntry->vtuData.memberTagP[port]);
    }
    PRINT("\n");
}

GT_STATUS sampleDisplayVIDTable(GT_QD_DEV *dev)
{
    GT_STATUS status;
    GT_VTU_ENTRY vtuEntry;
    GT_LPORT port;    
    

    gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
    vtuEntry.vid = 0;
    if((status = gvtuGetEntryFirst(dev,&vtuEntry)) != GT_OK) {
        DB_PRINT(DB_ERR,"gvtuGetEntryCount returned fail.\n");
        return status;
    }
    dump_vlan_entry(&vtuEntry, dev->numOfPorts);
    
    while(1)  {
        status = gvtuGetEntryNext(dev,&vtuEntry);
        if(GT_FAIL == status) {
            DB_PRINT(DB_ERR,"gvtuGetEntryCount returned fail.\n");
            break;
        } else if(GT_NO_SUCH == status) {
            DB_PRINT(DB_ERR,"interator vlan entry finish\n");
            break;
        }
        dump_vlan_entry(&vtuEntry, dev->numOfPorts);
    }
    return GT_OK;
}

void dump_port_counter(GT_LPORT        port, GT_STATS_COUNTER_SET3    *statsCounterSet)
{
    printf("port:%u\n", port);
    printf("InGoodOctets  %lu InBadOctets %u InUnicasts %u InBroadcasts %u InMulticasts %u InFCSErr %u\n",statsCounterSet->InGoodOctetsLo | statsCounterSet->InGoodOctetsHi << 32,
        statsCounterSet->InUnicasts, statsCounterSet->InBroadcasts, statsCounterSet->InMulticasts, statsCounterSet->InFCSErr);
    printf("OutOctets  %lu InBadOctets %u OutUnicasts %u OutBroadcasts %u OutMulticasts %u OutFCSErr %u\n",statsCounterSet->OutOctetsLo | statsCounterSet->OutOctetsHi << 32,
        statsCounterSet->OutUnicasts, statsCounterSet->OutBroadcasts, statsCounterSet->OutMulticasts, statsCounterSet->OutFCSErr);
}

#if 0

void dump_port_counter(GT_LPORT        port, GT_STATS_COUNTER_SET3    *statsCounterSet)
{
    printf("port:%u\n", port);
    printf("InUnicasts  %4u InBroadcasts %4u InPause     %4u InMulticasts %4u InFCSErr     %4u\n",
            statsCounterSet->InUnicasts, statsCounterSet->InBroadcasts,statsCounterSet->InPause, statsCounterSet->InMulticasts, statsCounterSet->InFCSErr);
    printf("AlignErr    %4u InBadOctets  %4u Undersize   %4u Fragments    %4u Deferred     %4u\n",\
            statsCounterSet->AlignErr, statsCounterSet->InBadOctets,statsCounterSet->Undersize,statsCounterSet->Fragments, statsCounterSet->Deferred);
    printf("In64Octets  %4u In127Octets  %4u In255Octets %4u In511Octets  %4u In1023Octets %4u\n",  \
            statsCounterSet->In64Octets, statsCounterSet->In127Octets, statsCounterSet->In255Octets, statsCounterSet->In511Octets,statsCounterSet->In1023Octets);
    printf("InMaxOctets %4u Jabber       %4u InDiscards  %4u Filtered     %4u OutUnicasts  %4u\n",\
            statsCounterSet->InMaxOctets, statsCounterSet->Jabber, statsCounterSet->InDiscards, statsCounterSet->Filtered,statsCounterSet->OutUnicasts);
    printf("OutBroadcasts %4u OutPause   %4u OutMulticasts  %4u OutFCSErr %4u OutGoodOctets %4u\n",\
            statsCounterSet->OutBroadcasts, statsCounterSet->OutPause, statsCounterSet->OutMulticasts,statsCounterSet->OutFCSErr, statsCounterSet->OutGoodOctets);
    printf("Out64Octets   %4u Out127Octets %4u Out255Octets %4u Out511Octets %4u Out1023Octets %4u\n", \
            statsCounterSet->Out64Octets, statsCounterSet->Out127Octets, statsCounterSet->Out255Octets, statsCounterSet->Out511Octets, statsCounterSet->Out1023Octets);
    printf("OutMaxOctets  %4u Collisions   %4u Excessive    %4u Multiple     %4u Single %4uOutDiscards %4u\n",\
            statsCounterSet->OutMaxOctets, statsCounterSet->Collisions, statsCounterSet->Excessive, statsCounterSet->Multiple, statsCounterSet->Single, \
            statsCounterSet->OutDiscards);
}
#endif

int main(int argc,char *argv[])
{
    int tmp_v=0;
    RET_STATUS status;
    uint16_t smiaddr = 0;
	net_port_cfg_s *port_cfg;
	uint16_t port_cfg_len;
	net_service_vlan_cfg_s *vlan_cfg;
	uint16_t vlan_cfg_len;
    GT_STATS_COUNTER_SET3 counter;
    GT_PORT_STAT port_statics;

    uint8_t gslot = 0;
    if(argc != 2) {
        DB_PRINT(DB_ERR, "USAGE:%s slot_num\n",argv[0]);
        return -1;
    }

    sscanf(argv[1]," %i",&tmp_v);
    gslot = (uint8_t )tmp_v;
    DB_PRINT(DB_INFO,"slot is [%d]\n",gslot);

    status = load_netdescriptor("lc02",&smiaddr,&port_cfg,&port_cfg_len,&vlan_cfg,&vlan_cfg_len);
	if(status != NET_RET_OK) {
		DB_PRINT(DB_ERR,"ERROR:load net descriptor from xml file failed\n");
		return -1;
	}

    DB_PRINT(DB_CURR_LEVEL,"DEV  smi BUS %d ADDR %#x\n", 1, smiaddr);


    if(qdStart(3,smiaddr) != GT_OK) {
        DB_PRINT(DB_CURR_LEVEL,"load driver failed\n");
        return -1;
    }


    memset(&counter, 0, sizeof(GT_STATS_COUNTER_SET));
    status =gstatsGetPortAllCounters3(dev, gslot, &counter);
    if(status != GT_OK){
        DB_PRINT(DB_CURR_LEVEL,"gstatsGetPortAllCounters3 failed %d\n",status);
    }
    dump_port_counter(gslot, &counter);

    return 0;
}