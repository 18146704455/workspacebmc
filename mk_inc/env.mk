
# job count
CPU_NO        =  $(shell grep -c 'model name' /proc/cpuinfo)

# release version
REL_VERSION   = REL1.0.0
BUILD_TIME=$(shell date  "+\%Y-\%m-\%d \%H:\%M:\%S")
COMMIT_VERSION = $(shell git log -n 1 --pretty=format:"%h%ad" --date=format:'%y%m%d%H%M' )
export CPU_NO REL_VERSION BUILD_TIME COMMIT_VERSION

# cross-compile env
BOARD_DIR	  = $(shell pwd)
TOP_ROOT      =  $(BOARD_DIR)/../..
PATH          := $(PATH):$(TOP_ROOT)/toolchains/arm-gcc-5.5.0/bin/
ARCH          =  arm
CROSS_COMPILE =  arm-xarh-linux-gnueabi-
INCLUDE_DIR=$(TOP_ROOT)/mk_inc/
export TOP_ROOT PATH ARCH CROSS_COMPILE INCLUDE_DIR

MAKE	= make
KERNEL_MAKE= $(MAKE)


APP_CFLAGS= --std=c99 -Wall -Werror -fPIC -I./ -I$(ROOTFS_DEV_DIR)/include
APP_LDFLAGS= -L$(ROOTFS_DIR)/lib -L$(ROOTFS_DEV_DIR)/lib
APP_MAKE = $(MAKE)
APP_MAKE += CC="$(CROSS_COMPILE)gcc"
APP_MAKE += AR="$(CROSS_COMPILE)ar"
APP_MAKE += LD="$(CROSS_COMPILE)ld"
APP_MAKE += STRIP="$(CROSS_COMPILE)strip"
APP_MAKE += APP_CFLAGS="$(APP_CFLAGS)" APP_LDFLAGS="$(APP_LDFLAGS)"
export APP_MAKE





# source code dir
UBOOT_DIR   = $(TOP_ROOT)/bootloader/u-boot-v2018.07/
KERNEL_DIR  = $(TOP_ROOT)/linux/linux-4.18.16/
BOARD_CFG_DIR  = $(BOARD_DIR)/config
export UBOOT_DIR KERNEL_DIR BOARD_CFG_DIR

# target dir
UBOOT_OUTPUT   ?= $(BOARD_DIR)/build/u-boot/
KERNEL_OUTPUT  ?= $(BOARD_DIR)/build/linux/
export UBOOT_OUTPUT KERNEL_OUTPUT

ROOTFS_DEV_DIR = $(BOARD_DIR)/build/rootfs_dev
IMAGE_DIR    = $(BOARD_DIR)/build/image
ROOTFS_DIR   = $(BOARD_DIR)/build/rootfs
export IMAGE_DIR ROOTFS_DIR ROOTFS_DEV_DIR

# utilities
INSTALL      = /usr/bin/install
RM           = rm -f
CP           = cp
export INSTALL RM CP

#SYSROOT_DIR  = $(TOP_ROOT)/toolchains/arm-xarh-linux-gnueabi/sysroot/
BASIC_ROOTFS = $(TOP_ROOT)/sysroot/basic_rootfs/
TRD_APP_DIR  = $(BOARD_DIR)/app/
SYS_APP_DIR += $(TOP_ROOT)/sysroot/app/
SYS_LIBS_DIR += $(TOP_ROOT)/sysroot/libs/
ROOTFS_OVERLAY=$(BOARD_DIR)/rootfs_overlay
export SYSROOT_DIR BASIC_ROOTFS TRD_APP_DIR ROOTFS_OVERLAY SYS_APP_DIR SYS_LIBS_DIR  

# additional configuration

