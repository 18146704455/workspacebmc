#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include "flash.h"

#define SZ_32K             (32* 1024)
#define SZ_64K             (64* 1024)
#define SZ_256K            (256* 1024)
#define CPLD_SPI_PROG_SEL  0xfe2d

extern int epld_write(uint32_t offset, uint8_t value);
FLASH_OPS* pflash = &flash_intf;

static void
usage(FILE *fp, int argc, char **argv)
{
    fprintf(fp,
        "Usage: %s [options]\n\n"
        "Options:\n"
        " -h | --help                 Print this message\n"
        " -r | --read                 Read Data from chip\n"
        " -p | --program              Program chip\n"
        " -f | --fn                   0-> cpu master flash\n"
        "                             1-> cpu backup flash\n"
        "                             2-> nps0 flash\n"
        "                             3-> nps1 flash\n"
        " -a | --addr                 address\n"
        " -s | --size                 size\n"
        "",
        argv[0]);
}

static const struct option
long_options [] = {
    { "help",    no_argument,       NULL, 'h' },
    { "read",    no_argument,       NULL, 'r' },
    { "addr",    required_argument, NULL, 'a' },
    { "size",    required_argument, NULL, 's' },
    { "program", required_argument, NULL, 'p' },
    { "fn",      required_argument, NULL, 'f' },
    { "boot",    required_argument, NULL, 'b' },
    { 0, 0, 0, 0 }
};

static const char short_options [] = "hra:s:p:f:";

static void hex_dump(char *buf, int len, int addr)
{
    int i, j, k;
    char binstr[80];

    for (i=0; i < len; i++) {
        if (0 == (i % 16)) {
            sprintf(binstr, "%08x -", i + addr);
            sprintf(binstr, "%s %02x", binstr,(uint8_t)buf[i]);
        } else if (15 == (i % 16)) {
            sprintf(binstr,"%s %02x",binstr,(uint8_t)buf[i]);
            sprintf(binstr,"%s  ",binstr);
            for (j = i - 15; j <= i; j++) {
                sprintf(binstr, "%s%c", binstr,
                       ('!' < buf[j] && buf[j] <= '~') ? buf[j] : '.');
            }
            printf("%s\n", binstr);
        } else {
            sprintf(binstr, "%s %02x", binstr,(uint8_t)buf[i]);
        }
    }
    if (0 != (i % 16)) {
        k = 16 - (i % 16);
        for (j = 0;j < k;j++) {
            sprintf(binstr, "%s   ", binstr);
        }
        sprintf(binstr, "%s  ", binstr);
        k = 16 - k;
        for (j = i-k; j < i; j++) {
            sprintf(binstr,"%s%c", binstr,
                    ('!' < buf[j] && buf[j] <= '~') ? buf[j] : '.');
        }
        printf("%s\n",binstr);
    }
}

static void exitFunc(void)
{
    epld_write(CPLD_SPI_PROG_SEL, 0);
    pflash->close();
}

int main(int argc, char *argv[])
{
    int func_rd = 1, fn = 0;
    char *in_file = NULL;
    uint32_t rd_sz = 0, rd_addr = 0;
    uint32_t wr_sz = 0, wr_addr = 0, erase_sz = 0;
    uint8_t* rd_buff, *wr_buff;
    struct stat fstat;
    uint16_t id;
    char option;
    char *stop = NULL;
    int fd;
    ssize_t f_sz;
    uint8_t boot_reg = 0;
    int erase_type = CHIP_ERASE; //erase whole chip by default

    while((option = getopt_long(argc, argv, short_options,
                              long_options, NULL)) != (char) - 1){
        switch(option){
            case 'h':
                usage(stdout, argc, argv);
                exit(EXIT_SUCCESS);
                break;
            case 'r':
                func_rd = 1; //read
                break;
            case 's':
                rd_sz = strtoul(optarg, &stop, 0);
                break;
            case 'a':
                rd_addr = strtoul(optarg, &stop, 16);
                break;
            case 'f':
                fn = strtoul(optarg, &stop, 0);
                break;
            case 'p':
                func_rd = 0;
                in_file = optarg;
                if(stat(in_file, &fstat)){
                    usage(stdout, argc, argv);
                    exit(-1);
                }
                break;
            default:
                usage(stdout, argc, argv);
                exit(-1);
        }
    }

    switch(fn) {
        case 0:  //cpu master flash
            boot_reg = 0x80;
            erase_type = BLOCK_ERASE_64K;
            printf("\nBuring CPU master Flash\n");
            break;

        case 1:  //cpu bk flash
            boot_reg = 0x40;
            erase_type = BLOCK_ERASE_64K;
            printf("\nBuring CPU backup Flash\n");
            break;

        case 2:  //nps 0
            boot_reg = 0x20;
            //erase_type = BLK_ERASE_256K_3B_MODE;
            printf("\nBuring NPS0 Flash\n");
            break;

        case 3:  //nps 1
            boot_reg = 0x10;
            //erase_type = BLK_ERASE_256K_3B_MODE;
            printf("\nBuring NPS1 Flash\n");
            break;

        case 4:  //nps 1
            boot_reg = 0x1;
            //erase_type = BLK_ERASE_256K_3B_MODE;
            printf("\nBuring NPS1 Flash\n");
            break;

        default:
            exit(-1);
    }
    (void)epld_write(CPLD_SPI_PROG_SEL, boot_reg);

    atexit(exitFunc);
    if(pflash->open()){
        exit(-1);
    }

    //detect twice whether failed in the first attemption
    if(pflash->detect(&id) || \
       pflash->detect(&id)){
        exit(-1);
    }

    printf("Found chip ID: %04x\n", id);

    if(func_rd) {
        rd_buff = calloc(1, rd_sz);
        assert(NULL != rd_buff);
        if(pflash->read(rd_addr, rd_buff, rd_sz)) {
            free(rd_buff);
            exit(-1);
        }
        hex_dump((char*)rd_buff, (int)rd_sz, (int)rd_addr);
        free(rd_buff);
    } else {
        if(stat(in_file, &fstat)){
            perror(in_file);
            exit(-1);
        }

        wr_sz   = fstat.st_size;
        wr_buff = calloc(1, wr_sz);
        assert(NULL != wr_buff);
        fd = open(in_file, O_RDONLY);
        if(fd < 0) {
            free(wr_buff);
            exit(-1);
        }
        f_sz = read(fd, wr_buff, wr_sz);
        assert(f_sz == wr_sz);

        //BLOCK_ERASE_64K
        if(erase_type == BLOCK_ERASE_64K){
            erase_sz = (wr_sz % SZ_64K) ? \
                        wr_sz + (SZ_64K - (wr_sz % SZ_64K)) : wr_sz;
        }

        if(pflash->protect_off_all()                         || \
           pflash->erase(erase_type, wr_addr, erase_sz)      || \
           pflash->program(wr_addr, wr_buff, wr_sz)          || \
           pflash->wdl()){
            free(wr_buff);
            close(fd);
            exit(-1);
        }

        printf("Verify.......\n");
        rd_buff = malloc(wr_sz);
        if(pflash->read(rd_addr, rd_buff, wr_sz)){
            printf("read data from chip error\n");
            free(rd_buff);
            free(wr_buff);
            close(fd);
            exit(-1);
        }
        if (memcmp(wr_buff, rd_buff, wr_sz)) {
            printf("FAILED\n");
        } else {
            printf("PASSED\n\n");
        }
        free(rd_buff);
        free(wr_buff);
    }

    epld_write(CPLD_SPI_PROG_SEL, 0);
    pflash->close();
    close(fd);
    return 0;
}
