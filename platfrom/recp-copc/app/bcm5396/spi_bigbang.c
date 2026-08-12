/*
 * @Author: younger
 * @Date: 2023-06-28 15:32:37
 * @LastEditors: younger
 * @LastEditTime: 2023-06-28 17:24:04
 * @FilePath: \bcm5396_dpc\spi_bigbang.c
 * @Description:
 * @Email: yang.yang@scidata.cn
 * Copyright (c) 2023 by scistor/younger, All Rights Reserved.
 */
/*
 * @Author: younger
 * @Date: 2023-06-28 15:32:37
 * @LastEditors: younger
 * @LastEditTime: 2023-06-28 15:46:29
 * @FilePath: \bcm5396_dpc\spi_bigbang.c
 * @Description:
 * @Email: yang.yang@scidata.cn
 * Copyright (c) 2023 by scistor/younger, All Rights Reserved.
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define SCU_BASE          0x1E6E2000
#define GPIO_BASE         0x1E780000
#if 0

//AB0 start
#define PIN_BASE            0x1E0
#define CS_PIN           (1UL << 24)
#define CLI_PIN           (1UL << 25)
#define MOSI_PIN           (1UL << 26)
#define MISO_PIN           (1UL << 27)

//S2 start
#define PIN_BASE 0x80
#define CS_PIN (1UL << 18)
#define CLI_PIN (1UL <<19)
#define MOSI_PIN (1UL <<20)
#define MISO_PIN (1UL <<21)
#else
//E0 start
#define PIN_BASE 0x20
#define CS_PIN (1UL << 0)
#define CLI_PIN (1UL <<1)
#define MOSI_PIN (1UL <<2)
#define MISO_PIN (1UL <<3)
#endif


#define READ_CMD           0x60
#define WRITE_CMD          0x61
#define SPI_STS_DIO        0xf0
#define SPI_STS_REG        0xfe
#define PAGE_REG           0xff

#define SPI_STS_REG_RACK  (1 << 5)
#define SPI_STS_REG_SPIF  (1 << 7)

static uint32_t ast_read_gpio(uint32_t off);
static void ast_write_gpio(uint32_t off, uint32_t val);
static volatile unsigned int delay;
static void*scu_virt, *gpio_virt;

#define SOFT_DELAY(n) \
do{ \
    for(delay = 0; delay < n; delay++) \
        ast_read_gpio(PIN_BASE); \
}while(0);

#define SET_CS_PIN(v) \
do{ \
    if(v){ \
        ast_write_gpio(PIN_BASE, ast_read_gpio(PIN_BASE) |  CS_PIN); \
    }else{ \
        ast_write_gpio(PIN_BASE, ast_read_gpio(PIN_BASE) & ~CS_PIN); \
    } \
    SOFT_DELAY(5); \
}while(0);

#define SET_CLK_PIN(v) \
do{ \
    if(v){ \
        ast_write_gpio(PIN_BASE, ast_read_gpio(PIN_BASE) |  CLI_PIN); \
    }else{ \
        ast_write_gpio(PIN_BASE, ast_read_gpio(PIN_BASE) & ~CLI_PIN); \
    } \
    SOFT_DELAY(1); \
}while(0);

#define SET_MOSI_PIN(v) \
do{ \
    if(v){ \
        ast_write_gpio(PIN_BASE, ast_read_gpio(PIN_BASE) |  MOSI_PIN); \
    }else{ \
        ast_write_gpio(PIN_BASE, ast_read_gpio(PIN_BASE) & ~MOSI_PIN); \
    } \
    SOFT_DELAY(1); \
}while(0);

#define READ_MISO_PIN \
    (ast_read_gpio(PIN_BASE) & MISO_PIN ? 1 : 0)

static int ast_mem_map(void)
{
    int fd;

    if((fd = open("/dev/mem", O_RDWR|O_SYNC)) < 0){
        return (-1);
    }

    scu_virt  = mmap(NULL,
                    getpagesize(),
                    PROT_READ|PROT_WRITE,
                    MAP_SHARED,
                    fd,
                    SCU_BASE);

    gpio_virt = mmap(NULL,
                    getpagesize(),
                    PROT_READ|PROT_WRITE,
                    MAP_SHARED,
                    fd,
                    GPIO_BASE);

    if (MAP_FAILED == scu_virt || \
        MAP_FAILED == gpio_virt){
        close(fd);
        return (-1);
    }

    close(fd);
    return 0;
}

static void ast_mem_unmap(void)
{
    munmap(scu_virt,  getpagesize());
    munmap(gpio_virt, getpagesize());
}

#if 1
static uint32_t ast_read_scu(uint32_t off)
{
    return *(uint32_t volatile*)((char*)scu_virt + off);
}

static void ast_write_scu(uint32_t off, uint32_t val)
{
    *(uint32_t volatile*)((char*)scu_virt + off) = val;
}
#endif

static uint32_t ast_read_gpio(uint32_t off)
{
    return *(uint32_t volatile*)((char*)gpio_virt + off);
}

static void ast_write_gpio(uint32_t off, uint32_t val)
{
    *(uint32_t volatile*)((char*)gpio_virt + off) = val;
}

static void ast_spi_emulator_init(void)
{
#if 0
    unsigned int reg;

     /*Configure SPI2 to standard GPIO*/
     //N19 GPIOAB0 ID/O8 Bidir Hi-Z,Input SCU90[31]=0 & (SCUA8[0]=0 || SCU94[1:0]=0)
     //T21 GPIOAB1 ID/O8 Bidir Hi-Z,Input SCU90[31]=0 & (SCUA8[1]=0 || SCU94[1:0]=0)
     //T22 GPIOAB2 ID/O8 Bidir Hi-Z,Input SCUA8[2]=0
     //R20 GPIOAB3 ID/O8 Bidir Hi-Z,Input SCUA8[3]=0
     reg = ast_read_scu(0x90);
     reg &= ~(1 >> 31);
     ast_write_scu(0x90, reg);

     reg = ast_read_scu(0xa8);
     reg &= ~((1 >> 0) | (1 >> 1) | (1 >> 2) | (1 >> 3));
     ast_write_scu(0xa8, reg);

     reg = ast_read_scu(0x94);
     reg &= ~((1 >> 0) | (1 >> 1));
     ast_write_scu(0x94, reg);

    /*
     * CS   -> GPIOAB0,  CLK   -> GPIOAB1
     * MOSI -> GPIOAB2,  MISO  -> GPIOAB3
     */
     //Direction Register
     reg = ast_read_gpio(0x1E4);
     reg |= ((1 << 24) | (1 << 25) | (1 << 26)); //output mode
     reg &= ~(1 << 27);
     ast_write_gpio(0x1E4, reg);

     //Value Register
     reg = ast_read_gpio(0x1E0);
     reg |=  (1 << 24) | (1 << 25); //CS high
     reg &= ~(1 << 26); //SPI mode1
     ast_write_gpio(0x1E0, reg);
     usleep(1);
