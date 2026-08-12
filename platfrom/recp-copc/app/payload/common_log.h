/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-03-13 10:15:21
 * @LastEditors: smith.zj
 * @LastEditTime: 2019-10-23 20:23:52
 */
#ifndef __COMMON_H__
#define __COMMON_H__
#include <stdio.h>
#include <syslog.h>


#define  DP_DEBUG
#if (defined(DP_DEBUG))
#define PRINT(format, ...) printf("*"format, ##__VA_ARGS__)
#else
#define PRINT(format, ...)
#endif

#define TRACE  PRINT("%s:%d\n",__func__,__LINE__);

/*#define LOG_TO_FILE*/
/*#define LOG_TO_SYSLOG*/
#define LOG_TO_TTY
/*#define LOG_TO_ZLOG*/
#define NEED_SWAP
#ifdef NEED_SWAP
#define Swap32(l) ( ((l) >> 24) | (((l) & 0x00ff0000) >> 8)  | (((l) & 0x0000ff00) << 8)  | ((l) << 24) )  
#define Swap16(l) ( ((l) >> 8) |  (((l) & 0xff) << 8) ) 
#else
#define Swap32(l) (l)
#define Swap16(l) (l)
#endif

#ifdef LOG_TO_ZLOG
extern zlog_category_t *glog;
#define ZLOG_CFG_FILE "/etc/sysconfig/zlog.ini"
#endif


#define DB_DEBUG  5
#define DB_INFO   4
#define DB_WARN   3
#define DB_ERR    2
#define DB_FATAL  1
#define DB_CURR_LEVEL DB_DEBUG

#ifdef LOG_TO_FILE
#define LOG_FILE_PATH "/tmp/fmbmc.log"
extern FILE *log_file;
#define DB_PRINT(level, s, params...)   do{ if(level <= DB_CURR_LEVEL)  fprintf(log_file,"[%s, %d]: " s "\n", __func__, __LINE__, ##params);}while(0)
#endif

#ifdef LOG_TO_SYSLOG
#define DB_PRINT(level, s, params...)   do{ if(level <= DB_CURR_LEVEL)  syslog(LOG_EMERG,"[%s, %d]: " s "\n", __func__, __LINE__, ##params);}while(0)
#endif

#ifdef LOG_TO_TTY
#define DB_PRINT(level, s, params...)   do{ if(level <= DB_CURR_LEVEL)  printf("[%s, %d]: " s , __func__, __LINE__, ##params);}while(0)
#endif

#ifdef LOG_TO_ZLOG
#define DB_PRINT(level, s, params...)   do{ if(level <= DB_CURR_LEVEL)  zlog_info(glog,"[%s, %d]: " s "\n", __func__, __LINE__, ##params);}while(0)
#endif




#endif
