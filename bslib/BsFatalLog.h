/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ fatal errors(signals) handling and logging library.</p>
 * @author Yury Demidenko
 **/
#ifndef BSERRORLOG_H
#define BSERRORLOG_H 1

#include "BsLog.h"

  //public lib:
/**
 * <p>Initialize interception of fatal signals.</p>
 **/
void bsfatallog_init_fatal_signals(void);

/**
 * <p>Set logging instrument.</p>
 * @param pLgCnst pointer to log constant data
 **/
void bsfatallog_set_logconst(BsLogConst *pLgCnst);

/**
 * <p>Set fatal log's path for method number 0-backtrace_symbols_fd.</p>
 * @param pFatLogPth path
 **/
void bsfatallog_set_log_path(char *pFatLogPth);

/**
 * <p>Set backtrace method number 0-backtrace_symbols_fd into separate fatal log,
 * 1-logging of set of BT addresses into BSLOG.</p>
 * @param pUseBtNu method number
 **/
void bsfatallog_set_use_bt_num(int pUseBtNu);

/**
 * <p>Set fatal destructor.</p>
 * @param pFatDestr fatal destructor
 **/
void bsfatallog_set_fatal_destructor(BsVoid_Method *pFatDestr);
#endif
