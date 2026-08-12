DSDT_ROOT=$(shell pwd)
DSDT_USER_BASE=$(DSDT_ROOT)/switch
DSDT_PROJ_NAME=qdDrv
DSDT_TOOL_DIR=$(DSDT_USER_BASE)/tools
RELEASE=YES
OS_RUN=LINUX
MAD_USER_BASE=$(DSDT_ROOT)/phy
MAD_PROJ_NAME=madDrv
MAD_TOOL_DIR=$(MAD_USER_BASE)/tools
USER_API_NAME=
USER_NAME=
DSP_VCT=FALSE
VCT_PENDING=TRUE
DSDT_USE_MAD=FLASE
DSDT_SWITCH_INCLUDE=$(DSDT_ROOT)/switch/Incluede
DSDT_CROSS_COMPILE=arm-xarh-linux-gnueabi-

export DSDT_ROOT DSDT_USER_BASE DSDT_PROJ_NAME DSDT_TOOL_DIR RELEASE OS_RUN MAD_USER_BASE MAD_PROJ_NAME MAD_TOOL_DIR USER_API_NAME USER_NAME DSP_VCT VCT_PENDING DSDT_USE_MAD DSDT_SWITCH_INCLUDE DSDT_CROSS_COMPILE



MVL_DRV_DIRS := $(DSDT_USER_BASE)/..

MVL_QD_DIR := $(MVL_DRV_DIRS)/switch
MVL_MAD_DIR := $(MVL_DRV_DIRS)/phy

-include $(INCLUDE_DIR)/libs.mk 
DEPENDS+= libxml2 libev libjsonrpcc MCLI

all:DEPEND_LIB   SW PHY netcfg


SW:
	make -C $(MVL_QD_DIR)/src;

PHY:
	make -C $(MVL_MAD_DIR)/src;

netcfg:
	make -C $(MVL_DRV_DIRS)/usrapp/vlan16u

install:all
	cp -f $(MVL_DRV_DIRS)/usrapp/vlan16u/netcfg_dsdt $(ROOTFS_DIR)/usr/bin/
	cp -f $(MVL_DRV_DIRS)/usrapp/vlan16u/sw_cmd $(ROOTFS_DIR)/usr/bin/
	mkdir -p $(ROOTFS_DIR)/etc/netcfg
	cp -f $(MVL_DRV_DIRS)/usrapp/vlan16u/switch_6185_func.json $(ROOTFS_DIR)/etc/netcfg
	#cp -rf $(MVL_DRV_DIRS)/host/linux/resource $(ROOTFS_DIR)/usr/bin/

clean :
	make -C $(MVL_QD_DIR)/src clean
	make -C $(MVL_MAD_DIR)/src clean
	make -C $(MVL_DRV_DIRS)/usrapp/vlan16u clean
