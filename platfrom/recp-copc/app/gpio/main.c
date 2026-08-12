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
static volatile unsigned int *lpc_base;
static int mem_fd;
static int mem_dir_fd;
static int scu_fd ;
static int lpc_fd ;
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
    printf("gpio [w|r] [X|A|D|B|M|L] num[0-7] value[0|1] \n");
    exit(-1);

}

static int pin_to_gpio(char gpio, int num)
{

#define SET_SCU(addr) printf("scu%x old_value 0x%x\n",addr,*(volatile unsigned int *)((char*)scu_base + addr) ); *(volatile unsigned int *)((char*)scu_base + addr)
#define SET_LPC(addr) printf("lpc%x old_value 0x%x\n",addr,*(volatile unsigned int *)((char*)lpc_base + addr) ); *(volatile unsigned int *)((char*)lpc_base + addr)

    unsigned int data_addr = 0;
    unsigned int dir_addr = 0;
    switch(gpio)
    {
        case 'X': //mean AA
            //gpio_base = (volatile unsigned int *)isp_gpio_map(GPIOAA_DATA_ADDR, &mem_fd);
            data_addr = GPIOAA_DATA_ADDR;
            //gpio_dir_base = (volatile unsigned int *)isp_gpio_map(GPIOAA_DIR_ADDR, &mem_dir_fd);
            dir_addr = GPIOAA_DIR_ADDR;
            offset = 0;
            SET_SCU(0x80) &= ~(1);
            SET_SCU(0x90) &= ~(1 << 31);
            SET_SCU(0xA4) &= ~(0xff << 24);
            SET_SCU(0x94) &= ~(3);
            break;
        case 'A':
            //gpio_base = (volatile unsigned int *)isp_gpio_map(GPIOA_DATA_ADDR, &mem_fd);
            data_addr = GPIOA_DATA_ADDR;
            dir_addr = GPIOA_DIR_ADDR;
            //gpio_dir_base = (volatile unsigned int *)isp_gpio_map(GPIOA_DIR_ADDR, &mem_dir_fd);
            offset = 0;
            SET_SCU(0x80) &= ~(1);
            break;
        case 'L':
            //gpio_base = (volatile unsigned int *)isp_gpio_map(GPIOL_DATA_ADDR, &mem_fd);
            data_addr = GPIOL_DATA_ADDR;
            dir_addr = GPIOL_DIR_ADDR;
            //gpio_dir_base = (volatile unsigned int *)isp_gpio_map(GPIOL_DIR_ADDR, &mem_dir_fd);
            offset = 24;
            SET_SCU(0x90) &= ~(1<<5);
            SET_SCU(0x84) &= ~(1 <<(16+num));
            break;
        case 'B':
            offset = 8;
            //gpio_base = (volatile unsigned int *)isp_gpio_map(GPIOD_DATA_ADDR, &mem_fd);
            data_addr = GPIOD_DATA_ADDR;
            //gpio_dir_base = (volatile unsigned int *)isp_gpio_map(GPIOD_DIR_ADDR, &mem_dir_fd);
            dir_addr = GPIOD_DIR_ADDR;

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
            //gpio_base = (volatile unsigned int *)isp_gpio_map(GPIOM_DATA_ADDR, &mem_fd);
            data_addr = GPIOM_DATA_ADDR;
            dir_addr = GPIOM_DIR_ADDR;
            //gpio_dir_base = (volatile unsigned int *)isp_gpio_map(GPIOM_DIR_ADDR, &mem_dir_fd);

            SET_SCU(0x90) &= ~(1<< 5);
            SET_SCU(0x84) &= ~(1<< (24+num));
            SET_SCU(0x94) |= (3<<0);
            break;
        case 'D':
            offset=24;
            //gpio_base = (volatile unsigned int *)isp_gpio_map(GPIOD_DATA_ADDR, &mem_fd);
            data_addr= GPIOD_DATA_ADDR;
            dir_addr = GPIOD_DIR_ADDR;
            //gpio_dir_base = (volatile unsigned int *)isp_gpio_map(GPIOD_DIR_ADDR, &mem_dir_fd);

            SET_SCU(0x90) &= ~(1<<1);
            //set strap[21] 0
            SET_SCU(0x7c) |= (1<<21);
            num &= ~(1<<0);
            SET_SCU(0x8c) &= ~(0xf << 8);
            SET_SCU(0xa8) &= ~(0xf << 20);
            break;
        case 'F':
            offset=8;
            data_addr = GPIOF_DATA_ADDR;
            dir_addr= GPIOF_DIR_ADDR;
            //gpio_base = (volatile unsigned int *)isp_gpio_map(GPIOF_DATA_ADDR, &mem_fd);
            //gpio_dir_base = (volatile unsigned int *)isp_gpio_map(GPIOF_DIR_ADDR, &mem_dir_fd);
            if(num != 6)
            {
                SET_SCU(0x90) &= ~(1<<30);
            }
            SET_SCU(0x80) &= ~(1<< (24+num));
            SET_LPC(0xa0) &= ~(1<<0);
            break;

        default:
            print_help();
            break; 
    }
            gpio_base = (volatile unsigned int *)isp_gpio_map(data_addr, &mem_fd);
            gpio_dir_base = (volatile unsigned int *)isp_gpio_map(dir_addr, &mem_dir_fd);

    return 0;

}
static int gpio_to_out(char gpio, int num, int n)
{
    printf("GPIO%c_dir old_value 0x%x\n",gpio,*(volatile unsigned int *)gpio_dir_base);
    *(volatile unsigned int *)gpio_dir_base |= (1 << (offset + num));
    return 0;
    
}


static int gpio_value(char gpio, int num, int n)
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

    if(argc < 3 || (*argv[1] != 'r' && *argv[1]!='w' && *argv[1] != 'R' && *argv[1] != 'W'))
    {
        print_help();
    }

    scu_base = (volatile unsigned int *)isp_gpio_map(SYSTEM_SCU_BASE_ADDR, &scu_fd);
    lpc_base = (volatile unsigned int *)isp_gpio_map(SYSTEM_LPC_BASE_ADDR, &lpc_fd);

    pin_to_gpio(argv[2][0], atoi(argv[3]));
    if(argc == 4 && (*argv[1] == 'r' || *argv[1] == 'R'))
    {
        gpio_value(argv[2][0], atoi(argv[3]), -1);

    }
    if(argc == 5 && (*argv[1] == 'w' || *argv[1] == 'W'))
    {
        gpio_to_out(argv[2][0], atoi(argv[3]),atoi(argv[4]));
        gpio_value(argv[2][0], atoi(argv[3]), atoi(argv[4]));
    }

    isp_gpio_unmap((void*)lpc_base, lpc_fd);
    isp_gpio_unmap((void*)scu_base, scu_fd);
    isp_gpio_unmap((void*)gpio_base, mem_fd);
    isp_gpio_unmap((void*)gpio_dir_base, mem_dir_fd);

    return 0;
}
