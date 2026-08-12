# makefile - build main object file
#
# modification history
# --------------------
# 04-15-02,mj	created
#
#######################################################################
CROSS_COMILE=$(DSDT_CROSS_COMPILE)

CC=$(CROSS_COMILE)gcc
LD=$(CROSS_COMILE)ld
export CC LD

# Include common variable definitions
-include $(DSDT_TOOL_DIR)/makelnx.defs

QDLIB = $(LIB_DIR)/$(DSDT_PROJ_NAME).o
OBJDIRS	= $(subst /,\,$(dir $(QDLIB)))
CSOURCES = osSem.c bmcmii.c main.c main_cmd_line.c  netcfg.c xmlparser.c

EXTRA_INCLUDE = -I$(DSDT_SWITCH_INCLUDE)
COBJECTS  	= $(CSOURCES:.c=.o)
AOBJECTS  	= $(ASOURCES:.s=.o)
LIBS=-lpthread -lxml2 -lmcli -lcli
ifeq ($(OBJECTS),)
OBJECTS  	= $(COBJECTS) $(AOBJECTS)
endif

#CFLAGS+=-L/home/zj/bmc/scm_bmc/xarhbmc/platform/recp-lc02/build/rootfs_dev/lib -I/home/zj/bmc/scm_bmc/xarhbmc/platform/recp-lc02/build/rootfs_dev/include
CFLAGS+=-Wall  -std=gnu99 -L$(ROOTFS_DEV_DIR)/lib/ -I$(ROOTFS_DEV_DIR)/include/libxml2 -I$(ROOTFS_DEV_DIR)/include

all:netcfg_dsdt sw_cmd 

sw_cmd:$(OBJECTS)  $(QDLIB)
	echo $(OBJECTS)
	$(CC) $(LIBS) $(filter-out %main.o, $(OBJECTS)) $(CFLAGS) $(QDLIB) -o $@ 

OBJECTS_MS= $(subst /,\,$(OBJECTS))
netcfg_dsdt: $(OBJECTS) $(QDLIB)
	echo $(OBJECTS)
	$(CC) $(LIBS) $(filter-out %main_cmd_line.o, $(OBJECTS)) $(CFLAGS) $(QDLIB) -o $@ 

$(OBJECTS) : %.o : %.c
.c.o :
	$(CC) $(CFLAGS) $(EXTRA_DEFINE) $(EXTRA_INCLUDE) $(ADDED_CFLAGS) -c $< -o $@

lib:
	make -C ../../switch/src/

.PHONY : clean
clean :
	-make -C ../../switch/src/ clean
	-rm -f *.o
	-rm -f sw
	-rm -f netcfg_dsdt sw_cmd

#end of file
