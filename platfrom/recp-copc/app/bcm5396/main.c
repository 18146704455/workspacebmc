#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

extern int bcm5389_read(uint8_t page, uint8_t reg,  uint8_t* byte, uint8_t n_bytes);
extern int bcm5389_write(uint8_t page, uint8_t reg, uint8_t* byte, uint8_t n_bytes);

#define PRINT_USAGE_EXIT                                 \
{                                                        \
    printf("\n\tbcmspi r page register nbytes\n");       \
    printf("\tbcmspi w page register value nbytes\n\n"); \
    return (-1);                                         \
}

int main(int argc, char* argv[])
{
    int CMD_R = 0, i = 0;
    uint8_t page = 0, reg = 0, n_bytes = 0;
    uint64_t val;
    char*stop1, *stop2, *stop3, *stop4;
    uint8_t tmp[64] = {0};

    /*phrase command line*/
    if(argc != 5 && argc != 6) {
        PRINT_USAGE_EXIT;
    }else if(strncasecmp(argv[1], "r", strlen("r")) == 0 && argc == 5){
        page    = strtoul(argv[2], &stop1, 16) & 0xff;
        reg     = strtoul(argv[3], &stop2, 16) & 0xff;
        n_bytes = strtoul(argv[4], &stop3, 16) & 0xff;
        if((*stop1 != '\0') || (*stop2 != '\0') || \
           (*stop3 != '\0') || (n_bytes > 8))
            PRINT_USAGE_EXIT;
        CMD_R = 1;
    }else if(strncasecmp(argv[1], "w", strlen("w")) == 0 && argc == 6){
        page    = strtoul(argv[2],  &stop1, 16) & 0xff;
        reg     = strtoul(argv[3],  &stop2, 16) & 0xff;
        val     = strtoull(argv[4], &stop4, 16);
        n_bytes = strtoul(argv[5],  &stop3, 16) & 0xff;
        if((*stop1 != '\0') || (*stop2 != '\0') || \
           (*stop3 != '\0') || (*stop4 != '\0') || \
           (n_bytes > 8))
            PRINT_USAGE_EXIT;
    }else{
        PRINT_USAGE_EXIT;
    }

    CMD_R ? bcm5389_read(page, reg,  tmp,            n_bytes) : \
            bcm5389_write(page, reg, (uint8_t*)&val, n_bytes);

    if(CMD_R){
        for(i = 0; i < n_bytes; i++)
            printf("%02x", tmp[n_bytes - i - 1]);
        printf("\n");
    }

    return 0;
}
