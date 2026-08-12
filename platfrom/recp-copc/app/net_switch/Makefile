
#CC      = $(CROSS_COMPILE)gcc
#AR      = $(CROSS_COMPILE)ar
#LD      = $(CROSS_COMPILE)ld

CFLAGS  = -Wall -Werror -std=gnu99 -fPIC

default: phyr phyw sw

phyr: phy.o phyr.o
	$(CC) $^ -o $@

phyw: phy.o phyw.o 
	$(CC) $^ -o $@

sw:phy.o netswitch.o main_sw.o
	$(CC) $^ -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o  phyr phyw sw

install: phyr phyw sw
	cp -af $^ $(ROOTFS_DIR)/usr/sbin/

.PHONY: clean install
