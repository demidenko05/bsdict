/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Test of BsLog multi-threads.</p>
 * @author Yury Demidenko
 **/

#include "stdlib.h"
#include "pthread.h"
#include "unistd.h"

#include "BsLog.h"
#include "BsError.h"

#define NUMTHRDS 10

static pthread_t sThreads[NUMTHRDS];

static BsLogConst *s_lconst = NULL;

static int sCountFinished = 0;

//Dic opener args:
typedef struct {
  BS_IDX_T ixSt;
  BS_IDX_T ixEn;
} OpenArg;

void *tst_log(void *p_arg) {
  OpenArg *arg = (OpenArg*) p_arg;
  for (int i = 0; i < 1000; i++) {
    BSLOG_LOG_TO(s_lconst, BSLINFO, "arg.st="BS_IDX_FMT", arg.en="BS_IDX_FMT", step#%d........................................................\n", arg->ixSt, arg->ixEn, i)
  }
  sCountFinished++;
  printf("Finished count = %d\n", sCountFinished);
  pthread_exit((void*) 0);
}

/**
 * <p>Main program initializes log files
 * and passes file's indexes to clients.</p>
 **/
int main(int argc, char *argv[]) {
  BS_DO_E_RETE(BsLogFiles *bslf=bslogfiles_new(1))
  bslog_files_set_path(bslf, 0, "bslogtest.log");
  if (errno != 0) {
    bslogfiles_free(bslf);
    return errno;
  }
  BS_DO_E_OUT(bslog_init(bslf))
  BsLogConst bslc = { .file_index=0, .tag="test1" };
  BS_DO_E_OUT(bslogconst_check(&bslc))
  s_lconst = &bslc;
  //bslog_set_maximum_size(5000);
  long i;
  //void *status;
  pthread_attr_t thrdAttr; 
  
  pthread_attr_init(&thrdAttr);
  //pthread_attr_setdetachstate(&thrdAttr, PTHREAD_CREATE_JOINABLE);
  BS_IF_ENM_OUTE ( pthread_attr_setdetachstate(&thrdAttr, PTHREAD_CREATE_DETACHED) != 0,
                      BSE_TEST_ERR, "Can't set detached attr\n")
  OpenArg oargs[2];
  oargs[0].ixSt = BS_IDX_0;
  oargs[0].ixEn = BS_IDX_1;
  oargs[1].ixSt = BS_IDX_2;
  oargs[1].ixEn = BS_IDX_10;

  for (i = 0; i < NUMTHRDS; i++) {
    int k = i % 2;
    pthread_create(&sThreads[i], &thrdAttr, tst_log, (void*) &oargs[k]);
  }
  pthread_attr_destroy(&thrdAttr);
  for(i = 0; i < NUMTHRDS; i++) {
    //pthread_join(sThreads[i], &status);
  }
  long l = 0L;
  while (1) {
    l++;
    if (sCountFinished == NUMTHRDS) {
      break;
    }
    sleep (1);
  }
  printf("Main quit, count = %d, l = %ld\n", sCountFinished, l);
out:
  bslog_destroy();
  pthread_exit(NULL);
oute:
  bslog_destroy();
  return errno;
}

