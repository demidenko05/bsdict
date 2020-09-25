/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Test of BsArrays and BsFatalLog.</p>
 * @author Yury Demidenko
 **/

#include "stdlib.h"
#include "errno.h"
#include "locale.h"
#include "errno.h"
#include "string.h"

#include "BsStrings.h"
#include "BsError.h"
#include "BsFatalLog.h"

static void s_test0() {
 //0-"week" 1-"beep" 2-"speak",
 //sorted indexes array will be:
 //1 2 0
  BS_DO_E_RET(BsStrings *strings = bsstrings_new(3L))
  BsIdxSet *sidxs = NULL;
  BS_DO_E_OUT(bsstrings_add_inc(strings, bsstring_new("week"), 1L))
  BS_DO_E_OUT(bsstrings_add_inc(strings, bsstring_new("beep"), 1L))
  BS_DO_E_OUT(bsstrings_add_inc(strings, bsstring_new("speak"), 1L))
  BS_DO_E_OUT(sidxs = bsstrings_create_idxset(strings, BS_IDX_0))
  BS_IF_ENM_OUT(sidxs->size != 3, BSE_TEST_ERR, "sidxs->size != 3\n")
  bslog_log(BSLONLYMSG, "unsorted:\n");
  for (int i = 0; i < strings->size; i++) {
    bslog_log(BSLONLYMSG, "  #%d %s\n", i, strings->vals[i]->val);
  }
  bslog_log(BSLONLYMSG, "sorted:\n");
  for (int i = 0; i < strings->size; i++) {
    bslog_log(BSLONLYMSG, "  #%d %s\n", i, strings->vals[sidxs->vals[i]]->val);
  }
  BS_IF_ENM_OUT(sidxs->vals[0] != 1L, BSE_TEST_ERR, "sidxs->vals[0] != 1L\n")
  BS_IF_ENM_OUT(sidxs->vals[1] != 2L, BSE_TEST_ERR, "sidxs->vals[1] != 2L\n")
  BS_IF_ENM_OUT(sidxs->vals[2] != 0L, BSE_TEST_ERR, "sidxs->vals[2] != 0L\n")
  BS_DO_E_OUT(bsstrings_add_inc(strings, bsstring_new("asterisk"), 1L))
 //0-"week" 1-"beep" 2-"speak" 3-"asterisk",
 //sorted indexes array will be:
 //3 1 2 0
  BS_DO_E_OUT(bsstrings_redo_idxset(strings, sidxs, BS_IDX_1))
  BS_IF_ENM_OUT(sidxs->size != 4, BSE_TEST_ERR, "sidxs->size != 4\n")
  bslog_log(BSLONLYMSG, "unsorted:\n");
  for (int i = 0; i < strings->size; i++) {
    bslog_log(BSLONLYMSG, "  #%d %s\n", i, strings->vals[i]->val);
  }
  bslog_log(BSLONLYMSG, "sorted:\n");
  for (int i = 0; i < strings->size; i++) {
    bslog_log(BSLONLYMSG, "  #%d %s\n", i, strings->vals[sidxs->vals[i]]->val);
  }
  BS_IF_ENM_OUT(sidxs->vals[0] != 3L, BSE_TEST_ERR, "sidxs->vals[0] != 3L\n")
  BS_IF_ENM_OUT(sidxs->vals[1] != 1L, BSE_TEST_ERR, "sidxs->vals[1] != 1L\n")
  BS_IF_ENM_OUT(sidxs->vals[2] != 2L, BSE_TEST_ERR, "sidxs->vals[2] != 2L\n")
  BS_IF_ENM_OUT(sidxs->vals[3] != 0L, BSE_TEST_ERR, "sidxs->vals[3] != 0L\n")
out:
  bsstrings_free(strings);
  bsidxset_free(sidxs);
}

static int sCount = 0;

static void
  s_void_meth1 (void)
{
  sCount++;
}

static void
  s_test1 ()
{
  //it will return error on hardly ever possible ENOMEM:
  BSVOIDMETHS_NEW_E_RET (arr, 2L) //size=2

  //increase=2 when full-filled:
  BS_DO_E_OUT (bsvoidmeths_add_inc (arr, &s_void_meth1, 2))

  BS_DO_E_OUT (bsvoidmeths_add_inc (arr, &s_void_meth1, 2))

  //it will increase size or hardly ever possible ENOMEM:
  BS_DO_E_OUT (bsvoidmeths_add_inc (arr, &s_void_meth1, 2))

  bsvoidmeths_invoke_all (arr); //sCount must be 3

  BS_DO_ERR (bsvoidmeths_remove_shrink (arr, 4L)) //ERROR! 4 > (size-1)=3

out:
  bsvoidmeths_free(arr);
}

/**
 * <p>Main program initializes log files
 * and passes file's indexes to clients.</p>
 **/
int
  main (int argc, char *argv[])
{
  setlocale(LC_ALL, "");
  errno = 0;
  BS_DO_E_RETE(BsLogFiles *bslf=bslogfiles_new(1))
  bslog_files_set_path(bslf, 0, "bsdataset.log");
  if (errno != 0) {
    bslogfiles_free(bslf);
    return errno;
  }
  BS_DO_E_OUT(bslog_init(bslf))
  bsfatallog_set_log_path("bscollfatal.log");
  bsfatallog_init_fatal_signals();
  BS_DO_E_OUT(s_test0())
  s_test1(); //must be BSE_ARR_OUT_OF_BOUNDS
  if (errno == 0) {
    errno = BSE_ERR;
    BSLOG_ERR
  } else {
    if (argc > 1)  {
      char *nullch = NULL;
      int sz = strlen(nullch);
      if (sz > 1) {
        bslog_log(BSLERROR, "Wrong size on NULL=%d",  sz);
        goto out;
      }
    }
    if (errno != BSE_ARR_OUT_OF_BOUNDS || sCount != 3) {
      bslog_log(BSLERROR, "main sCount!=3=%d or errno!=11000=%d",  sCount, errno);
    } else {
      errno = 0;
    }
  }
out:
  bslog_destroy();
  return errno;
}

