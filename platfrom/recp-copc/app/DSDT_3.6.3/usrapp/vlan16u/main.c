/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-08-16 13:52:59
 * @LastEditors: smith.zj
 * @LastEditTime: 2021-01-05 16:46:25
 */
#include <Copyright.h>
#include <string.h>

#include "portable.h"
#include "common_log.h"
#include "netcfg.h"
#include "xmlparser.h"
#include "apiCLI.h"


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


    cfg.initPorts = GT_FALSE;    /* Set switch ports to Forwarding mode. If GT_FALSE, use Default Setting. */
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

bool checkPortVlanDot1qMode_6155(void *dev, NET_PORT port, uint8_t qmode);
bool checkPortVlanDot1qMode_6155(void *dev, NET_PORT port, uint8_t qmode)
{
    uint8_t cur_mode = 0;
    RET_STATUS status;
    status = gvlnGetPortVlanDot1qMode(dev,port, &cur_mode);
    if(status != GT_OK) {
        DB_PRINT(DB_ERR, "GetPort %d VlanDot1qMode failed\n", port);
        return false;
    }
    return qmode ==  cur_mode;
}

RET_STATUS setPortVlanDot1qMode_6155(void *dev, NET_PORT port, uint8_t qmode,void *usr_context);
RET_STATUS setPortVlanDot1qMode_6155(void *dev, NET_PORT port, uint8_t qmode,void *usr_context) 
{
    RET_STATUS status;
    GT_UNUSED_PARAM(usr_context);
    if((status = gvlnSetPortVlanDot1qMode((GT_QD_DEV *)dev, port, qmode)) != NET_RET_OK) {
        DB_PRINT(DB_ERR,"Set Port %d VlanDot1qMode return Failed\n",port);
        return status;
    } 

    if(!checkPortVlanDot1qMode_6155(dev, port, qmode)) {
        DB_PRINT(DB_ERR, "check port %d qmode %d NOT PASSED\n", port, qmode);
        return GT_FAIL;
    } else {
        DB_PRINT(DB_INFO, "check port %d qmode %d PASSED\n", port, qmode);      
    }

	return NET_RET_OK;
}

bool checkPortVid(void *dev,NET_PORT port,uint16_t pvid);
bool checkPortVid(void *dev,NET_PORT port,uint16_t pvid)
{
    uint8_t cur_pvid = 0;
    RET_STATUS status;
    status = gvlnGetPortVid(dev,port, &cur_pvid);
    if(status != GT_OK) {
        DB_PRINT(DB_ERR, "GetPort %d PortVid failed\n", port);
        return false;
    }
    return pvid ==  cur_pvid;
}


RET_STATUS setPortVid_6155(void *dev,NET_PORT port,uint16_t pvid,void *usr_context);
RET_STATUS setPortVid_6155(void *dev,NET_PORT port,uint16_t pvid,void *usr_context)
{		
    RET_STATUS status;
    if((status = gvlnSetPortVid((GT_QD_DEV *)dev,port,pvid)) != GT_OK) {
        DB_PRINT(DB_ERR,"Set Port %d Vid %d returned fail.\n",port,pvid);
        return status;
    }

    if(!checkPortVid(dev, port, pvid)) {
        DB_PRINT(DB_ERR, "check port %d pvid %d NOT PASSED\n", port,pvid);
        return GT_FAIL;
    } else {
        DB_PRINT(DB_INFO, "check port %d pvid %d PASSED\n", port,pvid);      
    }


	return NET_RET_OK;
}

bool checkPortVlanPorts(void *dev, NET_PORT port, NET_PORT *portVectormap, uint16_t num);
bool checkPortVlanPorts(void *dev, NET_PORT port, NET_PORT *portVectormap, uint16_t num)
{
    NET_PORT memPorts[11] = {0};
    GT_U8    memPortsLen;
    RET_STATUS status;
    int i;
    bool result = false;
    status = gvlnGetPortVlanPorts(dev,port, memPorts, &memPortsLen);
    if(status != GT_OK) {
        DB_PRINT(DB_ERR, "GetPort %d portbase vlan failed\n", port);
        return false;
    }
    result = (num == memPortsLen);

    for( i = 0;i < memPortsLen;i++) {
        result |= (portVectormap[i] == memPorts[i]);
    }

    return result;     
}

