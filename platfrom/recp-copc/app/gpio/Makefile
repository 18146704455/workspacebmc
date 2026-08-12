
#CC      = $(CROSS_COMPILE)gcc
#AR      = $(CROSS_COMPILE)ar
#LD      = $(CROSS_COMPILE)ld
CFLAGS  = -Wall -Werror -std=gnu99 -fPIC
APP_ALL = gpio mmem


gpio: main.c
	$(CC) $^ -o $@

mmem: mmem.c
	$(CC) $^ -o $@ 

scu:scu.c
	$(CC) $^ -o $@ 

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

install: $(APP_ALL)
	cp -af $< $(ROOTFS_DIR)/usr/sbin/

clean:
	-rm -f $(APP_ALL)
	-rm -f *.o io

.PHONY: clean install
