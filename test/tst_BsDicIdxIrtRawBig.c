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

#include "BsDicIdxIrtRaw.h"
#include "BsDicWordDsl.h"
#include "BsFatalLog.h"
#include "BsError.h"
static const int s_dic_entry_buffer_size = 299;

static void sf_print_all_iwords(BsDicIwrds *p_iwrds) {
  BS_WCHAR_T wstr[200];
  bslog_log(BSLONLYMSG, "iwrds sorted%p:\n", p_iwrds);
  for (BS_IDX_T l = BS_IDX_0; l < p_iwrds->size; l++) {
    bsdicidxab_istr_to_wstr(p_iwrds->vals[l]->iword, wstr, p_iwrds->idx_ab);
    bslog_log(BSLONLYMSG, "#"BS_IDX_FMT" %ls offset_dword=%ld length_dword=%d\n", l, wstr, p_iwrds->vals[l]->offset_dword, p_iwrds->vals[l]->length_dword);
  }
}

static void sf_test1(int argc, char *argv[]) {
  int ab_sz;
  sscanf(argv[2], "%d", &ab_sz);
  FILE *dic = fopen(argv[1], "r");
  if (dic == NULL) {
    if (errno == 0) { errno = BSE_ERR; }
    BSLOG_LOG(BSLERROR, "Can't open %s\n", argv[2]);
    return;
  }
  bslog_log(BSLTEST, "Process dic=%s, AB size=%d\n", argv[1], ab_sz);
  BsDicIwrds *iwrds = NULL;
  BsDicI2wrds *i2wrds = NULL;
  BsDicIdxIrtRaw *irt = NULL;
  BsDicIdxIrtTots *irt_tots = NULL;
  BS_DO_E_OUT(BsDicIdxAb *idx_ab = bsdicidxab_new(ab_sz))
  BSDICIDXABTOTALS_CREATE(ab_tots)
  BS_DO_E_OUT(bsdicidxab_iter_dsl_fill(dic, s_dic_entry_buffer_size, idx_ab, &ab_tots))
  rewind(dic);
  BS_DO_E_OUT(bsdiciwrds_iter(dic, s_dic_entry_buffer_size, &ab_tots, idx_ab, bsdicworddsl_iter_tus,
    bsdiciwrds_dwrd_csm, &iwrds))
  if (ab_tots.i2wptSz > BS_IDX_0) {
    BS_DO_E_OUT(i2wrds = bsdici2wrds_new_fill(ab_tots.i2wptSz, iwrds))
  }
  if (argc > 3) {
    sf_print_all_iwords(iwrds);
  }
  BS_DO_E_OUT(irt_tots = bsdicidxirttotals_new())
  irt_tots->ab_chars_total = idx_ab->chrsTot;
  BS_DO_E_OUT(irt = bsdicidxirtraw_create(iwrds, i2wrds, irt_tots))
  BS_DO_E_OUT(bsdicidxirtraw_validate(irt, irt_tots->irtSz))
out:
  bsdicidxirttotals_free(irt_tots);
  bsdicidxirtraw_free(irt);
  bsdiciwrds_free(iwrds);
  bsdici2wrds_free(i2wrds);
  bsdicidxab_free(idx_ab);
  fclose(dic);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    errno = BSE_WRONG_PARAMS;
    perror(bserror_to_str(errno));
    return errno;
  }
  setlocale(LC_ALL, ""); //it set to default system locale, e.g. en_US.UTF-8
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(1), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDicIdxIrtRawBig.log");
  if (errno != 0) {
    bslf = bslogfiles_free(bslf);
    goto outlog;
  }
  bslog_set_maximum_size(6000000L);
  bslog_init(bslf);
outlog:
  bsfatallog_init_fatal_signals();
  errno = 0;
  bslog_set_debug_floor(BS_DEBUGL_DICIDXIRTRAW + 13);
  bslog_set_debug_ceiling(BS_DEBUGL_DICIDXIRTRAW + 14);
  sf_test1(argc, argv);
  if (errno != 0) {
    BSLOG_ERR
  }
  bslog_destroy();
  return errno;
}
