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

#ifdef ISP_DEBUG
#define DPRINTF(fmt, args...) printf(fmt, ##args);
#else
#define DPRINTF(FMT, args...)
#endif
static volatile unsigned int *gpio_base;
static volatile unsigned int *gpio_dir_base;
static volatile unsigned int *scu_base;
static int mem_fd;
static int mem_dir_fd;
static int scu_fd ;
static int offset;
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

static void print_help(void)
{
    printf("scr [r|w] reg [val] \n");
    exit(-1);

}

int pin_to_gpio(char gpio, int num)
{

#define SET_SCU(addr) printf("scu%x old_value 0x%x\n",addr,*(volatile unsigned int *)((char*)scu_base + addr) ); *(volatile unsigned int *)((char*)scu_base + addr)
    switch(gpio)
    {
        case 'A':
            gpio_base = (volatile unsigned int *)isp_gpio_map(GPIOA_DATA_ADDR, &mem_fd);
            gpio_dir_base = (volatile unsigned int *)isp_gpio_map(GPIOA_DIR_ADDR, &mem_dir_fd);
            offset = 0;
            SET_SCU(0x80) &= ~(1);
            break;
        case 'L':
            gpio_base = (volatile unsigned int *)isp_gpio_map(GPIOL_DATA_ADDR, &mem_fd);
            gpio_dir_base = (volatile unsigned int *)isp_gpio_map(GPIOL_DIR_ADDR, &mem_dir_fd);
            offset = 24;
            SET_SCU(0x90) &= ~(1<<5);
            SET_SCU(0x84) &= ~(1 <<(16+num));
            break;
        case 'B':
            offset = 8;
            gpio_base = (volatile unsigned int *)isp_gpio_map(GPIOD_DATA_ADDR, &mem_fd);
            gpio_dir_base = (volatile unsigned int *)isp_gpio_map(GPIOD_DIR_ADDR, &mem_dir_fd);

            if(num == 4) 
            {
                //set strap[23] 0
                SET_SCU(0x7c) |= (1<<23);
            }
            if(num>=5 && num <= 6)
            {
                SET_SCU(0x80) &= ~(1<< (8+num));
            }
            break;
        case 'M':
            offset=0;
            gpio_base = (volatile unsigned int *)isp_gpio_map(GPIOM_DATA_ADDR, &mem_fd);
            gpio_dir_base = (volatile unsigned int *)isp_gpio_map(GPIOM_DIR_ADDR, &mem_dir_fd);

            SET_SCU(0x90) &= ~(1<< 5);
            SET_SCU(0x84) &= ~(1<< (24+num));
            SET_SCU(0x94) |= (3<<0);
            break;
        case 'D':
            offset=24;
            gpio_base = (volatile unsigned int *)isp_gpio_map(GPIOD_DATA_ADDR, &mem_fd);
            gpio_dir_base = (volatile unsigned int *)isp_gpio_map(GPIOD_DIR_ADDR, &mem_dir_fd);

            SET_SCU(0x90) &= ~(1<<1);
            //set strap[21] 0
            SET_SCU(0x7c) |= (1<<21);
            num &= ~(1<<0);
            SET_SCU(0x8c) &= ~(0xf << 8);
            SET_SCU(0xa8) &= ~(0xf << 20);
            break;
        default:
            print_help();
            break; 
    }
    return 0;

}
int gpio_to_out(char gpio, int num, int n)
{
    printf("GPIO%c_dir old_value 0x%x\n",gpio,*(volatile unsigned int *)gpio_dir_base);
    *(volatile unsigned int *)gpio_dir_base |= (1 << (offset + num));
    return 0;
    
}


int gpio_value(char gpio, int num, int n)
{

    unsigned int value;
    if (n < 0)
    {
        printf("GPIO%c mode  0x%x\n",gpio, (*(volatile unsigned int *)gpio_dir_base >> offset) & 0xff);
        printf("GPIO%c value 0x%x\n",gpio, (*(volatile unsigned int *)gpio_base >> offset) & 0xff);
        return 0;

    }
    printf("GPIO%c%d %d old_value 0x%x\n",gpio,num,n,*(volatile unsigned int *)gpio_base);
    value = *(volatile unsigned int *)gpio_base ;
    if(n == 1)
    {
        value |= (1 << (offset + num));
    }else{
        value &= ~(1 << (offset + num));
    }
    *(volatile unsigned int *)gpio_base =value;
    printf("GPIO%c%d %d new_value 0x%x\n",gpio,num,n,value);
    return 0;

}

int main(int argc, char *argv[])
{
    int scu_reg;
    int val;
    char *stop3 = NULL, *stop4 = NULL;
    if(argc != 3 && argc != 4  )
    {
        print_help();
    }

    scu_reg  = (unsigned short)strtoul(argv[2], &stop4, 16);
    if('\0' != *stop4) {
        print_help();
    }

    scu_base = (volatile unsigned int *)isp_gpio_map(SYSTEM_SCU_BASE_ADDR, &scu_fd);
    if(*argv[1] == 'w') {
        val  = (unsigned short)strtoul(argv[3], &stop3, 16);
        if('\0' != *stop3) {
            print_help();
        }
        SET_SCU(scu_reg) = val;
        printf("set scu 0x%x to 0x%x",scu_reg, val);
    } else if (*argv[1] == 'r' ) {
        printf("get scu 0x%x val 0x%x\n",scu_reg,*(volatile unsigned int *)((char*)scu_base + scu_reg));
    } else {
            print_help();
    }
    scu_base = (volatile unsigned int *)isp_gpio_map(SYSTEM_SCU_BASE_ADDR, &scu_fd);

    

    isp_gpio_unmap((void*)scu_base, scu_fd);
    return 0;
}
