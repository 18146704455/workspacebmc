/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-09-27 15:40:25
 * @LastEditors: smith.zj
 * @LastEditTime: 2019-09-27 15:51:30
 */
#ifndef __BOARD_CTRL_H__
#define __BOARD_CTRL_H__
#include "cpld_lc_addr.h"

typedef enum _NPS_iD {
	NPS_ID_0,
	NPS_ID_1,
	NPS_ID_ALL,
	NPS_ID_INVALID,
}NPS_iD;

typedef enum _NPS_RST_CTRL {
	NPS_CTRL_HW_RST,
	NPS_CTRL_PO_RST,
    NPS_CTRL_INVALID,   
}NPS_RST_CTRL;



typedef enum _TCAM_ID {
	TCAM_ID_0,
	TCAM_ID_1,
	TCAM_ID_ALL,
	TCAM_ID_INVALID,
}TCAM_ID;

typedef enum _TCAM_RST_CTRL {
	TCAM_CTRL_C_RST,
	TCAM_CTRL_S_RST,
    TCAM_CTRL_INVALID,   
}TCAM_RST_CTRL;

typedef enum _GEARBOX_ID{
    GEARBOX_ID_0,
    GEARBOX_ID_1,
    GEARBOX_ID_ALL,
    GEARBOX_ID_INVALID,
}GEARBOX_ID;
typedef enum _GEARBOX_RST_CTRL {
	GEARBOX_CTRL_RST,
    GEARBOX_CTRL_INVALID,   
}GEARBOX_RST_CTRL;
typedef enum _CPU_CTRL {
    CPU_CTRL_STOP,
    CPU_CTRL_START,
    CPU_CTRL_RESTART,
    CPU_CTRL_INVALID,
} CPU_CTRL;

/**
 * release the nps
 *
 * @param nps_id nps index
 *
 * @return 0 on success or -1 otherwise.
 */

int nps_release_po_rst(NPS_iD nps_id);
int nps_release_hw_rst(NPS_iD nps_id);	
int nps_reset_po_rst(NPS_iD nps_id);
int nps_reset_hw_rst(NPS_iD nps_id);	
int nps_reset_all_po_rst();	
int nps_reset_all_hw_rst();	
int nps_release_all_po_rst();
int nps_release_all_hw_rst();

int tcam_ctrl_reset(TCAM_ID,TCAM_RST_CTRL,bool);
#define tcam_release_c_rst(tcam_id) tcam_ctrl_reset(tcam_id,TCAM_CTRL_C_RST,true)
#define tcam_release_s_rst(tcam_id) tcam_ctrl_reset(tcam_id,TCAM_CTRL_S_RST,true)
#define tcam_reset_s_rst(tcam_id) tcam_ctrl_reset(tcam_id,TCAM_CTRL_S_RST,false)
#define tcam_reset_c_rst(tcam_id) tcam_ctrl_reset(tcam_id,TCAM_CTRL_C_RST,false)
#define tcam_release_all_s_rst() tcam_ctrl_reset(TCAM_ID_ALL,TCAM_CTRL_S_RST,true)
#define tcam_release_all_c_rst() tcam_ctrl_reset(TCAM_ID_ALL,TCAM_CTRL_C_RST,true)
#define tcam_reset_all_s_rst() tcam_ctrl_reset(TCAM_ID_ALL,TCAM_CTRL_S_RST,false)
#define tcam_reset_all_c_rst() tcam_ctrl_reset(TCAM_ID_ALL,TCAM_CTRL_C_RST,false)

#define gearbox_release_res(gearbox_id) gearbox_reset_ctrl(gearbox_id,GEARBOX_CTRL_RST,true)
#define gearbox_reset_res(gearbox_id) gearbox_reset_ctrl(gearbox_id,GEARBOX_CTRL_RST,false)

#define gearbox_release_all_res() gearbox_reset_ctrl(GEARBOX_ID_ALL,GEARBOX_CTRL_RST,true)
#define gearbox_reset_all_res() gearbox_reset_ctrl(GEARBOX_ID_ALL,GEARBOX_CTRL_RST,false)

int board_init();

#endif /* LIB_I2C_SMBUS_H */
