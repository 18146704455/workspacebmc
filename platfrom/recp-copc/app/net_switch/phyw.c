#include <stdio.h>
#include <stdlib.h>

extern int phy_write(char busno, char phyaddr, char regaddr, unsigned short val);

#define PRINT_USAGE                                            \
{                                                              \
    printf("\nphyw  macno[0|1]  phyaddr  regaddr regval\n\n"); \
    return -1;                                                 \
}

int main(int argc, char*argv[])
{
    char macno, phyaddr;
    unsigned short regaddr, regval;
    char *stop1 = NULL, *stop2 = NULL;
    char *stop3 = NULL, *stop4 = NULL;

    if(argc != 5)
        PRINT_USAGE;

    macno   = (char)strtoul(argv[1], &stop1, 16);
    phyaddr = (char)strtoul(argv[2], &stop2, 16);
    regaddr = (char)strtoul(argv[3], &stop3, 16);
    regval  = (unsigned short)strtoul(argv[4], &stop4, 16);
    if(('\0' != *stop1)               || \
       ('\0' != *stop2)               || \
       ('\0' != *stop3)               || \
       ('\0' != *stop4)               || \
       ((0 != macno) && (1 != macno)) || \
       (regaddr > 0x20))
        PRINT_USAGE;

printf("regval %x\n",regval);
    if(0 != phy_write(macno, phyaddr, regaddr, regval)){
        printf("phy_write error: macno %x, phyaddr %x, regaddr %x, regval %x\n",
                macno, phyaddr, regaddr, regval);
        return -1;
    }

    return 0;
}
