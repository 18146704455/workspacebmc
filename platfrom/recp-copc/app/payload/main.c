#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include "si3547.h"

#define ERROR_EXIT   \
{    print_usage();  \
     return -1;      \
}

void print_usage(void)
{
  printf("\nUsage:\n");
  printf("si3547 [rd] [offset]\n");
  printf("si3547 [wr] [offset] [value]\n");
  printf("si3547 [dump] [page_num]\n\n");
}

int main(int argc, char* argv[])
{
    int cmd = CMD_NULL;
    char *stop = NULL;
    unsigned char offset = 0;
    unsigned char value  = 0;
    unsigned char  page   = 0;
    unsigned char *tmp_buf = NULL;
    unsigned int  address = 0;

    /*phrase options*/
    if(argc != 3 && argc != 4) {
        ERROR_EXIT;
    }

    if(strncmp(argv[1], "rd", strlen("rd")) == 0) {
        offset = strtoul(argv[2], &stop, 16);
        if (*stop != '\0' || errno) ERROR_EXIT;
        cmd = CMD_READ;
    } else if(strncmp(argv[1], "wr", strlen("wr")) == 0) {
        if(argc != 4) ERROR_EXIT;
        offset = strtoul(argv[2], &stop, 16);
        if (*stop != '\0' || errno) ERROR_EXIT;
        value = strtoul(argv[3], &stop, 16);
        if (*stop != '\0' || errno) ERROR_EXIT;
        cmd = CMD_WRITE;
    } else if(strncmp(argv[1], "dump", strlen("dump")) == 0) {
        page = strtoul(argv[2], &stop, 16);
        if (*stop != '\0' || errno) ERROR_EXIT;
        cmd = CMD_DUMP;
    } else {
        ERROR_EXIT;
    }

    switch(cmd){
    case CMD_READ:
        si3547_readb(offset, &value);
        printf("[%02x] %02x\n", offset, value);
        break;

    case CMD_WRITE:
        si3547_writeb(offset, value);
        break;

    case CMD_DUMP:
        tmp_buf = calloc(1, PAGE_SZ);
        si3547_dump_page(page, 0, PAGE_SZ, tmp_buf);
        address  = page << 8;
        hex_dump((char*)tmp_buf, PAGE_SZ, address);
        free(tmp_buf);
        break;

    default:
        ERROR_EXIT;
    }

    return 0;
}
