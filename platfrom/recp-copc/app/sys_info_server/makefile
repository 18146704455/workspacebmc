
-include $(INCLUDE_DIR)/libs.mk 
DEPENDS += libev libiniparser 

LIBS= -lev -liniparser 
CFLAGS = -L$(ROOTFS_DEV_DIR)/lib -I$(ROOTFS_DEV_DIR)/include -I.
clean_OBJ = $(wildcard *.o)

#arm-gcc build
SYS_INFO_SERVER=system_info_server

#gcc build
SYS_INFO_CLIENT=system_info_client

PROJECT_BIN+=$(SYS_INFO_SERVER)
$(SYS_INFO_SERVER)_OBJ+= main.o
$(SYS_INFO_SERVER)_OBJ+= rpc_func_core.o
$(SYS_INFO_SERVER)_OBJ+= board_bsp_info.o
$(SYS_INFO_SERVER)_OBJ+= eeprom_base.o gpio.o sys_file.o

define CREATE_PROJ_TARGET 
$1:$$($1_OBJ)
	$$(CC) $$(CFLAGS) $$^ -o $$@  $$(LIBS)
endef

#$(foreach proj,$(ALL_PROJS),$(eval $(call CREATE_PROJ_TARGET,$(proj))))

.c.o:
	$(CC) $(CFLAGS) -c $^ -o $@ $(LIBS)

$(foreach proj,$(PROJECT_BIN),$(eval $(call CREATE_PROJ_TARGET,$(proj))))

all: DEPEND_LIB $(PROJECT_BIN) $(SYS_INFO_CLIENT)

system_info_client: system_info_client.c
	gcc $(CFLAGS) $^ -o $@ 

install: all
	cp -fp $(PROJECT_BIN) $(ROOTFS_DIR)/bin/

#install:

clean:
	-rm -rf $(clean_OBJ)
	-rm -rf $(PROJECT_BIN)
	-rm -rf $(SYS_INFO_CLIENT)
