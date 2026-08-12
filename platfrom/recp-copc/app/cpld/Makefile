
CC      = $(CROSS_COMPILE)gcc
AR      = $(CROSS_COMPILE)ar
LD      = $(CROSS_COMPILE)ld
CFLAGS  = -Wall -Werror -std=gnu99 -pedantic -fPIC
LDFLAGS = -fPIC -shared -Wl,-soname,libcpld.so
SLIBS   = $(patsubst %.c, %.o, $(wildcard lib*))

default: cpld libcpld.a libcpld.so

cpld: $(patsubst %.c, %.o, $(wildcard *.c))
	$(CC) $^ -o $@

libcpld.a: $(SLIBS)
	$(AR) -rcs $@ $^

libcpld.so: $(SLIBS)
	$(CC) $^ -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $<
install_lib: libcpld.a libcpld.so
	cp -af {libcpld.a,libcpld.so} $(ROOTFS_DEV_DIR)/lib
	cp -af cpld.h $(ROOTFS_DEV_DIR)/include
	

install: install_lib cpld 
	cp -af cpld $(ROOTFS_DIR)/usr/sbin/
	cp -af dev_func.sh $(ROOTFS_DIR)/usr/sbin/

clean:
	rm -f *.o cpld libcpld.a libcpld.so

.PHONY: clean install install_lib
