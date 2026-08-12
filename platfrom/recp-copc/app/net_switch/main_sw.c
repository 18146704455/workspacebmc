#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

extern int fabric_switch_write(uint8_t dev,uint8_t reg,uint16_t val);
extern int fabric_switch_read(uint8_t dev,uint8_t reg,uint16_t *val);
extern int fabric_counter(uint8_t port, uint16_t mode, bool clear,uint32_t *cnt);



#define ERROR_EXIT                                            \
{                                                              \
    printf("\nsw rd portaddr regaddr\n\n"); \
    printf("sw wr portaddr regaddr regval\n\n"); \
    printf("sw cnt port mode [clear]\n\n"); \
    return -1;                                                 \
}

int main(int argc, char*argv[])
{
    char *stop = NULL;
    uint8_t portaddr = -1;
    uint8_t regaddr = -1;

    uint16_t val = 0;
    int ret;
	uint32_t cnt = 0;
	uint8_t clear = 0;
    
    /*phrase options*/
    if(argc != 4 && argc != 5) {
        ERROR_EXIT;
    }

    portaddr = strtoul(argv[2], &stop, 16);
    if (*stop != '\0' || errno) {
        ERROR_EXIT;
    }

    regaddr = strtoul(argv[3], &stop, 16);
    if (*stop != '\0' || errno) {
        ERROR_EXIT;
    }


    if(strncmp(argv[1], "rd", strlen("rd")) == 0) {
        ret = fabric_switch_read(portaddr,regaddr,&val);
        if(ret != 0) {
            printf("read port 0x%x [0x%x] failed\n",portaddr,regaddr);
            return -1;
        }
        printf("[0x%x|0x%x] 0x%x\n",portaddr,regaddr,val);
    } else if(strncmp(argv[1], "wr", strlen("wr")) == 0) {
        val = strtoul(argv[4], &stop, 16);
        if (*stop != '\0' || errno) {
            ERROR_EXIT;
        }
        
        ret = fabric_switch_write(portaddr,regaddr,val);
        if(ret != 0) {
            printf("write port  [0x%x|0x%x] 0x%x failed\n",portaddr,regaddr,val);
            return -1;
        }

        printf("write [0x%x|0x%x] 0x%x success\n",portaddr,regaddr,val);
    } else if(strncmp(argv[1], "cnt", strlen("wr")) == 0) {
		if(argc == 5) {
			clear = strtoul(argv[4], &stop, 16);
	        if (*stop != '\0' || errno) {
	            ERROR_EXIT;
	        }
		}
        
        ret = fabric_counter(portaddr,regaddr,(bool)clear,&cnt);
        if(ret != 0) {
            printf("fabric_counter  [0x%x|0x%x] clear %x failed\n",portaddr,regaddr,clear);
            return -1;
        } else {
			printf("[0x%x] 0x%x\n",portaddr,cnt);
		}
	} else {
        ERROR_EXIT;
    }


    return 0;
}
