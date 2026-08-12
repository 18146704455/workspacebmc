#ifndef __COMMON_H__
#define __COMMON_H__


#define DB_DEBUG  4
#define DB_INFO   3
#define DB_ERR    2
#define DB_FATAL  1
#define DB_CURR_LEVEL DB_DEBUG

#define DB_PRINT(level, s, params...)   do{ if(level <= DB_CURR_LEVEL)  printf("[%s, %d]: " s "\n", __func__, __LINE__, ##params);}while(0)


#endif