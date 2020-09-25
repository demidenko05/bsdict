/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ logger library. A file is the best alternative to output logs.
 * If a log file exceed max size, then it will be overwritten to avoid "out of order" cause huge log files.
 * It is designed for using multiply log-files. The first log file will be used as main log when client didn't choose file.
 * This is fault-tolerated log, so wrong initialized log will print into stdout/err.
 * </p>
 * @author Yury Demidenko
 **/

#ifndef BSLOG_EXT
#include "stdbool.h"
#include "stdio.h"
#include "errno.h"

#include "BsBase.h"

#define BSLOG_EXT "log"
#define BSLOG_EXT_SECOND "lg2"

//Data:

/**
 * <p>Log file bundle partly initialized by client:
 * 1. path - initialized by client, must have extension "log"!
 *     If file "log" size excesses maximum, then it will overwrite second file with extension "lg2".
 *     If file "lg2" size excesses maximum, then it will overwrite first file with extension "log".
 * 2. file - opened by this logger file to append or to overwrite if its size excesses maximum
 * </p>
 **/
typedef struct {
  char *pth; //
  FILE *file; //file
} BsLogFile;

/**
 * <p>Log files array initialized by client:
 * 1. size - initialized by client.
 * 2. files - initialized by client only with "path".
 * </p>
 **/
typedef struct {
  int size; //array size
  BsLogFile **files; //array of files
} BsLogFiles;

/* Log data constant for clients */
typedef struct {
  int file_index; //index of log file
  char *tag; //it's usually C file name
} BsLogConst;

/* Log level or print only message without date, thread#, tag and level */
typedef enum {
  BSLDEBUG, BSLINFO, BSLWARN, BSLERROR, BSLFATAL, BSLTEST,
  BSLONLYMSG //print only message without date, thread#, tag and level
} BsELogLevOmsg;

/**
 * <p>Make log message into the first log file.</p>
 * @param p_levomsg Log level or print only message without date, thread#, tag and level
 * @param p_fmt message or format
 **/
void bslog_log(BsELogLevOmsg p_levomsg, char *p_fmt, ...);

#define BSLOG_LOG(p_levomsg, p_fmt, ...) bslog_log(p_levomsg, "\n  %s:%s:%d, " p_fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__);

//Report error with only errno
#define BSLOG_ERR bslog_log(BSLERROR, "\n  %s:%s:%d\n", __FILE__, __func__, __LINE__);

//Generic method returning error macro wrappers:
#define BS_DO_ERR(pInvoked) pInvoked; if (errno != 0) { BSLOG_ERR }
#define BS_DO_E_RET(pInvoked) pInvoked; if (errno != 0) { BSLOG_ERR return; }
#define BS_DO_E_RETF(pInvoked) pInvoked; if (errno != 0) { BSLOG_ERR return FALSE; }
#define BS_DO_E_RETE(pInvoked) pInvoked; if (errno != 0) { BSLOG_ERR return errno; }
#define BS_DO_E_RETN(pInvoked) pInvoked; if (errno != 0) { BSLOG_ERR return NULL; }
#define BS_DO_E_OUT(pInvoked) pInvoked; if (errno != 0) { BSLOG_ERR goto out; }
#define BS_DO_E_OUTE(pInvoked) pInvoked; if (errno != 0) { BSLOG_ERR goto oute; }
#define BS_DO_E_GOTO(pInvoked, pLbl) pInvoked; if (errno != 0) { BSLOG_ERR goto pLbl; }
#define BS_DO_E_CONT(pInvoked) pInvoked; if (errno != 0) { BSLOG_ERR continue; }
#define BS_DO_E_BREAK(pInvoked) pInvoked; if (errno != 0) { BSLOG_ERR break; }
//Generic method returning error macro wrappers with clearing errno on start
//e.g. GTK leaves errno non-cleared:
#define BS_DO_CEERR(pInvoked) errno = 0; pInvoked; if (errno != 0) { BSLOG_ERR }
#define BS_DO_CEE_RET(pInvoked) errno = 0; pInvoked; if (errno != 0) { BSLOG_ERR return; }
#define BS_DO_CEE_RETF(pInvoked) errno = 0; pInvoked; if (errno != 0) { BSLOG_ERR return FALSE; }
#define BS_DO_CEE_RETE(pInvoked) errno = 0; pInvoked; if (errno != 0) { BSLOG_ERR return errno; }
#define BS_DO_CEE_RETN(pInvoked) errno = 0; pInvoked; if (errno != 0) { BSLOG_ERR return NULL; }
#define BS_DO_CEE_OUT(pInvoked) errno = 0; pInvoked; if (errno != 0) { BSLOG_ERR goto out; }
#define BS_DO_CEE_OUTE(pInvoked) errno = 0; pInvoked; if (errno != 0) { BSLOG_ERR goto oute; }
#define BS_DO_CEE_GOTO(pInvoked, pLbl) errno = 0; pInvoked; if (errno != 0) { BSLOG_ERR goto pLbl; }
#define BS_DO_CEE_CONT(pInvoked) errno = 0; pInvoked; if (errno != 0) { BSLOG_ERR continue; }
#define BS_DO_CEE_BREAK(pInvoked) errno = 0; pInvoked; if (errno != 0) { BSLOG_ERR break; }

