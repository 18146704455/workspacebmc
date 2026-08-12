#include <stdio.h>
#include <unistd.h>  
#include <sys/mman.h>  
#include <stdlib.h>  
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define AST_SCU_BASE            0x1E6E2000
#define AST_GPIO_BASE           0x1E780000

static uint32_t scu_reg_base;
static uint32_t gpio_reg_base;

static int mem_map(void)
{
    int fd;

    fd = open("/dev/mem", O_RDWR|O_SYNC);
    if (fd == -1) {
        return (-1);
    }

    scu_reg_base   = (uint32_t)mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE,
                                   MAP_SHARED, fd, AST_SCU_BASE);
    gpio_reg_base  = (uint32_t)mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE,
                                   MAP_SHARED, fd, AST_GPIO_BASE);
    if (0 == scu_reg_base  || 0 == gpio_reg_base) {
        printf("NULL pointer!\n");
        return (-1);
    }

    close(fd);
    return 0;
}

static void mem_unmap(void)
{
    munmap((void*)scu_reg_base, getpagesize());
    munmap((void*)gpio_reg_base, getpagesize());
}

uint32_t scu_read(uint16_t offset)
{
    uint32_t value = 0;

    mem_map();
    value = *((uint32_t *)(scu_reg_base + offset));
    mem_unmap();
    return value;
}

void scu_write(uint16_t offset, uint32_t value)
{
    mem_map();
    *((uint32_t *)(scu_reg_base + offset)) = value;
    mem_unmap();
}

uint32_t gpio_read(uint16_t offset)
{
    uint32_t value = 0;

    mem_map();
    value = *((uint32_t *)(gpio_reg_base + offset));
    mem_unmap();
    return value;
}

void gpio_write(uint16_t offset, uint32_t value)
{
    mem_map();
    *((uint32_t *)(gpio_reg_base + offset)) = value;
    mem_unmap();
}