RET_STATUS setPortVlanPorts_6155(void *dev, NET_PORT port, NET_PORT *portVectormap, uint16_t num,void *usr_context);
RET_STATUS setPortVlanPorts_6155(void *dev, NET_PORT port, NET_PORT *portVectormap, uint16_t num,void *usr_context)
{
    RET_STATUS status;
    status = gvlnSetPortVlanPorts((GT_QD_DEV *)dev,port,portVectormap,num);
    if (status != NET_RET_OK) {
        DB_PRINT(DB_FATAL,"gvlnSetPortVlanPorts port %d returned fail.",port);
        return status;
    }

    if ( !checkPortVlanPorts(dev, port, portVectormap, num) ) {
        DB_PRINT(DB_ERR, "check port %d port base vlan NOT PASSED\n", port);
        return GT_FAIL;
    } else {
        DB_PRINT(DB_INFO, "check port %d port base vlan PASSED\n", port);      
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
        DsdtVtuEntry.vtuData.portStateP[i] = 3;
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


GT_STATUS enable_all_port()
{
    int port;
    GT_STATUS retVal;
    DB_PRINT(DB_ERR,"interator vlan entry finish\n");
    for (port=0; port<dev->numOfPorts; port++) {
        if((retVal = gstpSetPortState(dev,port,GT_PORT_FORWARDING)) != GT_OK) {
            DB_PRINT(DB_ERR,"enable port %d to FORWARDING state failed\n");
            return retVal;
        }
    }

    return GT_OK;
}


/*************************************cli*******************************************/
/*---------------------------atu--------------------------------*/
int atu_dump(int argc,char *argv[]);
int atu_dump(int argc,char *argv[])
{
    GT_STATUS status;
    GT_ATU_ENTRY    atuEntry;
    
    if(argc != 3) {
        return NET_RET_BAD_PARAM;
    }
    atuEntry.DBNum = (uint8_t)strtoul(argv[2], NULL, 0);

    CLI_INFO("\n------------------FID %u----------------------------------\n", atuEntry.DBNum);
    CLI_INFO("FID    MAC           LAG  PORTVEC      STATE  FPRI\n");
    CLI_INFO("----------------------------------------------------\n");
    status = gfdbGetAtuEntryFirst(dev, &atuEntry);
    if(status == GT_NO_SUCH) {
        CLI_INFO("-----------empty-----------------\n", status);
        return NET_RET_OK;
    }

    if(status != GT_OK) {
        CLI_INFO("dump failed %d\n", status);
        return NET_RET_FAIL;
    }
    CLI_INFO("0x%-5x%02x%02x%02x%02x%02x%02x  %-5x%x%x%x%x%x%x%x%x%x%x%x  %-7x%-6x\n", \
        atuEntry.DBNum, atuEntry.macAddr.arEther[0], atuEntry.macAddr.arEther[1], atuEntry.macAddr.arEther[2],
        atuEntry.macAddr.arEther[3], atuEntry.macAddr.arEther[4], atuEntry.macAddr.arEther[5], atuEntry.trunkMember,
        ((atuEntry.portVec & 0x400) ? 1 : 0), ((atuEntry.portVec & 0x200) ? 1 : 0), ((atuEntry.portVec & 0x100) ? 1 : 0),
        ((atuEntry.portVec & 0x80) ? 1 : 0), ((atuEntry.portVec & 0x40) ? 1 : 0), ((atuEntry.portVec & 0x20) ? 1 : 0),
        ((atuEntry.portVec & 0x10) ? 1 : 0), ((atuEntry.portVec & 0x8) ? 1 : 0), ((atuEntry.portVec & 0x4) ? 1 : 0),
        ((atuEntry.portVec & 0x2) ? 1 : 0), ((atuEntry.portVec & 0x1) ? 1 : 0),
        atuEntry.entryState.ucEntryState, atuEntry.exPrio.macFPri);

    while(1) {
        status = gfdbGetAtuEntryNext(dev, &atuEntry);
        if (GT_NO_SUCH == status)
            break;
        
        if(status != GT_OK) {
            CLI_INFO("dump failed %d\n", status);
            return NET_RET_FAIL;
        }
        CLI_INFO("0x%-5x%02x%02x%02x%02x%02x%02x  %-5x%x%x%x%x%x%x%x%x%x%x%x  %-7x%-6x\n", \
            atuEntry.DBNum, atuEntry.macAddr.arEther[0], atuEntry.macAddr.arEther[1], atuEntry.macAddr.arEther[2],
            atuEntry.macAddr.arEther[3], atuEntry.macAddr.arEther[4], atuEntry.macAddr.arEther[5], atuEntry.trunkMember,
            ((atuEntry.portVec & 0x400) ? 1 : 0), ((atuEntry.portVec & 0x200) ? 1 : 0), ((atuEntry.portVec & 0x100) ? 1 : 0),
            ((atuEntry.portVec & 0x80) ? 1 : 0), ((atuEntry.portVec & 0x40) ? 1 : 0), ((atuEntry.portVec & 0x20) ? 1 : 0),
            ((atuEntry.portVec & 0x10) ? 1 : 0), ((atuEntry.portVec & 0x8) ? 1 : 0), ((atuEntry.portVec & 0x4) ? 1 : 0),
            ((atuEntry.portVec & 0x2) ? 1 : 0), ((atuEntry.portVec & 0x1) ? 1 : 0),
            atuEntry.entryState.ucEntryState, atuEntry.exPrio.macFPri);
        
        if (GT_IS_BROADCAST_MAC(atuEntry.macAddr)) {
            break;
        }
    }

    CLI_INFO("\n");
    return NET_RET_OK;
}

/*---------------------------port--------------------------------*/
char *speed_str(GT_PORT_SPEED_MODE speed)
{
 switch(speed){
 case PORT_SPEED_10_MBPS:
     return "PORT_SPEED_10_MBPS";
     break;
 case PORT_SPEED_100_MBPS:
    return "PORT_SPEED_100_MBPS";
    break;
 case PORT_SPEED_1000_MBPS:
    return "PORT_SPEED_1000_MBPS";
    break;
 case PORT_SPEED_10_GBPS:
    return "PORT_SPEED_10_GBPS";
    break;
 default:
    return "PORT_SPEED_UNKNOWN";
 }
}

int portGetStatus(int argc,char *argv[])
{
    uint8_t port;
    GT_STATUS status;
    GT_BOOL linkOn = GT_FALSE;
    GT_BOOL duplex = GT_FALSE;
    GT_PORT_SPEED_MODE   speed;

    if(argc != 3) {
        return NET_RET_BAD_PARAM;
    }
    port = (uint8_t)strtoul(argv[2], NULL, 0);
    status = gprtGetLinkState(dev, port, &linkOn);
    if(status != GT_OK) {
        CLI_INFO("gprtGetLinkState failed\n");
        return NET_RET_FAIL;
    }

    status = gprtGetDuplex(dev, port, &duplex);
    if(status != GT_OK) {
        CLI_INFO("gprtGetDuplex failed\n");
        return NET_RET_FAIL;
    }

    status = gprtGetSpeedMode(dev, port, &speed);
    if(status != GT_OK) {
        CLI_INFO("gprtGetSpeedMode failed\n");
        return NET_RET_FAIL;
    }
    CLI_INFO("link    duplex   speed\n");
    CLI_INFO("%s  %s  %s\n",linkOn?"true":"false", duplex?"true":"false", speed_str(speed));

    return NET_RET_OK;
}


int setPortAn(int argc,char *argv[])
{
    uint8_t port;
    GT_STATUS status;
    GT_BOOL an = GT_FALSE;


    if(argc != 4) {
        return NET_RET_BAD_PARAM;
    }
    port = (uint8_t)strtoul(argv[2], NULL, 0);
    an = (GT_BOOL)strtoul(argv[3], NULL, 0);
    status = gpcsSetAnBypassMode(dev, port, an);
    if(status != GT_OK) {
        CLI_INFO("set port:%u an:%u failed\n",port, an);
        return NET_RET_FAIL;
    }
    CLI_INFO("set port:%u an:%u success\n",port, an);

    return NET_RET_OK;
}

int getPortAn(int argc,char *argv[])
{
    uint8_t port;
    GT_STATUS status;
    GT_BOOL an = GT_FALSE;


    if(argc != 3) {
        return NET_RET_BAD_PARAM;
    }
    port = (uint8_t)strtoul(argv[2], NULL, 0);
    status = gpcsGetAnBypassMode(dev, port, &an);
    if(status != GT_OK) {
        CLI_INFO("get port:%u an:%u failed\n",port);
        return NET_RET_FAIL;
    }
    CLI_INFO("port:%u an:%s\n",port, an==GT_TRUE?"enable":"false");

    return NET_RET_OK;
}

/*---------------------------mib--------------------------------*/
void dump_port_counter(GT_LPORT        port, GT_STATS_COUNTER_SET3    *statsCounterSet)
{
    CLI_INFO("port:%u\n", port);
    CLI_INFO("InGoodOctets  %lu  InUnicasts %u InBroadcasts %u InMulticasts %u InBadOctets %u InFCSErr %u\n",\
    statsCounterSet->InGoodOctetsLo | statsCounterSet->InGoodOctetsHi << 32, statsCounterSet->InUnicasts,\
    statsCounterSet->InBroadcasts, statsCounterSet->InMulticasts, statsCounterSet->InBadOctets, statsCounterSet->InFCSErr);
    CLI_INFO("OutOctets  %lu OutBadOctets %u OutUnicasts %u OutBroadcasts %u OutMulticasts %u OutFCSErr %u\n",statsCounterSet->OutOctetsLo | statsCounterSet->OutOctetsHi << 32,
        statsCounterSet->OutUnicasts, statsCounterSet->OutBroadcasts, statsCounterSet->OutMulticasts, statsCounterSet->OutFCSErr);
}

int getPortAllCounter(int argc,char *argv[]);
int getPortAllCounter(int argc,char *argv[])
{
    uint8_t port;
    GT_STATUS status;
    GT_STATS_COUNTER_SET3 counter;

    if(argc != 3) {
        return NET_RET_BAD_PARAM;
    }
    port = (uint8_t)strtoul(argv[2], NULL, 0);
    memset(&counter, 0, sizeof(GT_STATS_COUNTER_SET3));
    status =gstatsGetPortAllCounters3(dev, port, &counter);
    if(status != GT_OK){
        DB_PRINT(DB_CURR_LEVEL,"gstatsGetPortAllCounters3 failed %d\n",status);
    }

    dump_port_counter(port, &counter);
    return NET_RET_OK;
}

int port_counter_flush(int argc,char *argv[]);
int port_counter_flush(int argc,char *argv[])
{
    uint8_t port;
    GT_STATUS status;

    if (argc != 3) {
        return NET_RET_BAD_PARAM;
    }
    
    port = (uint8_t)strtoul(argv[2], NULL, 0);
    status =gstatsFlushPort(dev, port);
    if ( status != GT_OK ) {
        DB_PRINT(DB_CURR_LEVEL,"flush port %d failed %d\n",port, status);
    }

    return NET_RET_OK;
}

int flushAll_counter( int argc,char *argv[] );
int flushAll_counter( int argc,char *argv[] )
{
    GT_STATUS status;
    if ( argc != 2 ) {
        return NET_RET_BAD_PARAM;
    }

    status =gstatsFlushAll(dev);
    if ( status != GT_OK ) {
        DB_PRINT(DB_CURR_LEVEL,"flush failed %d\n",status);
    }

    return NET_RET_OK;
}

/*---------------------------vlan------------------------------------------*/

    GT_BOOL        vidPriOverride;
    GT_U8          vidPriority;
    GT_U8          ;      
    GT_VTU_EXT_INFO    vidExInfo;

void cli_dump_vlan_entry(GT_VTU_ENTRY *entry)
{
    CLI_INFO("0x%-5x0x%-5x0x%-5x%x %x %x %x %x %x %x %x %x %x %x   %-3x%--8x%-5x%-8x%-5x%-10x%-9x\n", \
            entry->DBNum, entry->vid , entry->sid,
			entry->vtuData.memberTagP[0], entry->vtuData.memberTagP[1], entry->vtuData.memberTagP[2], entry->vtuData.memberTagP[3],\
             entry->vtuData.memberTagP[4], entry->vtuData.memberTagP[5],
			entry->vtuData.memberTagP[6], entry->vtuData.memberTagP[7], entry->vtuData.memberTagP[8], entry->vtuData.memberTagP[9], \
            entry->vtuData.memberTagP[10], entry->vidPolicy, 
			entry->vidExInfo.useVIDFPri, entry->vidExInfo.vidFPri, entry->vidExInfo.useVIDQPri, entry->vidExInfo.vidQPri,
			entry->filter);
}

int vlan_dump(int argc,char *argv[]);
int vlan_dump(int argc,char *argv[])
{
    GT_STATUS status;
    GT_VTU_ENTRY vtuEntry;

    if ( argc != 2 ) {
        return NET_RET_BAD_PARAM;
    }  

	CLI_INFO("\n-------------------------------------------------------------------------------------------------------------\n");
	CLI_INFO("FID    VID    SID    MEMBERTAGP              VP USEFPRI FPRI USEQPRI QPRI FILTER\n");
	CLI_INFO("------------------------------------------------------------------------------------------------------------\n");

    gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
    vtuEntry.vid = 0;
    if((status = gvtuGetEntryFirst(dev,&vtuEntry)) != GT_OK) {
        DB_PRINT(DB_ERR,"gvtuGetEntryCount returned fail.\n");
        return status;
    }
    cli_dump_vlan_entry(&vtuEntry);
    
    while(1)  {
        status = gvtuGetEntryNext(dev,&vtuEntry);
        if(GT_FAIL == status) {
            DB_PRINT(DB_ERR,"gvtuGetEntryCount returned fail.\n");
            break;
        } else if(GT_NO_SUCH == status) {
            DB_PRINT(DB_ERR,"interator vlan entry finish\n");
            break;
        }
        cli_dump_vlan_entry(&vtuEntry);
    }

    return NET_RET_OK;
}


CLI_COMMAND_STRUCT portCMDList[] =
{
    {"getPortStatus",portGetStatus},
    {"set_an", setPortAn},
    {"get_an",getPortAn},
    { "", NULL },
};

CLI_COMMAND_STRUCT atuCMDList[] =
{
  #if 0
    { "findEntry", atu_findEntry },
    { "getNext", atu_getNext },
    #endif
    {"dump",atu_dump},
    { "", NULL },
};

CLI_COMMAND_STRUCT vlanCMDList[] =
{
    {"dump",vlan_dump},
    { "", NULL },
};

CLI_COMMAND_STRUCT mibCMDList[] =
{
    {"flush",flushAll_counter},
    {"flushport",port_counter_flush},
    {"getPortAllCounter",getPortAllCounter},
    { "", NULL },
};


EXTERN_CLI_STRUCT externCLICMDList[] =
{
    {"port", portCMDList},
    {"atu",  atuCMDList},
    {"vlan", vlanCMDList},
    {"mib", mibCMDList},
    { NULL, NULL },
};

CLI_COMMAND_STRUCT directCMDList[] =
{
    { "", NULL },
};
/********************************************************************************/

static int init_switch = 1;
static int gslot = -1;
static int cli = 0;
static char board_string[64]= {0};
static int32_t parse_arguments(int argc, char **argv)
{
   int idx, rc, buidx = 0;

   for(idx=0;idx<argc;idx++) {
      printf("arg %u %s\n",idx,argv[idx]);
   }

   for (idx = 1; idx < argc;)
   {
      if (argv[idx][0] == '-') {
         if (strcmp(argv[idx], "-no_init") == 0) {
            init_switch = 0;
            printf("init_switch is %d\n",init_switch);
            idx += 1;
         } else if (strcmp(argv[idx], "-slot") == 0) {
            rc = sscanf(argv[idx + 1], "%x", &gslot);
            if (rc < 1) {
               printf("Error: expecting number as slot\n");
               return -1;
            }
            idx += 2;
         } else if (strcmp(argv[idx], "-board") == 0) {
            strcpy(board_string, argv[idx + 1]);
            idx += 2;
         } else if (strcmp(argv[idx], "-cli") == 0) {
            cli = 1;
            idx += 1;
         }  
      } else {
         printf("Error: An unrecognized argument was found %s\n",
               argv[idx]);
         idx++;
      }
   }

   return 0;
}


void usage(char *name)
{
    DB_PRINT(DB_ERR, "USAGE:%s -board board_string -slot slot [-no_init] [-cli]\n", name);
}

int main(int argc,char *argv[])
{
    RET_STATUS status;
    uint16_t smiaddr = 0;
	net_port_cfg_s *port_cfg;
	uint16_t port_cfg_len;
	net_service_vlan_cfg_s *vlan_cfg;
	uint16_t vlan_cfg_len;

    /*省的改lc02代码*/
    strcpy(board_string, "lc02");
    if(parse_arguments(argc, argv) != 0) {
        usage(argv[0]);
        return -1;
    }
    DB_PRINT(DB_INFO,"board:%s slot:%d init:%d$$$$$$$$$\n", board_string,gslot, init_switch);
    if(gslot == -1) {
        usage(argv[0]);
        return -1;  
    }

    status = load_netdescriptor(board_string, &smiaddr,&port_cfg,&port_cfg_len,&vlan_cfg,&vlan_cfg_len);
	if(status != NET_RET_OK) {
		DB_PRINT(DB_ERR,"ERROR:load net descriptor from xml file failed\n");
		return -1;
	}

    register_netctrl_func(vlanEntryAdd_6155,setPortVlanPorts_6155,setPortVlanDot1qMode_6155,setPortVid_6155);

    DB_PRINT(DB_CURR_LEVEL,"DEV  smi BUS %d ADDR %#x\n", 1, smiaddr);
	dump_port_cfg_array(port_cfg,port_cfg_len);
	dump_vlan_entry_array(vlan_cfg,vlan_cfg_len);

    if(qdStart(3,smiaddr) != GT_OK) {
        DB_PRINT(DB_CURR_LEVEL,"load driver failed\n");
        return -1;
    }

    if(init_switch) {
    /*   sample802_1qSetup(dev);*/
    /*   sampleDisplayVIDTable(dev);*/
        DB_PRINT(DB_INFO,"config switch port\n");
        status = cfgNetPort((void *)dev,port_cfg, port_cfg_len, (void *)&gslot);
        if (status != NET_RET_OK) {
            DB_PRINT(DB_ERR,"FATAL:config switch port failed\n");
            return -1;
        } else {
        DB_PRINT(DB_INFO,"config switch port success!!\n"); 
        }

        DB_PRINT(DB_INFO,"config switch 8021q vlan\n");
        status = set_vlan_entry((void *)dev, vlan_cfg, vlan_cfg_len,(void *)&gslot);
        if(status != NET_RET_OK) {
            DB_PRINT(DB_ERR,"config switch 8021q vlan failed!!\n");
            return -1;
        } else {
            DB_PRINT(DB_INFO,"config switch 8021q vlan success\n");
        } 

        /*sampleDisplayVIDTable(dev);*/
        if(enable_all_port() != GT_OK) {
            DB_PRINT(DB_ERR,"enable all port failed\n");
            return -1;
        }
    }

    if(cli == 1) {
        registerClicmd(externCLICMDList);
        registerDirectClicmd(directCMDList);
        if(cliCommand(6185, "/etc/netcfg/switch_6185_func.json") != 0) {
            DB_PRINT(DB_ERR,"enable all port failed\n");
        }
    }
    return 0;
}