//#else
     unsigned int reg;
     //Configure SPI1 to standard GPIO
     // CS -> GPIOS2,   CLK ->GPIOS3
     // MOSI ->GPIOS4, MISO     ->GPIOS5

     //init pin to gpio
     reg = ast_read_scu(0x8c);
     reg &= ~(1<<2);
     reg &= ~(1<<3);
     reg &= ~(1<<4);
     reg &= ~(1<<5);
     ast_write_scu(0x8c,reg);
     // Direction Register
     reg = ast_read_gpio(0x84);
     reg |= (1<<18);
     reg |=(1<<19);
     reg|= (1<<20);
     reg &=~(1<<21);
     ast_write_gpio(0x84,reg);
     reg = ast_read_gpio(0x80);
     reg |= (1 << 18);
     reg |= (1 << 19);
     reg &=~ (1<<20);
     ast_write_gpio(0x80,reg);
#else
unsigned int reg;
    //Configure SPI1 to standard GPIO
    // CS -> GPIOE0,   CLK ->GPIOE1
    // MOSI ->GPIOE2, MISO     ->GPIOE3

    //init pin to gpio
    #if 0
    reg = ast_read_scu(0x70);
    reg &= ~(1<<22);
    ast_write_scu(0x70,reg);


    reg = ast_read_scu(0x80);
    reg &= ~(1<<16);
    reg &= ~(1<<17);
    reg &= ~(1<<18);
    reg &= ~(1<<19);
    ast_write_scu(0x80,reg);


    reg = ast_read_scu(0x8c);
    reg &= ~(1<<8);
    reg &= ~(1<<9);
    reg &= ~(1<<22);
    reg |= (1<<20);
    ast_write_scu(0x8c,reg);



    reg = ast_read_scu(0xa8);
    reg &= ~(1<<24);
    reg &= ~(1<<25);
    ast_write_scu(0xa8,reg);
     #endif
    reg = ast_read_scu(0x80);
    reg &= ~(1<<16);
    reg &= ~(1<<17);
    reg &= ~(1<<18);
    reg &= ~(1<<19);
    ast_write_scu(0x80,reg);


     // Direction Register
     reg = ast_read_gpio(0x24);
     reg |= (1<<0);
     reg |= (1<<1);
     reg |= (1<<2);
     reg &=~(1<<3);
     ast_write_gpio(0x24,reg);
     reg = ast_read_gpio(0x20);
     reg |= (1 << 0);
     reg |= (1 << 1);
     reg &=~ (1<< 2);
     ast_write_gpio(0x20,reg);
#endif

}

