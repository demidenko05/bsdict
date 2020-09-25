/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Tester of BsDicDescrDsl.c.</p>
 * @author Yury Demidenko
 **/

#include "stdio.h"
#include "stdlib.h"
#include "wchar.h"
#include "wctype.h"
#include "string.h"
#include "locale.h"

#include "BsFatalLog.h"
#include "BsError.h"
#include "BsDicDescrDsl.h"

static void sf_test1() {
  char *dic_pth = "tst_dic1.dsl";
  FILE *dic = fopen(dic_pth, "r");
  if (dic == NULL) {
    if (errno == 0) { errno = BSE_ERR; }
    BSLOG_LOG(BSLERROR, "Can't open %s\n", dic_pth);
    return;
  }
  BS_DO_E_OUT(BsHypStrs *hstrs = bsdicdescrdsl_read(dic, 75L))
  bslog_log(BSLONLYMSG, "Strings:\n");
  for (BS_IDX_T l = BS_IDX_0; l < hstrs->bsize; l++) {
    if (hstrs->vals[l] != NULL) {
      bslog_log(BSLONLYMSG, "  %s <-tags[", hstrs->vals[l]->str->val);
      if (hstrs->vals[l]->tags != NULL && hstrs->vals[l]->tags->size > BS_IDX_0) {
        for (int i = 0; i < hstrs->vals[l]->tags->size; i++) {
          bslog_log(BSLONLYMSG, " %s", bshyptag_name (hstrs->vals[l]->tags->vals[i]));
        }
      }
      bslog_log(BSLONLYMSG, " ]");
    }
  }
  BS_IF_ENM_OUT(strncmp(hstrs->vals[0]->str->val, "с.", 2) != 0, BSE_TEST_ERR, "hs[0]string!='c.'\n")
  BS_IF_ENM_OUT(strncmp(hstrs->vals[1]->str->val, " (сукна)", 15) != 0, BSE_TEST_ERR, "hs[1]string!=' (сукна)'\n")
  BS_IF_ENM_OUT(strncmp(hstrs->vals[3]->str->val, "fulling", 7) != 0, BSE_TEST_ERR, "hs[2]string!='fulling'\n")
  BS_IF_ENM_OUT(strncmp(hstrs->vals[4]->str->val, ", milling ", 10) != 0, BSE_TEST_ERR, "hs[3]string!=', milling '\n")
  BS_IF_ENM_OUT(hstrs->vals[0]->tags->vals[0] != EBSHT_ITALIC, BSE_TEST_ERR, "hs[0]tags[0]!=EBSHT_ITALIC\n")
  BS_IF_ENM_OUT(hstrs->vals[0]->tags->vals[1] != EBSHT_GREEN, BSE_TEST_ERR, "hs[0]tags[1]!=EBSHT_GREEN\n")
  BS_IF_ENM_OUT(hstrs->vals[0]->tags->vals[2] != EBSHT_TOOLTIP, BSE_TEST_ERR, "hs[0]tags[2]!=EBSHT_TOOLTIP\n")
  BS_IF_ENM_OUT(hstrs->vals[1]->tags->vals[0] != EBSHT_ITALIC, BSE_TEST_ERR, "hs[1]tags[0]!=EBSHT_ITALIC\n")
  BS_IF_ENM_OUT(hstrs->vals[3]->tags->vals[0] != EBSHT_TAB1, BSE_TEST_ERR, "hs[2]tags[0]!=EBSHT_TAB1\n")
  BS_IF_ENM_OUT(hstrs->vals[3]->tags->vals[1] != EBSHT_BOLD, BSE_TEST_ERR, "hs[2]tags[1]!=EBSHT_BOLD\n")
  BS_IF_ENM_OUT(hstrs->vals[4]->tags->vals[0] != EBSHT_TAB1, BSE_TEST_ERR, "hs[3]tags[0]!=EBSHT_TAB1\n")
out:
  bshypstrs_free(hstrs);
  fclose(dic);
}

static void sf_test2 (char *argv[]) {
  FILE *dic = fopen(argv[1], "r");
  if (dic == NULL) {
    if (errno == 0) { errno = BSE_ERR; }
    BSLOG_LOG(BSLERROR, "Can't open %s\n", argv[1]);
    return;
  }
  BS_FOFST_T ofst;
  sscanf(argv[2], "%ld", &ofst);
  BS_DO_E_OUT(BsHypStrs *hstrs = bsdicdescrdsl_read(dic, ofst))
  bslog_log(BSLONLYMSG, "Strings:\n");
  for (BS_IDX_T l = BS_IDX_0; l < hstrs->bsize; l++) {
    if (hstrs->vals[l] != NULL) {
      bslog_log(BSLONLYMSG, "  %s <-tags[", hstrs->vals[l]->str->val);
      if (hstrs->vals[l]->tags != NULL && hstrs->vals[l]->tags->size > BS_IDX_0) {
        for (int i = 0; i < hstrs->vals[l]->tags->size; i++) {
          bslog_log(BSLONLYMSG, " %s", bshyptag_name (hstrs->vals[l]->tags->vals[i]));
        }
      }
      bslog_log(BSLONLYMSG, " ]");
    }
  }
out:
  bshypstrs_free(hstrs);
  fclose(dic);
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "");
  //log file wrong initialized! so printing into stdout
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(0), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDicDescrDsl.log");
  if (errno != 0) {
    bslf = bslogfiles_free(bslf);
    goto outlog;
  }
  bslog_init(bslf);
outlog:
  bsfatallog_init_fatal_signals();
  errno = 0;
  //bslog_set_debug_ceiling(999999);
  BS_DO_E_OUT (sf_test1())
  if ( argc == 3 )
  {
    sf_test2 (argv);
  }
out:
  bslog_destroy();
  return errno;
}
