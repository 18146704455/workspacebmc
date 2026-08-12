#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "flash.h"

//SPI BUS1, CS0
#define FLASH_DEV           "/dev/spidev0.0"
//#define FLASH_DEV               "/dev/mtd4"
#define DUMMY                 0x00
//depends on SPI FIFO deep length
#define PAGE_SZ               256
#define msleep(x)             usleep((x) * 1000)

#define CMD_READ_ID           0x9F
#define CMD_WR_EN             0x06
#define CMD_WR_DISABLE        0x04
#define CMD_RD_STATUS1        0X05
#define CMD_WR_STATUS1        0X01
#define CMD_CHIP_ERASE        0xc7
#define CMD_PAGE_PROGRAM      0x02
#define CMD_SECTOR_ERASE      0x20
#define CMD_BLOCK_32K_ERASE   0x52
#define CMD_BLOCK_64K_ERASE   0xd8
#define CMD_READ              0x03

//for 4 bytes address mode
#define CMD_READ_4B_MODE                         0x13
#define CMD_BLK_ERASE_256K_4B_MODE               0xdc
#define CMD_PAGE_PROGRAM_4B_MODE                 0x12
#define CMD_BLK_256K_ERASE_FOR_SPANSON_3B_MODE   0xd8

#define SUCCESS               0
#define ERROR                 1

typedef union STATUS_REG1_rd{
    uint8_t u8;
    struct STATUS_REG1_b_rd{
        uint8_t busy:1;     /*erase/write in progress*/
        uint8_t wel:1;      /*write enable latch*/
        uint8_t bp0:1;      /*block protect bits*/
        uint8_t bp1:1;      /*block protect bits*/
        uint8_t bp2:1;      /*block protect bits*/
        uint8_t tb:1;       /*top/bottom protect bits*/
        uint8_t sec:1;      /*sector protect bit*/
        uint8_t srp0:1;     /*status register protect 0*/
    }b;
}STATUS_REG1_T;

static int spidev_1_1 = -1;
static pthread_mutex_t g_spi_lock = PTHREAD_MUTEX_INITIALIZER;

static int spi_write(const uint8_t* wbuf, uint32_t wlen)
{
    struct spi_ioc_transfer tr = {
        .tx_buf   = (unsigned long)wbuf,
        .rx_buf   = 0,
        .len      = wlen,
    };

    return ioctl(spidev_1_1, SPI_IOC_MESSAGE(1), &tr);
}

static int spi_write_then_read(const uint8_t* wbuf, uint32_t wlen,
                               uint8_t* rbuf, uint32_t rlen)
{
    struct spi_ioc_transfer tr[2] = {
       {
            .tx_buf   = (unsigned long)wbuf,
            .rx_buf   = 0,
            .len      = wlen,
        },{
             .tx_buf  = 0,
            .rx_buf   = (unsigned long)rbuf,
            .len      = rlen,
        },
    };

    return ioctl(spidev_1_1, SPI_IOC_MESSAGE(2), tr);
}

static int open_flash(void)
{
    pthread_mutex_lock(&g_spi_lock);
    spidev_1_1 = open(FLASH_DEV, O_RDWR);
    if(spidev_1_1 < 0){
        SPI_ERR("open %s error\n", FLASH_DEV);
        pthread_mutex_unlock(&g_spi_lock);
        return ERROR;
    }
    return SUCCESS;
}

static void close_flash(void)
{
    (void)close(spidev_1_1);
    pthread_mutex_unlock(&g_spi_lock);
}

static int read_chip_id(unsigned short* id)
{
    uint8_t cmd[] = {CMD_READ_ID};
    if(spi_write_then_read(cmd, sizeof(cmd),
                           (uint8_t*)id,  sizeof(*id)) < 0) {
        SPI_ERR("can't detect %s device\n", FLASH_DEV);
        return ERROR;
    }

    return SUCCESS;
}

static int read_chip_sts_reg1(uint8_t* sts_reg)
{
    uint8_t cmd = CMD_RD_STATUS1;
    if(spi_write_then_read(&cmd,    sizeof(cmd),
                           sts_reg, sizeof(*sts_reg)) < 0) {
        return ERROR;
    }

    return SUCCESS;
}

