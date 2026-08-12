
BOARD_SENSOR_RPC_SERVER_BIN = board_rpc_server
BOARD_VERSION_BIN = version
SRC = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRC))
-include $(INCLUDE_DIR)/libs.mk 


CFLAGS += -g -L$(ROOTFS_DEV_DIR)/lib 
LIBS = $(LDFLAGS) -lev -lpthread -ljsonrpcc -liniparser -lzlog -lsmbus -lcpld
DEPENDS+= libev libjsonrpcc libiniparser libzlog libsmbus #board_libcpld
BOARD_SENSOR_RPC_SERVER_OBJS= sensor_rpc_server.o sensor.o version.o time_check.o led_set.o board_ctrl.o 
all: DEPEND_LIB $(BOARD_SENSOR_RPC_SERVER_BIN)  $(FCB_API_BIN)  $(BOARD_VERSION_BIN)


$(BOARD_SENSOR_RPC_SERVER_BIN): $(BOARD_SENSOR_RPC_SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)


$(BOARD_VERSION_BIN):version.o debug_version.o  
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS)


install:all 
	cp -af $(BOARD_SENSOR_RPC_SERVER_BIN) $(ROOTFS_DIR)/usr/bin/
	cp -af $(BOARD_VERSION_BIN) $(ROOTFS_DIR)/usr/bin/
	test -d $(ROOTFS_DIR)/etc/sysconfig  || mkdir -p $(ROOTFS_DIR)/etc/sysconfig
	cp  fan_rpc.ini	$(ROOTFS_DIR)/etc/sysconfig
	cp  zlog.ini	$(ROOTFS_DIR)/etc/sysconfig

clean:
	rm -rf $(OBJS)  $(BOARD_SENSOR_RPC_SERVER_BIN) $(BOARD_VERSION_BIN)
