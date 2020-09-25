/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Tester on big dic.</p>
 * @author Yury Demidenko
 **/

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "locale.h"

#include "BsDicIwrds.h"
#include "BsDicWordDsl.h"
#include "BsFatalLog.h"
#include "BsError.h"

static BsDicIdxAb *s_idx_ab;
static const int s_dic_entry_buffer_size = 299;


static void sf_print_all_iwords(BsDicIwrds *p_iwrdssort) {
  BS_WCHAR_T wstr[200];
  bslog_log(BSLONLYMSG, "iwrds sorted%p:\n", p_iwrdssort);
  for (BS_IDX_T l = BS_IDX_0; l < p_iwrdssort->size; l++) {
    bsdicidxab_istr_to_wstr(p_iwrdssort->vals[l]->iword, wstr, p_iwrdssort->idx_ab);
    bslog_log(BSLONLYMSG, "#%lu %ls offset_dword=%ld length_dword=%d\n", l, wstr, p_iwrdssort->vals[l]->offset_dword, p_iwrdssort->vals[l]->length_dword);
  }
}

static void sf_test1(int argc, char *argv[]) {
  int ab_sz;
  char *pth = NULL;
  if (argc < 3) {
    pth = "tst_dic1.dsl";
    ab_sz = 15;
  } else {
    sscanf(argv[2], "%d", &ab_sz);
    pth = argv[1];
  }
  FILE *dic = fopen(pth, "r");
  if (dic == NULL) {
    if (errno == 0) { errno = BSE_ERR; }
    BSLOG_LOG(BSLERROR, "Can't open %s\n", pth);
    return;
  }
  bslog_log(BSLTEST, "Process dic=%s, AB size=%d\n", pth, ab_sz);
  BsDicIwrds *iwrds = NULL;
  BsDicI2wrds *i2wrds = NULL;
  BS_DO_E_OUT(s_idx_ab = bsdicidxab_new(ab_sz))
  BSDICIDXABTOTALS_CREATE(totals)
  BS_DO_E_OUT(bsdicidxab_iter_dsl_fill(dic, s_dic_entry_buffer_size, s_idx_ab, &totals))
  rewind(dic);
  BS_DO_E_OUT(bsdiciwrds_iter(dic, s_dic_entry_buffer_size, &totals, s_idx_ab, bsdicworddsl_iter_tus, bsdiciwrds_dwrd_csm, &iwrds))
  if (totals.i2wptSz > BS_IDX_0) {
    BS_DO_E_OUT(i2wrds = bsdici2wrds_new_fill(totals.i2wptSz, iwrds))
  }
out:
  if (iwrds != NULL && (errno != 0 || argc > 3 || argc < 3)) {
    sf_print_all_iwords(iwrds);
  }
  bsdiciwrds_free(iwrds);
  bsdici2wrds_free(i2wrds);
  fclose(dic);
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, ""); //it set to default system locale, e.g. en_US.UTF-8
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(1), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDicIwrdsBig.log");
  if (errno != 0) {
    bslf = bslogfiles_free(bslf);
    goto outlog;
  }
  bslog_set_maximum_size(30000000L);
  bslog_init(bslf);
outlog:
  bsfatallog_init_fatal_signals();
  errno = 0;
  //bslog_set_debug_floor(BS_DEBUGL_DATASET);
  //bslog_set_debug_ceiling(BS_DEBUGL_BSDICIWRDSORT);
  //bslog_set_debug_floor(BS_DEBUGL_DICIDXIRTRAW + 13);
  //bslog_set_debug_ceiling(BS_DEBUGL_DICIDXIRTRAW + 14);
  sf_test1(argc, argv);
  if (errno != 0) {
    BSLOG_ERR
  }
  if (s_idx_ab != NULL) {
    bsdicidxab_free(s_idx_ab);
  }
  bslog_destroy();
  return errno;
}
