
CC      = $(CROSS_COMPILE)gcc
AR      = $(CROSS_COMPILE)ar
LD      = $(CROSS_COMPILE)ld
CFLAGS  = -Wall -Werror -std=gnu99 -fPIC

#dependancy objs

OBJS  =  power_ctl.o ../cpld/libcpld.o  pwr_phase_ctrl.o

#default target
default:  payload



payload: $(OBJS)
	$(CC) $^ -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<

install:  payload
	cp -af $^ $(ROOTFS_DIR)/usr/bin/

clean:
	rm -f *.o  payload

.PHONY: clean install