#define BS_IF_EN_RET(pCond,pErrno) if (pCond) { if (errno==0) { errno=pErrno; } BSLOG_ERR return; }
#define BS_IF_EN_RETE(pCond,pErrno) if (pCond) { if (errno==0) { errno=pErrno; } BSLOG_ERR return errno; }
#define BS_IF_EN_RETN(pCond,pErrno) if (pCond) { if (errno==0) { errno=pErrno; } BSLOG_ERR return NULL; }
#define BS_IF_EN_OUT(pCond,pErrno) if (pCond) { if (errno==0) { errno=pErrno; } BSLOG_ERR goto out; }
#define BS_IF_EN_OUTE(pCond,pErrno) if (pCond) { if (errno==0) { errno=pErrno; } BSLOG_ERR goto oute; }
#define BS_IF_EN_GO(pCond,pErrno,pLbl) if (pCond) { if (errno==0) { errno=pErrno; } BSLOG_ERR goto pLbl; }

#define BS_IF_ENM_RET(pCond,pErrno,pMsg) if (pCond) { if (errno==0) { errno=pErrno; } BSLOG_LOG(BSLERROR,pMsg) return; }
#define BS_IF_ENM_RETE(pCond,pErrno,pMsg) if (pCond) { if (errno==0) { errno=pErrno; } BSLOG_LOG(BSLERROR,pMsg) return errno; }
#define BS_IF_ENM_RETN(pCond,pErrno,pMsg) if (pCond) { if (errno==0) { errno=pErrno; } BSLOG_LOG(BSLERROR,pMsg) return NULL; }
#define BS_IF_ENM_OUT(pCond,pErrno,pMsg) if (pCond) { if (errno==0) { errno=pErrno; } BSLOG_LOG(BSLERROR,pMsg) goto out; }
#define BS_IF_ENM_OUTE(pCond,pErrno,pMsg) if (pCond) { if (errno==0) { errno=pErrno; } BSLOG_LOG(BSLERROR,pMsg) goto oute; }
#define BS_IF_ENM_GO(pCond,pErrno,pMsg,pLbl) if (pCond) { if (errno==0) { errno=pErrno; } BSLOG_LOG(BSLERROR,pMsg) goto pLbl; }

/**
 * <p>Debug sub range model.</p>
 * @member int floor
 * @member int ceiling
 **/
typedef struct {
  int floor;
  int ceiling;
} BsLogDbgRange;

/**
 * <p>Debug sub ranges.</p>
 * @member int bsize - buffer size more than 0
 * @member int size - size from 0
 * @member BsLogDbgRange **vals - array of objects
 **/
typedef struct {
  int bsize;
  int size;
  BsLogDbgRange **vals;
} BsLogDbgRanges;

/**
 * <p>Add object to the first null cell of collection.
 * It increases collection if it's full-filled.</p>
 * @param pSet - collection
 * @param pObj - object
 * @param pInc - how much realloc when adding into
 *   full-filled collection, if BS_IDX_0 or less then error
 **/
