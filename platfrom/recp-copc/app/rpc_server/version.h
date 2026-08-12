#ifndef __VERSION_H
#define __VERSION_H 
#include <stdint.h>
#include <stdbool.h>
  
enum HWID {
    HW_BMC,
    HW_NPS0,
    HW_NPS1,
    HW_TOMHOCK,
    HW_COME,
    HW_MAX
};
enum SWID {
    SW_BMC,
    SW_NPS0,
    SW_NPS1,
    SW_CTR7340,
    SW_COME,
    SW_MAX
};
#define SN_SIZE 16
#define HW_SIZE 32
#define SW_SIZE 32
#define PATH_SIZE 256

int8_t get_board_id(void);
int8_t get_slot_id(void);
enum HWID cpuid2int(char *name);
const char *cpuid2str(enum HWID id);
int set_eeprom_file(char *file);
int init_eeprom(void);
int save_eeprom(void);

// sn is char * , in deal_* use strlen(sn)
int deal_SN(uint8_t *sn,bool getset);
int deal_HW(uint8_t *fw, enum HWID cpuid, bool getset);
int deal_SW(uint8_t *sw, enum SWID swid,bool getset);
const char *get_board_name(void);
int set_board_name(const char *);
#endif /* ifndef __VERSION_H */


