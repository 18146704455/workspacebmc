
CC      = $(CROSS_COMPILE)gcc
AR      = $(CROSS_COMPILE)ar
LD      = $(CROSS_COMPILE)ld
CFLAGS  = -Wall -Werror -std=gnu99 -fPIC
LIBS    = -lcpld -L../cpld/
INCLUDE = -I $(shell pwd)/../include

oflash: $(patsubst %.c, %.o, $(wildcard *.c))
	$(CC) $^ -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

install: oflash
	cp -af $< $(ROOTFS_DIR)/usr/sbin/

clean:
	rm -f *.o oflash

.PHONY: clean install
