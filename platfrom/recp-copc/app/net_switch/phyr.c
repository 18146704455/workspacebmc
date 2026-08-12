#include <stdio.h>
#include <stdlib.h>

extern int phy_read(char busno, char phyaddr, char regaddr, unsigned short* val);

#define PRINT_USAGE                                     \
{                                                       \
    printf("\nphyr  macno[0|1]  phyaddr  regaddr\n\n"); \
    return 1;                                           \
}

int main(int argc, char*argv[])
{
    char macno, phyaddr;
    unsigned short regaddr, val = 0xffff;
    char *stop1 = NULL, *stop2 = NULL, *stop3 = NULL;

    if(argc != 4)
        PRINT_USAGE;

    macno   = (char)strtoul(argv[1], &stop1, 16);
    phyaddr = (char)strtoul(argv[2], &stop2, 16);
    regaddr = (char)strtoul(argv[3], &stop3, 16);
    if(('\0' != *stop1)               || \
       ('\0' != *stop2)               || \
       ('\0' != *stop3)               || \
       ((0 != macno) && (1 != macno)) || \
       (regaddr > 0x20))
        PRINT_USAGE;

    if(0 != phy_read(macno, phyaddr, regaddr, &val)){
        printf("phy_read error: macno %x, phyaddr %x, regaddr %x\n",
                macno, phyaddr, regaddr);
        return 2;
    }

    printf("[%04x] %04x\n", regaddr, val);
    return 0;
}