static int write_enable(void)
{
    uint8_t cmd = CMD_WR_EN;
    STATUS_REG1_T sts_reg1;
    uint8_t retry = 0;

    do {
        if(spi_write(&cmd, sizeof(cmd)) < 0) {
            SPI_ERR("ERROR in sending write enable command:%x\n", cmd);
            return ERROR;
        }

        if(read_chip_sts_reg1((uint8_t*)(&sts_reg1))){
            SPI_ERR("ERROR in sending read chip status register1 command\n");
            return ERROR;
        }

        retry++;
    }while((!sts_reg1.b.wel) && (retry < 5)); //bit1: Write Enable Latch

    if (5 == retry) {
        SPI_ERR("ERROR in sending write enable command timeout\n");
        return ERROR;
    }

    return SUCCESS;
}

static int write_disable(void)
{
    uint8_t cmd = CMD_WR_DISABLE;
    STATUS_REG1_T sts_reg1;
    uint8_t retry = 0;

    do {
        if(spi_write(&cmd, sizeof(cmd)) < 0) {
            SPI_ERR("ERROR in sending write disable command: %x\n", cmd);
            return ERROR;
        }

        if(read_chip_sts_reg1((uint8_t*)(&sts_reg1))){
            SPI_ERR("ERROR in sending read chip status register1 command\n");
            return ERROR;
        }

        retry++;
    }while(sts_reg1.b.wel && (retry < 5)); //bit1: Write Enable Latch

    if (5 == retry) {
        SPI_ERR("ERROR in sending write disable command timeout\n");
        return ERROR;
    }

    return SUCCESS;
}

static int chip_erase(int type, uint32_t addr, uint32_t size)
{
    uint8_t i = 0, j = 0;
    uint32_t addr_align, loop;
    STATUS_REG1_T sts_reg1;
    uint8_t cmd[4];

    switch(type){
        case SECTOR_ERASE:
             cmd[0] = CMD_SECTOR_ERASE;
             addr_align = 4 * 1024; //4K
             break;
        case BLOCK_ERASE_32K:
             cmd[0] = CMD_BLOCK_32K_ERASE;
             addr_align = 32 * 1024;
             break;
        case BLOCK_ERASE_64K:
             cmd[0] = CMD_BLOCK_64K_ERASE;
             addr_align = 64 * 1024;
             break;
        case BLK_ERASE_256K_3B_MODE:
             cmd[0] = CMD_BLK_256K_ERASE_FOR_SPANSON_3B_MODE;
             addr_align = 256 * 1024;
             break;
        default:
             cmd[0] = CMD_CHIP_ERASE;
    }

    SPI_PRINT("\nChip Erase......\n");
    /*erase whole chip*/
    if (cmd[0] == CMD_CHIP_ERASE){
        write_enable();
        if(spi_write(&cmd[0], sizeof(cmd[0])) < 0){
            SPI_ERR("ERROR in erase whole chip\n");
            return ERROR;
        }

        do {
            if(read_chip_sts_reg1((uint8_t*)(&sts_reg1))){
                SPI_ERR("ERROR in sending read chip status register1 command\n");
                return ERROR;
            }
            if(sts_reg1.b.busy){
                msleep(100);
            }

            if(0 == i++ % 64)
                putchar('\n');
            putchar('#');
        }while(sts_reg1.b.busy);
    } else {
        if (addr % addr_align || size % addr_align) {
            SPI_ERR("addr: %08x size %08x is not %08x aligned\n", addr, size, addr_align);
            return ERROR;
        }

        loop = size / addr_align;
        for (j = 0; j < loop; j++){
            printf("\rErasing %d Kibyte @ %08x -- %2i %% complete ", \
                              addr_align / 1024,                     \
                              addr,                                  \
                              (j == (loop - 1)) ? 100 : (100 * 100 / loop) * (j + 1) / 100);
            write_enable();
            cmd[1] = *((uint8_t*)&addr + 2);
            cmd[2] = *((uint8_t*)&addr + 1);
            cmd[3] = *((uint8_t*)&addr);
            if(spi_write(cmd, 4) < 0) {
                SPI_ERR("ERROR in erase chip, type %02x\n", cmd[0]);
                return ERROR;
            }

            do {
                if(read_chip_sts_reg1((uint8_t*)(&sts_reg1))){
                    SPI_ERR("ERROR in sending read chip status register1 command\n");
                    return ERROR;
                }
            }while(sts_reg1.b.busy);
            addr += addr_align;
        }

    }
    SPI_PRINT("\ndone\n");
    return SUCCESS;
}

