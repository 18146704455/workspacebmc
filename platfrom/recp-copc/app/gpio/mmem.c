#include "vmopcode.h"
#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
static void *isp_gpio_map(unsigned int addr, int *fop)
{
    int fd;
    void *base, *virt_addr;

    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd < 0) {
        printf("open /dev/mem error!\n");
        return NULL;
    }
    base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr & ~MAP_MASK);
    if(base == (void *)-1) {
        printf("map base is NULL!\n");
        return NULL;
    }
    virt_addr = base + (addr & MAP_MASK);
    *fop = fd;

    return virt_addr;
}
static void isp_gpio_unmap(void *addr, int fd)
{
    munmap(addr, MAP_SIZE);
    close(fd);
}

int main(int argc, char *argv[])
{
    int fd ;
    volatile unsigned int *base;
    unsigned int addr=0;
    sscanf(argv[1],"%x",&addr);
    base = (volatile unsigned int *)isp_gpio_map(addr,&fd);
    printf("addr[%04x]:[%04x]\n",addr,*(volatile unsigned int *)base);
    isp_gpio_unmap((void*)base,fd);
    return 0;
}
