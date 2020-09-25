/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Tester of BsDicIdxAb.c idx.word matching</p>
 * @author Yury Demidenko
 **/

#include "string.h"
#include "stdlib.h"
#include "locale.h"

#include "BsFatalLog.h"
#include "BsError.h"
#include "BsDicIdxAb.h"

static void sf_print_istr(BS_CHAR_T *p_istr) {
  bslog_log(BSLTEST, "IDX WORD - ");
  for (int i = 0; ; i++) {
    if (p_istr[i] == 0) {
      break;
    } else {
      bslog_log(BSLONLYMSG, " %u", p_istr[i]);
    }
  }
  bslog_log(BSLONLYMSG, "\n");
}

#define TOTAL_WRDS 6
static BS_WCHAR_T *s_dwords[] = { L"бюл",  L"бюллетенить", L"валяние бюл", L"ящур--инд док бюл", L"ящурм2--инд док бюл", L"дллбюл" };
static BS_WCHAR_T *s_dwords_tr[] = { L"бюл",  L"бюллетенить", L"валяние бюл", L"ящур--инд док бюл", L"ящурм2--инд док бюл", L"дллбюл" };
static BS_CHAR_T s_idxwords[TOTAL_WRDS][20];
static BS_WCHAR_T dword[20];

static void sf_test_ab1() {
  BsDicIdxAb *idx_ab = bsdicidxab_new(15);
  int i;
  for (i = 0; i < TOTAL_WRDS; i++) {
    bsdicidxab_add_wstr(s_dwords[i], idx_ab);
    if (errno != 0) {
      BSLOG_LOG(BSLERROR, "Error adding word into AB - %ls\n", s_dwords[i]);
      goto out;
    }
  }
  for (i = 0; i < TOTAL_WRDS; i++) {
    //bslog_log(BSLTEST, "Address of idxword#%d=%d\n", i, s_idxwords[i]);
    bsdicidxab_wstr_to_istr(s_dwords[i], s_idxwords[i], idx_ab);
    bsdicidxab_istr_to_wstr(s_idxwords[i], dword, idx_ab);
    if (bsdicidx_wstr_cmp(s_dwords_tr[i], dword) != 0) {
      errno = BSE_ERR;
      BSLOG_LOG(BSLERROR, "1 Error cross converting orig_tr=%ls conv=%ls i=%d\n", s_dwords_tr[i], dword, i);
      goto out;
    }
    bsdicidxab_istr_to_wstr(s_idxwords[i], dword, idx_ab);
    if (bsdicidx_wstr_cmp(s_dwords_tr[i], dword) != 0) {
      errno = BSE_ERR;
      BSLOG_LOG(BSLERROR, "2 Error cross converting orig_tr=%ls conv=%ls i=%d\n", s_dwords_tr[i], dword, i);
      goto out;
    }
  }
  for (i = 0; i < TOTAL_WRDS; i++) {
    bsdicidxab_istr_to_wstr(s_idxwords[i], dword, idx_ab);
    if (bsdicidx_wstr_cmp(s_dwords_tr[i], dword) != 0) {
      errno = BSE_ERR;
      BSLOG_LOG(BSLERROR, "3 Error cross converting orig_tr=%ls conv=%ls i=%d\n", s_dwords_tr[i], dword, i);
      sf_print_istr(s_idxwords[i]);
      goto out;
    }
  }
  int n;
  BS_IF_ENM_OUT (bsdicidx_istr_cont (s_idxwords[0], s_idxwords[1]), BSE_TEST_ERR, "бюл cont бюллетенить");
  BS_IF_ENM_OUT (!bsdicidx_istr_cont (s_idxwords[1], s_idxwords[0]), BSE_TEST_ERR, "бюллетенить !cont бюл");
  BS_IF_ENM_OUT (!bsdicidx_istr_cont (s_idxwords[2], s_idxwords[0]), BSE_TEST_ERR, "валяние бюл !cont бюл");
  if (bsdicidx_istr_match(s_idxwords[0], s_idxwords[1], idx_ab->ispace) != EWMATCHFROMSTART) {
    n = 1; bsdicidxab_istr_to_wstr(s_idxwords[n], dword, idx_ab);
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Error matching subword=%ls to word#%d=%ls  rez=%d\n", s_dwords[0], n, dword, bsdicidx_istr_match(s_idxwords[0], s_idxwords[n], idx_ab->ispace));
  } else if (bsdicidx_istr_match(s_idxwords[0], s_idxwords[2], idx_ab->ispace) != EWMATCHAFTERSTART) {
    errno = BSE_ERR;
    n = 2; bsdicidxab_istr_to_wstr(s_idxwords[n], dword, idx_ab);
    BSLOG_LOG(BSLERROR, "Error matching subword=%ls to word#%d=%ls  rez=%d\n", s_dwords[0], n, dword, bsdicidx_istr_match(s_idxwords[0], s_idxwords[n], idx_ab->ispace));
  } else if (bsdicidx_istr_match(s_idxwords[0], s_idxwords[3], idx_ab->ispace) != EWMATCHAFTERSTART) {
    n = 3; bsdicidxab_istr_to_wstr(s_idxwords[n], dword, idx_ab);
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Error matching subword=%ls to word#%d=%ls  rez=%d\n", s_dwords[0], n, dword, bsdicidx_istr_match(s_idxwords[0], s_idxwords[n], idx_ab->ispace));
  } else if (bsdicidx_istr_match(s_idxwords[0], s_idxwords[4], idx_ab->ispace) != EWMATCHAFTERSTART) {
    n = 4; bsdicidxab_istr_to_wstr(s_idxwords[n], dword, idx_ab);
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Error matching subword=%ls to word#%d=%ls  rez=%d\n", s_dwords[0], n, dword, bsdicidx_istr_match(s_idxwords[0], s_idxwords[n], idx_ab->ispace));
  } else if (bsdicidx_istr_match(s_idxwords[0], s_idxwords[5], idx_ab->ispace) != EWMATCHNONE) {
    n = 5; bsdicidxab_istr_to_wstr(s_idxwords[n], dword, idx_ab);
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Error matching subword=%ls to word#%d=%ls  rez=%d\n", s_dwords[0], n, dword, bsdicidx_istr_match(s_idxwords[0], s_idxwords[n], idx_ab->ispace));
  }
out:
  bsdicidxab_free(idx_ab);
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "");
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(1), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDicIdxAbMatch.log");
  if (errno != 0) {
    bslf = bslogfiles_free(bslf);
    goto outlog;
  }
  bslog_init(bslf);
outlog:
  bsfatallog_init_fatal_signals();
  errno = 0;
  sf_test_ab1();
  if (errno != 0) {
    BSLOG_ERR
  }
  bslog_destroy();
  return errno;
}
