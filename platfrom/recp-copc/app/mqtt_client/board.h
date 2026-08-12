/*************************************************************************
  > File Name: board.h
  > Author: 
  > Mail: 
  > Created Time: Fri 17 Jan 2020 01:16:50 PM CST
 ************************************************************************/

#ifndef _BOARD_H
#define _BOARD_H
#include <base.h>
#include <cJSON.h>

#define MATCH_ID_PSU_BOADRD   "PSU_board"
#define MATCH_ID_PSU_FCB      "PSU_FCB"
#define MATCH_ID_TEMPERATURE_BOARD  "temperature-board"
#define MATCH_ID_TEMPERATURE_FCB    "temperature-FCB"

struct board_device{
  char* match_id;
  char *board_type;
  int slot ;
  char *hw;
  char *sw;
  char *cpuId;
  char *sn;
  char *def_powr;
  int online;
  struct list_head node_root;
  struct device dev;
};
int reg_board(struct def* , struct board_device *);


//函数实体不在board.c
void BuildChipMsg(cJSON *root);

#endif
