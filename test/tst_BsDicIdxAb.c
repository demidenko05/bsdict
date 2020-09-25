/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Tester of BsDicIdxAb.c</p>
 * @author Yury Demidenko
 **/

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "locale.h"

#include "BsFatalLog.h"
#include "BsError.h"
#include "BsDicIdxAb.h"
#include "BsDicWordDsl.h"

static const int s_dic_entry_buffer_size = 99;

static BS_WCHAR_T *s_w123[3] = { L"бюллетенить", L"валяние", L"ящур" };
static BS_WCHAR_T *s_wb123[3] = { L"БЮЛЛЕТЕНИТЬ", L"ВАЛЯНИЕ", L"ЯЩУР" };

static void sf_test_ab0(BsDicIdxAb *pIdx_ab) {
  bslog_log(BSLTEST, "AB wchars - ");
  BS_WCHAR_T prev = 0;
  for (int k = 0; k < pIdx_ab->chrsTot; k++) {
    bslog_log(BSLONLYMSG, " %lc=%d", pIdx_ab->wchars[k], pIdx_ab->wchars[k]);
    if (pIdx_ab->wchars[k] <= prev) {
      errno = BSE_ERR;
      BSLOG_ERR
      return;
    }
  }
  bslog_log(BSLONLYMSG, "\n");
  BS_CHAR_T idxstr[3][15];
  BS_WCHAR_T wstr[15];
  for (int i = 0; i < 3; i++) {
    bsdicidxab_wstr_to_istr(s_wb123[i], idxstr[i], pIdx_ab);
    bsdicidxab_istr_to_wstr(idxstr[i], wstr, pIdx_ab);
    if (bsdicidx_wstr_cmp(s_w123[i], wstr) != 0) {
      errno = BSE_ERR;
      BSLOG_LOG(BSLERROR, "Error cross converting str  %ls\n", s_wb123[i]);
      return;
    }
  }
  BS_CHAR_T idxstr0[15];
  bsdicidx_istr_cpy(idxstr0, idxstr[0]);
  int cmpr = bsdicidx_istr_cmp(idxstr0, idxstr[0]);
  if (cmpr != 0) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Error comparing is0=is0\n");
  } else {
    cmpr = bsdicidx_istr_cmp(idxstr[0], idxstr[1]);
    if (cmpr != -1) {
      errno = BSE_ERR;
      BSLOG_LOG(BSLERROR, "Error comparing is0<is1\n");
    } else {
      cmpr = bsdicidx_istr_cmp(idxstr[2], idxstr[1]);
      if (cmpr != 1) {
        errno = BSE_ERR;
        BSLOG_LOG(BSLERROR, "Error comparing is2>is1\n");
      }
    }
    int cmpri = bsdicidx_istr_cmp(idxstr[0], idxstr[1]);
    if (cmpri != -1) {
      errno = BSE_ERR;
      BSLOG_LOG(BSLERROR, "Error comparing is0<is1\n");
    } else {
      cmpri = bsdicidx_istr_cmp(idxstr[2], idxstr[1]);
      if (cmpri != 1) {
        errno = BSE_ERR;
        BSLOG_LOG(BSLERROR, "Error comparing is2>is1\n");
      }
    }
  }
}

static void sf_test_ab1() {
  BsDicIdxAb *idx_ab = bsdicidxab_new(10);
  for (int i = 0; i < 3; i++) {
    bsdicidxab_add_wstr(s_wb123[i], idx_ab);
    if (errno != 0) {
      BSLOG_LOG(BSLERROR, "Error adding word into AB - %ls\n", s_wb123[i]);
      return;
    }
  }
  for (int i = 0; i < 3; i++) {
    int str_len = wcslen(s_w123[i]);
    for (int j = 0; j < str_len; j++) {
      BS_WCHAR_T wltr = s_w123[i][j];
      int is_char_fnd = FALSE;
      for (int k = 0; k < idx_ab->chrsTot; k++) {
        if (wltr == idx_ab->wchars[k]) {
          is_char_fnd = TRUE;
          break;
        }
      }
      if (!is_char_fnd) {
        errno = BSE_ERR;
        BSLOG_LOG(BSLERROR, "Error finding in AB char=%lc #%x\n", wltr, wltr);
        goto out;
      }
    }
  }
  sf_test_ab0(idx_ab);
out:
  bsdicidxab_free(idx_ab);
}

static void sf_test_ab2() {
  FILE *dic = fopen("tst_dic1.dsl", "r");
  if (dic == NULL) {
    BSLOG_ERR
    return;
  }
  BsDicIdxAb *idx_ab = bsdicidxab_new(11);
  BSDICIDXABTOTALS_CREATE(totals)
  BS_DO_E_OUT(bsdicidxab_iter_dsl_fill(dic, s_dic_entry_buffer_size, idx_ab, &totals))
  if (totals.max_iword_len != 11 || totals.dwoltSz != 3) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "totals.max_iword_len != 11=%d || totals.dwoltSz != 3=%d\n", totals.max_iword_len, totals.dwoltSz)
    goto out;
  } 
  //check words:
  sf_test_ab0(idx_ab);
out:
  bsdicidxab_free(idx_ab);
  fclose(dic);
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "");
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(1), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDicIdxAb.log");
  if (errno != 0) {
    bslf = bslogfiles_free(bslf);
    goto outlog;
  }
  bslog_init(bslf);
outlog:
  bsfatallog_init_fatal_signals();
  errno = 0;
  BS_DO_E_OUT(sf_test_ab1())
  sf_test_ab2();
out:
  if (errno != 0) {
    BSLOG_ERR
  }
  bslog_destroy();
  return errno;
}
