/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @LastEditors: smith.zj
 */
#ifndef __VERSION_H
#define __VERSION_H 
#include <stdint.h>
#include <stdbool.h>
 const char *eeprom_parser_boardname();
int set_board_name(const char *board_name);

const char *eeprom_parser_boardtype();

int init_eeprom(char *eepromfile);
void deinit_eeprom();
int eeprom_parser_sn(char *sn);
int eeprom_parser_pn(char *pn);

#endif /* ifndef __VERSION_H */


