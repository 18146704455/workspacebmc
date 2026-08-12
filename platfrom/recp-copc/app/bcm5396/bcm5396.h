#ifndef __BCM5396_H__
#define __BCM5396_H__

extern int bcm5389_read(unsigned char  page, unsigned char reg,
                        unsigned char* byte, unsigned char size);

extern int bcm5389_write(unsigned char  page, unsigned char reg,
                         unsigned char* byte, unsigned char size);
#endif
