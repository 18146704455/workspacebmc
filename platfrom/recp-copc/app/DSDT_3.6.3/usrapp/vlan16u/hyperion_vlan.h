/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-08-23 13:02:38
 * @LastEditors: smith.zj
 * @LastEditTime: 2019-10-23 20:04:36
 */
#ifndef __HYPERION_VLAN_H__
#define __HYPERION_VLAN_H__

/*
1、vlan7~vlan10为正常机框通信报文，全部为untag，用于ssh登录调试，tcp通信等
   vlan7:RJ45调试端口，用于从外部调试线卡,仅能访问BMC、cpu、NPS
   vlan8:SCM访问线卡通信，隔离掉RJ45调试端口防止成环，隔离掉NPS debug口，防止NPS IP冲突
   Vlan9：线卡bmc及CPU通信端口，BMC及CPU通信使用
   vlan10:NPS debug通信端口，只允许通过BMC及COMe登录

2、板内通信tag vlan
    vlan 11为tag，线卡内部CPU与NPS通信报文（可以传输协议报文等）
    vlan 12为tag，NPS喂狗专用vlan

3、板间通信 tag vlan
    vlanid = 0xff为广播，可以流通到所有线卡及网板
    vlanid = （0xff-1）为广播，SCM与所有线卡上CPU通信（cp所在CPU）
    vlanid = （0xff-2）为广播，SCM与所有线卡上NPS业务口通信（cp所在CPU）
    vlanid = (0x100+slot*10+CPUID) 为线卡上业务单元（CPU,NPS）与SCM通信点对
    点vlan
4、vlan定义详见hyperion_vlan.h宏
*/

/*===============================线卡板内通信===============================================*/
#define VID_LC_BASE             7

/*debug口通信，从线卡RJ45端口可以访问单板所有器件但不能访问到板外（隔离掉到SCM和CMM端口）*/
#define VID_LC_DEBUG            VID_LC_BASE

/*机框内部通信，从线卡SCM及CMM端口可以访问单板所有器件，除开NPS和debug口（主要用于解决NPS IP地址冲突问题）*/
#define VID_LC_SCM2LC           (VID_LC_BASE + 1) 
#define VID_LC_LC2SCM           (VID_LC_BASE + 2 )

/*NPS debug,用于加载DP及调试NPS使用*/
#define VID_LC_NPS2CPU_DEBUG         (VID_LC_BASE + 3)           


/*===============================SCM板内通信===============================================*/
#define VID_SCM_BASE                    1

/*机框管理及对外通信使用，能访问所有端口及到机框内部通信*/
#define VID_SCM_ALL                     VID_SCM_BASE

/*机框内部通信使用，隔离掉前面板端口*/
#define VID_SCM_SCM2FRAME               VID_SCM_BASE+1

/*前面板端口访问使用，允许访问SCM CPU及BMC，隔离掉其他端口*/
#define VID_SCM_SCM2FRONTPANEL          VID_SCM_BASE+2

/*==============================板间业务通信===========================================*/
#define VID_BRO_BASE                        255
#define VIDUNI_LC_BASE                      256

/*预留用于实现线卡与线卡间的广播及单播通信*/
#define VID_BRO_LC2LC_BASE                      1023
#define VIDUNI_LC2LC_BASE                       1024

#define SLOTTYPE_SCM               0
#define SLOTTYPE_CMM               1
#define SLOTTYPE_LC                2
#define SLOTTYPE_FAB               3

#define SERUNIT_LC_BMC              0
#define SERUNIT_LC_CPU              1
#define SERUNIT_LC_NPS0             2
#define SERUNIT_LC_NPS1             3

#define VID_UNI(base,slot,cpuid)                ((base) + ((slot) * 10 ) + (cpuid))
#define VID_UNI_LC(slot,cpuid)                  VID_UNI(VIDUNI_LC_BASE,slot,cpuid)
#define VID_BRO_LC                              VID_BRO_BASE
#define VID_BRO_FAB                             VID_BRO_BASE
#define VID_BRO_ALL                             VID_BRO_BASE
#define VID_BRO_LC_CPU                          (VID_BRO_BASE-1)
#define VID_BRO_LC_NPS                          (VID_BRO_BASE-2)

#define VID_UNI_LC_BMC(slot)                    VID_UNI_LC(slot,SERUNIT_LC_BMC)
#define VID_UNI_LC_CPU(slot)                    VID_UNI_LC(slot,SERUNIT_LC_CPU)
#define VID_UNI_LC_NPS0(slot)                   VID_UNI_LC(slot,SERUNIT_LC_NPS0)
#define VID_UNI_LC_NPS1(slot)                   VID_UNI_LC(slot,SERUNIT_LC_NPS1)

/*==============================线卡内业务通信使用 目前只有NPS到CPU===========================================*/
#define VID_LC_CPU2NPS_SERV      (VID_LC_BASE+4)                           
#define VID_LC_NPS_WDT           (VID_LC_BASE+5) 

#endif