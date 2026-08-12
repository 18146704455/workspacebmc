
-include $(INCLUDE_DIR)/libs.mk 
DEPENDS+= libev libiniparser libzlog libmosquitto libjsonrpcc

LIBS= -lev -liniparser -lcpld -lmosquitto -ljsonrpcc
CFLAGS = -D_GNU_SOURCE -L$(ROOTFS_DEV_DIR)/lib -I$(ROOTFS_DEV_DIR)/include -I.
clean_OBJ = $(wildcard *.o)

PROJECT_BIN+=rpc_client
rpc_client_OBJ+= main.o
rpc_client_OBJ+= Copc.o
rpc_client_OBJ+= time_check.o 
rpc_client_OBJ+= led_set.o
rpc_client_OBJ+= mqtt_client.o
rpc_client_OBJ+= smi_translayer.o
rpc_client_OBJ+= sys_file.o
rpc_client_OBJ+= gpio.o
rpc_client_OBJ+= fan.o
rpc_client_OBJ+= psu.o
rpc_client_OBJ+= temp.o
rpc_client_OBJ+= board.o
rpc_client_OBJ+= msg.o
rpc_client_OBJ+= udp_call.o
rpc_client_OBJ+= udp_server.o
rpc_client_OBJ+= usock.o
rpc_client_OBJ+= utils.o
rpc_client_OBJ+= dynamical_devs.o
rpc_client_OBJ+= version.o

PROJECT_BIN+=smi_read
smi_read_OBJ+= smi.o
smi_read_OBJ+=smi_translayer.o

define CREATE_PROJ_TARGET 
$1:$$($1_OBJ)
	$$(CC) $$(CFLAGS) $$^ -o $$@  $$(LIBS)
endef

#$(foreach proj,$(ALL_PROJS),$(eval $(call CREATE_PROJ_TARGET,$(proj))))

.c.o:
	$(CC) $(CFLAGS) -c $^ -o $@ $(LIBS)

$(foreach proj,$(PROJECT_BIN),$(eval $(call CREATE_PROJ_TARGET,$(proj))))
all: DEPEND_LIB $(PROJECT_BIN)

install: all
	cp -fp $(PROJECT_BIN) $(ROOTFS_DIR)/bin/
clean:
	-rm -rf $(clean_OBJ)
	-rm -rf $(PROJECT_BIN)
