#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern void hex_dump(char *buf, int len, int addr)
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