static int chip_read(uint32_t addr, uint8_t* data, uint32_t size)
{
    uint8_t cmd[4]   = {CMD_READ,};
    uint8_t* p_data = data;
    uint32_t i, loop, remain, rd_sz;

    if(size % PAGE_SZ == 0) {
        loop = size / PAGE_SZ;
        remain = 0;
    } else {
        loop = size / PAGE_SZ + 1;
        remain = size % PAGE_SZ;
    }

    for(i = 0; i < loop; i++){
        cmd[1] = *((uint8_t*)&addr + 2);
        cmd[2] = *((uint8_t*)&addr + 1);
        cmd[3] = *((uint8_t*)&addr);

        rd_sz  = ((i == loop - 1) && remain) ? remain : PAGE_SZ;
        if(spi_write_then_read(cmd, sizeof(cmd),
                               p_data, rd_sz) < 0) {
            SPI_ERR("ERROR in read chip data\n");
            return ERROR;
        }
        p_data += PAGE_SZ;
        addr   += PAGE_SZ;
    }

    return SUCCESS;
}

/*page program*/
static int chip_program(uint32_t addr, uint8_t* data, uint32_t size)
{
    uint8_t* p_data = data;
    uint32_t i = 0;
    uint64_t j = 0;
    int loop, remain, wr_sz;
    STATUS_REG1_T sts_reg1;
    uint8_t* cmd = calloc(1, 4 + PAGE_SZ);
    assert(NULL != cmd);

    if(size % PAGE_SZ == 0) {
        loop = size / PAGE_SZ;
        remain = 0;
    } else {
        loop = size / PAGE_SZ + 1;
        remain = size % PAGE_SZ;
    }

    printf("\nChip Program......\n");
    for(i = 0; i < loop; i++,j++){
        //256B * 1024
        if(j % 1024 == 0){ //256KiByte
            printf("\rProgram %d Kibyte @ %08x -- %2d %% complete ", \
                              256,                                    \
                              addr,                                   \
                              (int)((i == (loop - 1)) ? 100 : (100 * j) / loop));
        }

        write_enable();
        cmd[0] = CMD_PAGE_PROGRAM;
        cmd[1] = *((uint8_t*)&addr + 2);
        cmd[2] = *((uint8_t*)&addr + 1);
        cmd[3] = *((uint8_t*)&addr);
        wr_sz  = ((i == loop - 1) && remain) ? remain : PAGE_SZ;
        memcpy(cmd + 4, p_data, wr_sz);
        if(spi_write(cmd, wr_sz + 4) != (wr_sz + 4)) {
            SPI_ERR("ERROR in program chip\n");
            free(cmd);
            return ERROR;
        }

        do {
            if(read_chip_sts_reg1((uint8_t*)(&sts_reg1))){
                SPI_ERR("ERROR in sending read chip status register1 command\n");
                free(cmd);
                return ERROR;
            }
        }while(sts_reg1.b.busy);

        p_data += PAGE_SZ;
        addr   += PAGE_SZ;
    }

    printf("\rProgram %d Kibyte @ %08x -- %2d %% complete ", \
                     256,                                    \
                     addr,                                   \
                     100);

    printf("\ndone\n\n");
    free(cmd);
    return SUCCESS;
}

static int protect_off_all(void)
{
    STATUS_REG1_T sts_reg1, off_prot;
    uint8_t cmd[2] = {CMD_WR_STATUS1,};

    if(read_chip_sts_reg1((uint8_t*)(&sts_reg1))){
        SPI_ERR("ERROR in sending read chip status register1 command\n");
        return ERROR;
    }

    if(0 == sts_reg1.b.sec && \
       0 == sts_reg1.b.tb  && \
       0 == sts_reg1.b.bp0 && \
       0 == sts_reg1.b.bp1 && \
       0 == sts_reg1.b.bp2) {
        return  SUCCESS;
    }

    off_prot.u8 = 0;
    cmd[1] = off_prot.u8;
    if(spi_write(cmd, sizeof(cmd)) < 0) {
        SPI_ERR("ERROR in protect off all\n");
        return ERROR;
    }
    return SUCCESS;
}

FLASH_OPS flash_intf = {
    .open    = open_flash,
    .close   = close_flash,
    .detect  = read_chip_id,
    .wel     = write_enable,
    .wdl     = write_disable,
    .erase   = chip_erase,
    .read    = chip_read,
    .program = chip_program,
    .protect_off_all = protect_off_all,
};
