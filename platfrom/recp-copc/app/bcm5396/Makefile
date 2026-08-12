
LIB_NAME=libbcm5396.so
BIN_NAME=bcmspi

CC      = $(CROSS_COMPILE)gcc
AR      = $(CROSS_COMPILE)ar
LD      = $(CROSS_COMPILE)ld

CFLAGS  = -Wall -Werror -std=gnu99 -fPIC
LDFLAGS = -fPIC -shared -Wl,-soname,$(LIB_NAME)

default: $(LIB_NAME) $(BIN_NAME)

$(BIN_NAME): spi_bigbang.o main.o 
	$(CC) $^ -o $@ -lrt

$(LIB_NAME): spi_bigbang.o 
	$(CC) $^ -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o  $(BIN_NAME) $(LIB_NAME)

install: $(LIB_NAME) $(BIN_NAME)
	cp -f $(LIB_NAME)       $(ROOTFS_DIR)/lib/
	cp -f $(BIN_NAME)              $(ROOTFS_DIR)/usr/sbin/

.PHONY: clean install
