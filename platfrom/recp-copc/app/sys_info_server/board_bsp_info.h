/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @LastEditors: smith.zj
 */
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define DEF_EEPROM_FILE "/sys/bus/i2c/devices/i2c-0/0-0050/eeprom"

int init_bsp_info(char *eepromfile);

int get_boardtype_string(char *boardType);
uint16_t get_boardtype_id();
uint16_t get_board_slot();
int get_board_sn(char *sn);
int get_board_pn(char *pn);

int ctrl_led_status(uint8_t submoduleSlot, SERVICE_STATUS_E serviceStatus);