/*************************************************************************
  > File Name: board.c
  > Author: 
  > Mail: 
  > Created Time: Wed 15 Jan 2020 05:41:52 PM CST
 ************************************************************************/
#include <cpld.h>
#include "list.h"
#include "base.h"
#include "config.h"
#include <stdint.h>
#include <board.h>
  
#if 1
int  reg_board(struct def *config, struct board_device *board)
{
    //match 
    list_add(&board->node_root,&config->board_root);

}
#endif