void bslogdbgranges_add_inc(BsLogDbgRanges *pSet, BsLogDbgRange *pObj, int pInc);

  //Initialization by main client:

    //1. Settings:
/**
 * <p>Get registered debug ranges.
 * Usually for test purposes.</p>
 * @return registered debug ranges, maybe NULL
 **/
BsLogDbgRanges *bslog_get_dbgranges();

/**
 * <p>Add debug range. Non-synchronized!!!</p>
 * @param p_floor floor
 * @param p_ceiling ceiling
 **/
void bslog_add_dbgrange(int p_floor, int p_ceiling);

/**
 * <p>Set main range debug level floor.</p>
 * @param p_levomsg - level
 **/
void bslog_set_debug_floor(int p_levomsg);

/**
 * <p>Get main range debug level floor.</p>
 * @return level floor
 **/
int bslog_get_debug_floor();

/**
 * <p>Set main range debug level ceiling.</p>
 * @param p_levomsg - level
 **/
void bslog_set_debug_ceiling(int p_levomsg);

/**
 * <p>Get main range debug level ceiling.</p>
 * @return debug level ceiling.
 **/
int bslog_get_debug_ceiling();

/**
 * <p>Set maximum log file size.</p>
 * @param pSize - size
 **/
void bslog_set_maximum_size(BS_FOFST_T pSize);

/**
 * <p>Get maximum log file size.</p>
 * @return maximum log file size.
 **/
BS_FOFST_T bslog_get_maximum_size();

    //Initialization:
/**
 * <p>Invoked by main client constructor.</p>
 * @param pSize size of array
 * @return Log files array or NULL.
 * @set errno if error.
 **/
BsLogFiles *bslogfiles_new(int pSize);

/**
 * <p>Destructor.</p>
 * @param pLogFls - files array
 * @return always NULL
 **/
BsLogFiles *bslogfiles_free(BsLogFiles *pLogFls);

/**
 * <p>Invoked by main client initializer of file path in given array's file.</p>
 * @param pLogFls - files array
 * @param pIdx - file index
 * @param p_path - file path
 * @set errno if error.
 **/
void bslog_files_set_path(BsLogFiles *pLogFls, int pIdx, char *p_path);

/**
 * <p>Invoked by main client initializer.</p>
 * @param pLogFls only with paths
 * @set errno if error.
 **/
void bslog_init(BsLogFiles *pLogFls);

/**
 * <p>Check log constant after logging initializing.</p>
 * @param pLgCnst pointer to log constant data
 * @set errno if error.
 **/
void bslogconst_check(BsLogConst *pLgCnst);

  //Destroying by main client:
/**
 * <p>Free memory, closing files on main program exit.</p>
 **/
void bslog_destroy(void);

  //Using by clients (C-files libs):

/**
 * <p>Make log message.</p>
 * @param pLgCnst pointer to log constant data
 * @param p_levomsg Log level or print only message without date, thread#, tag and level
 * @param p_fmt message or format
 **/
void bslog_log_to(BsLogConst *pLgCnst, BsELogLevOmsg p_levomsg, char *p_fmt, ...);

#define BSLOG_LOG_TO(pLgCnst, p_levomsg, p_fmt, ...) bslog_log_to(pLgCnst, p_levomsg, "\n  %s:%s:%d, " p_fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__);

//Report error with only errno
#define BSLOG_ERR_TO(pLgCnst) bslog_log_to(pLgCnst, BSLERROR, "\n..%s:%s:%d\n", __FILE__, __func__, __LINE__);

/**
 * <p>Checker if show debug messages for given level.
 * This is for high performance and best filtering of debug messages.
 * Example:
 * <pre>
 * if (bslog_is_debug(12786) {
 *   //a lot of job to make message:
 *   ...
 *   bslog_log_to(s_lconst, BSLDEBUG, msg);
 * }
 * </pre>
 * </p>
 * @param p_levomsg int - debug level
 * @return if debug.
 **/
bool bslog_is_debug(int p_levomsg);

#endif