//SPI MODE0
static void ast_spi_transfer(uint8_t* snd, uint32_t snd_sz, uint8_t* rd, uint32_t rd_sz)
{
    uint32_t  i;
    volatile int j;
    uint8_t byte;

    //CS 0, sustain half cycle
    SET_CS_PIN(0);

    if(NULL != snd){
        for(i = 0; i < snd_sz; i++) {
            byte = snd[i];
            for(j = 7; j >= 0; j--){
                SET_CLK_PIN(0);

                //snd Data
                if(byte & (1 << j)){
                    SET_MOSI_PIN(1);
                }else{
                    SET_MOSI_PIN(0);
                }

                //CK = 1  rising edge sampling
                SET_CLK_PIN(1);
                SOFT_DELAY(1);
            }
            SOFT_DELAY(4); //sustain 2 cycle
        }
    }

    if(NULL != rd){
        for(i = 0; i < rd_sz; i++) {
            for(byte = 0, j = 0; j < 8; j++){
                SET_CLK_PIN(0);
                SOFT_DELAY(1);

                //sampling bit
                if(READ_MISO_PIN){
                    byte |= (1 << (7 - j));
                }

                SET_CLK_PIN(1);
                SOFT_DELAY(1);
            }
            rd[i] = byte;
            SOFT_DELAY(4);
        }
    }

    //CS negative
    SET_CS_PIN(1);
}

static unsigned char read_spi_sts(void)
{
    unsigned char byte = 0;
    unsigned char rd_sts_cmd[2];

    rd_sts_cmd[0] = READ_CMD;
    rd_sts_cmd[1] = SPI_STS_REG;

    ast_spi_transfer(rd_sts_cmd, sizeof(rd_sts_cmd),
                     &byte, 1);

    return byte;
}

static void set_page(unsigned char page)
{
    unsigned char set_page_cmd[3];

    set_page_cmd[0] = WRITE_CMD;
    set_page_cmd[1] = PAGE_REG;
    set_page_cmd[2] = page;

    ast_spi_transfer(set_page_cmd, sizeof(set_page_cmd), NULL, 0);
}

//to be discard
static unsigned char read_reg_address(unsigned char addr)
{
    unsigned char byte = 0;
    unsigned char rd_reg_cmd[2];

    rd_reg_cmd[0] = READ_CMD;
    rd_reg_cmd[1] = addr;

    ast_spi_transfer(rd_reg_cmd, sizeof(rd_reg_cmd), &byte, 1);
    return byte;
}

static void readb_dio_reg(unsigned char*byte, unsigned char size)
{
    unsigned char rd_dio_cmd[2];

    rd_dio_cmd[0] = READ_CMD;
    rd_dio_cmd[1] = SPI_STS_DIO;

    ast_spi_transfer(rd_dio_cmd, sizeof(rd_dio_cmd), byte, size);
}

static void set_reg_address_with_data(uint8_t addr, uint8_t* byte, uint8_t sz)
{
    unsigned char set_addr_cmd[256];

    set_addr_cmd[0] = WRITE_CMD;
    set_addr_cmd[1] = addr;
    memcpy(set_addr_cmd + 2, byte, sz);

    ast_spi_transfer(set_addr_cmd, 2 + sz,  NULL, 0);
}

extern int bcm5389_read(uint8_t page, uint8_t reg, uint8_t* byte, uint8_t size)
{
    int ret = 0;

    ast_mem_map();
    ast_spi_emulator_init();

    //Step1, check SPIF ?= 0
    if((read_spi_sts() & SPI_STS_REG_SPIF) != 0){
        printf("The SPI is not in compete state\n");
        ret = -1;
        goto out;
    }

    //Step2, Set page
    set_page(page);
    //Step3, Read Reg address
    read_reg_address(reg); //to be discard
    //Step4 Check RACK
    if(!(read_spi_sts() & SPI_STS_REG_RACK)){
        printf("No RACK Received\n");
        ret = -1;
        goto out;
    }

    //Step5 read SDIO
    readb_dio_reg(byte, size);
    //for(i = 0; i < size; i++)
    //    printf("%02x", data[size - i - 1]);
    //printf("\n");

out:
    ast_mem_unmap();
    return ret;
}

extern int bcm5389_write(uint8_t page,  uint8_t reg, uint8_t* byte, uint8_t size)
{
    int ret = 0;

    ast_mem_map();
    ast_spi_emulator_init();

    //Step1, check SPIF ?= 0
    if((read_spi_sts() & SPI_STS_REG_SPIF) != 0){
        printf("The SPI is not in compete state, sts %x\n", read_spi_sts());
        ret = -1;
        goto out;
    }

    //Step2, Set page
    set_page(page);

    //Step3 config reg
    set_reg_address_with_data(reg, byte, size);

out :
    ast_mem_unmap();
    return ret;
}
