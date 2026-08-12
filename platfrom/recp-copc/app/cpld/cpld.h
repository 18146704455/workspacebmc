#ifndef __CPLD_H
#define __CPLD_H 
#include <stdint.h>
 
//int cpld_write_byte(unsigned short ,unsigned char);
//int cpld_read_byte(unsigned short, unsigned char *); 
int epld_read(uint32_t , uint8_t *);
int epld_write(uint32_t, uint8_t );
#define cpld_read_byte(reg,val) epld_read(reg,val)
#define cpld_write_byte(reg,val) epld_write(reg, val)
#endif /* ifndef __CPLD_H */

