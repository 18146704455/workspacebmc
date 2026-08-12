#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

#define ERROR_EXIT   \
{    print_usage();  \
     return -1;      \
}

extern int epld_read(uint32_t offset, uint8_t* value);
extern int epld_write(uint32_t offset, uint8_t value);

void print_usage(void)
{
  printf("\nUsage:\n");
  printf("epld [rd] [offset]\n");
  printf("epld [wr] [offset] [value]\n\n");
}

int main(int argc, char* argv[])
{
    int ret = 0;
    int cmd_rd = 1;
    char *stop = NULL;
    unsigned int offset = 0;
    unsigned int value  = 0;

    /*phrase options*/
    if(argc != 3 && argc != 4) {
        ERROR_EXIT;
    }

    if(strncmp(argv[1], "rd", strlen("rd")) != 0) {
        if(strncmp(argv[1], "wr", strlen("wr")) != 0) {
            ERROR_EXIT;
        }

        cmd_rd = 0;
        if(argc != 4) ERROR_EXIT;

        value = strtoul(argv[3], &stop, 16);
        if (*stop != '\0' || errno) {
            ERROR_EXIT;
        }
    } else if (argc != 3) {
        ERROR_EXIT;
    }

    offset = strtoul(argv[2], &stop, 16);
    if (*stop != '\0' || errno) {
        ERROR_EXIT;
    }

    if(cmd_rd) {
        ret = epld_read(offset, (uint8_t*)&value);
    } else {
        ret = epld_write(offset, value);
    }
    if(!ret)
    {
        if(cmd_rd) printf("[%04x] %02x\n", offset, value);
    }else{
        fprintf(stderr,"cpld error code %d",ret);
    }

    return ret;
}
