
CC      = $(CROSS_COMPILE)gcc
AR      = $(CROSS_COMPILE)ar
LD      = $(CROSS_COMPILE)ld

CFLAGS  = -Wall -Werror -std=gnu99 -fPIC
MAIN_OBJS = i2cctrl.o  smbus.o command.o IR35215.o devctrl_main.o

all:plx

IR:$(MAIN_OBJS)
	$(CC) $^ -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o  IR
	
install: IR
	cp -af $< $(ROOTFS_DIR)/usr/sbin/

.PHONY: clean rootfs_install
