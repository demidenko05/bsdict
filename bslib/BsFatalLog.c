/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ fatal errors(signals) handling and logging library.</p>
 * @author Yury Demidenko
 **/

#include "signal.h"
#include "stdlib.h"
#include "string.h"
#include "execinfo.h"
#include "fcntl.h"
#include "unwind.h"

#include "BsFatalLog.h"

typedef struct {
  void **array;
  int size;
} ArrPtrs;

static int sUseBtNu = 1; 

#define BSLFTAG_SIZE 100

static char sLogTag[BSLFTAG_SIZE];

static BsLogConst sLgCnst = { .file_index=-1, .tag=sLogTag }; 

static char *sFatLogPth = "bssegfault.log"; 

static BsVoid_Method *sFatDestr = NULL;

//Methods:

static _Unwind_Reason_Code s_backtrace_consm(struct _Unwind_Context *p_ctx, void *p_arg) {
  ArrPtrs *arr = (ArrPtrs*) p_arg;
  int i = 0;
  void *addr = (void*) _Unwind_GetIP(p_ctx);
  if (addr != NULL) {
    for (i = 0; i < arr->size; i++) {
      if (arr->array[i] == NULL) {
        if (i == 0 || arr->array[i - 1] != addr) { //recursive filter
          arr->array[i] = addr;
        }
        break;
      }
    }
  }
  if (i == arr->size - 1) {
    return _URC_END_OF_STACK;
  }
  return _URC_NO_REASON;
}

static void sf_backtrace(void **array, int size) {
  ArrPtrs arg = { .array = array, .size = size };
  _Unwind_Backtrace(s_backtrace_consm, &arg);
}

static void sf_bt0(void) {
  int fd = open(sFatLogPth, O_WRONLY | O_CREAT, 0666);
  if (fd == -1) {
    fd = 2;
  }
  void **arr = alloca(50 * sizeof(void*));
  if (arr == NULL) {
    bslog_log_to(&sLgCnst, BSLFATAL, "sf_bt0: Can't alloca BT array!\n");
    return;
  }
  bslog_log_to(&sLgCnst, BSLFATAL, "sf_bt0: BT getting...\n");
  int cnt = backtrace(arr, 50);
  bslog_log_to(&sLgCnst, BSLFATAL, "sf_bt0: BT printing...\n");
  backtrace_symbols_fd(arr, cnt, fd);
}

static void sf_bt1(void) {
  int sz = 50;
  void **arr = alloca(sz * sizeof(void*));
  if (arr == NULL) {
    bslog_log_to(&sLgCnst, BSLFATAL, "sf_bt1: Can't alloca BT array!\n");
    return;
  }
  int i;
  for (i = 0; i < sz; i++) {
    arr[i] = NULL;
  }
  bslog_log_to(&sLgCnst, BSLFATAL, "sf_bt1: BT getting...\n");
  sf_backtrace(arr, sz);
  bslog_log_to(&sLgCnst, BSLFATAL, "sf_bt1: BT printing...\n");
  int count = 0;
  for (i = 0; i < sz; i++) {
    if (arr[i] != NULL) {
      count++;
    }
  }
  //addresses are usually 0x401da1 or 0x7f4c1ce03fbf, i.e. len=8-14
  char *fmsg = alloca(count * 12 * sizeof(char) + 20);
  if (fmsg == NULL) {
    bslog_log_to(&sLgCnst, BSLFATAL, "sf_bt1: Can't alloca BT fmsg!\n");
    return;
  }
  strcpy(fmsg, "BT:");
  for (i = 0; i < sz; i++) {
    if (arr[i] != NULL) {
      int len = strlen(fmsg);
      sprintf(fmsg + len, " %p", arr[i]);
    }
  }
  strcat(fmsg, "\n");
  bslog_log_to(&sLgCnst, BSLFATAL, fmsg);
}

/**
 * <p>Fatal signals handler.</p>
 * @param pSig - signal code
 **/
static void signal_fatal_handler(int pSig) {
  if (sUseBtNu == 0) {
    sf_bt0();
  } else {
    sf_bt1();
  }
  bslog_destroy();
  if (sFatDestr != NULL) {
    sFatDestr();
  }
  exit(pSig);
}

/**
 * <p>Initialize interception of fatal signals.</p>
 **/
void bsfatallog_init_fatal_signals(void) {
  if (sLgCnst.file_index == -1) {
    sLgCnst.file_index = 0;
    strcpy(sLogTag, __FILE__);
  }
  //Into main client:
  //signal(SIGINT , signal_exit_handler);
  //signal(SIGTERM, signal_exit_handler);
  signal(SIGABRT, signal_fatal_handler);
  signal(SIGILL, signal_fatal_handler);
  signal(SIGSEGV, signal_fatal_handler);
  signal(SIGFPE, signal_fatal_handler);
}

/**
 * <p>Set logging instrument.</p>
 * @param pLgCnst pointer to log constant data
 **/
void bsfatallog_set_logconst(BsLogConst *pLgCnst) {
  sLgCnst.file_index = pLgCnst->file_index;
  if (strlen(pLgCnst->tag) >= BSLFTAG_SIZE) {
    strncpy(sLgCnst.tag, pLgCnst->tag, BSLFTAG_SIZE);
    sLgCnst.tag[BSLFTAG_SIZE] = 0;
  } else {
    strcpy(sLgCnst.tag, pLgCnst->tag);
  }
}

/**
 * <p>Set fatal log's path for method number 0-backtrace_symbols_fd.</p>
 * @param pFatLogPth path
 **/
void bsfatallog_set_log_path(char *pFatLogPth) {
  sFatLogPth = pFatLogPth;
}

/**
 * <p>Set backtrace method number 0-backtrace_symbols_fd into separate fatal log,
 * 1-logging of set of BT addresses into BSLOG.</p>
 * @param pUseBtNu method number
 **/
void bsfatallog_set_use_bt_num(int pUseBtNu) {
  if (pUseBtNu == 0) {
    sUseBtNu = 0;
  } else {
    sUseBtNu = 1;
  }
}

/**
 * <p>Set fatal destructor.</p>
 * @param pFatDestr fatal destructor
 **/
void bsfatallog_set_fatal_destructor(BsVoid_Method *pFatDestr) {
  sFatDestr = pFatDestr;
}
