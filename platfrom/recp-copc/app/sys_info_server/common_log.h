/*************************************************************************
  > File Name: common_log.h
  > Author: 
  > Mail: 
  > Created Time: Mon 30 Dec 2019 09:46:52 AM CST
 ************************************************************************/

#ifndef _COMMON_LOG_H
#define _COMMON_LOG_H
#include <stdio.h>
#include <syslog.h>
 
#define DB_DEBUG  5
#define DB_INFO   4
#define DB_WARN   3
#define DB_ERR    2
#define DB_FATAL  1
#define DB_CURR_LEVEL DB_DEBUG
#define DB_LOG  DB_CURR_LEVEL


#define __PRINT(level, s, params...)  do{ if(level <= DB_CURR_LEVEL)  fprintf(stderr,"[%s, %d]: " s "\n", __func__, __LINE__, ##params);}while(0)
#define DB_PRINT(level, s, params...) __PRINT(level,s,##params)
#define pr_debug(s,params...) __PRINT(DB_DEBUG,s,##params)
#define pr_info(s,params...) __PRINT(DB_INFO,s,##params)
#define pr_warn(s,params...) __PRINT(DB_WARN,s,##params)
//#define pr_err(s,params...) __PRINT(DB_ERR,s,##params)
#define pr_err(s,params...) syslog(LOG_ERR,"[%s, %d]: " s "\n", __func__, __LINE__, ##params);
#define pr_fatal(s,params...) __PRINT(DB_FATAL,s,##params)
//#define pr_log(s,params...) __PRINT(DB_LOG,s,##params)
#define pr_log(s,params...) syslog(LOG_INFO,"[%s, %d]: " s "\n", __func__, __LINE__, ##params);

#define FUNC_START() pr_debug("start")
#define FUNC_END() pr_debug("end")
#endif
