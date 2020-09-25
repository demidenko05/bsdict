/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Tester of BsDiIxFind.c on outer big dic.</p>
 * @author Yury Demidenko
 **/

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "locale.h"

#include "BsFatalLog.h"
#include "BsError.h"
#include "BsDiIxFind.h"

static void sf_prn_idx(BsDiIxTxRm *pDiIxRm, BS_IDX_T p_irt_st, BS_IDX_T p_irt_end) {
  bslog_log(BSLONLYMSG, "idx-ram#%p\n", pDiIxRm);
  bslog_log(BSLONLYMSG, "  hirt:\n");
  BS_WCHAR_T wstr[150];
  for (int i = 0; i < pDiIxRm->head->hirtSz; i++) {
    bslog_log(BSLONLYMSG, "    #%d fchar=%hu irtIdx="BS_IDX_FMT" sch_sz=%d\n", i, pDiIxRm->head->hirt[i]->fchar, pDiIxRm->head->hirt[i]->irtIdx, pDiIxRm->head->hirt[i]->schrsSz);
    if (pDiIxRm->head->hirt[i]->schrsSz > 0) {
      for (int j = 0; j < pDiIxRm->head->hirt[i]->schrsSz; j++) {
        bslog_log(BSLONLYMSG, "      schar=%hu irtIdx="BS_IDX_FMT"\n", pDiIxRm->head->hirt[i]->schars[j], pDiIxRm->head->hirt[i]->irtIdxs[j]);
      }
    }
  }
  BS_IDX_T l;
  bslog_log(BSLONLYMSG, "  irt:\n", pDiIxRm);
  if (p_irt_st == BS_IDX_NULL) {
    p_irt_st = BS_IDX_0;
  }
  if (p_irt_end == BS_IDX_NULL) {
    if (pDiIxRm->head->irtSz < 500L) {
      p_irt_end = pDiIxRm->head->irtSz;
    } else {
      p_irt_end = 200L;
    }
  }
  for (l = p_irt_st; l < p_irt_end; l++) {
    bsdicidxab_istr_to_wstr(pDiIxRm->irt[l]->idx_subwrd, wstr, pDiIxRm->head->ab);
    bslog_log(BSLONLYMSG, "    #"BS_IDX_FMT" %ls dwolt_start="BS_IDX_FMT" i2wpt_quantity=%d i2wpt_start="BS_IDX_FMT"\n", l, wstr, pDiIxRm->irt[l]->dwolt_start, pDiIxRm->irt[l]->i2wpt_quantity, pDiIxRm->irt[l]->i2wpt_start);
  }
  if (pDiIxRm->head->irtSz < 500L) {
    bslog_log(BSLONLYMSG, "  i2wpt:\n", pDiIxRm);
    for (l = BS_IDX_0; l < pDiIxRm->head->i2wptSz; l++) {
      bslog_log(BSLONLYMSG, "    #"BS_IDX_FMT" = "BS_IDX_FMT"\n", l, pDiIxRm->i2wpt[l]);
    }
    bslog_log(BSLONLYMSG, "  dwolt:\n", pDiIxRm);
    for (l = BS_IDX_0; l < pDiIxRm->head->dwoltSz; l++) {
      bslog_log(BSLONLYMSG, "    #%ld ofst=%ld len=%d\n", l, pDiIxRm->dwolt[l]->offset_dword, pDiIxRm->dwolt[l]->length_dword);
    }
  }
}

static void sf_test1(int argc, char *argv[]) {
  BSDICIDXRAM_OPEN_E_RET (idx_ram, argv[1])
  BsDiIxOst *opSt = NULL;
  BS_DO_E_OUT (BsDiFdWds *dicWrds = bsdifdwds_new (BS_IDX_100))
  if (idx_ram == NULL) {
    bslog_log(BSLTEST, "Try to create IDX for%s\n", argv[1]);
    BS_DO_E_OUT(opSt = bsdiixost_new ())
    BS_DO_E_OUT(idx_ram = bsdiixtxrm_create(argv[1], opSt))
    BS_DO_E_OUT(bsdiixtxrm_validate(idx_ram))
    BS_DO_E_OUT(bsdiixtxrm_save(idx_ram, argv[1]))
    if(idx_ram->dicFl == NULL) {
      if (errno == 0) { errno = BSE_OPEN_FILE; }
      BSLOG_ERR
      goto out;
    }
  }
  if (argc == 5) {
    BS_IDX_T irt_st=BS_IDX_NULL, irt_end=BS_IDX_NULL;
    sscanf(argv[3], ""BS_IDX_FMT"", &irt_st);
    sscanf(argv[4], ""BS_IDX_FMT"", &irt_end);
    sf_prn_idx(idx_ram, irt_st, irt_end);
  }
  bslog_log(BSLTEST, "Try to find matched to '%s' in '%s'...\n", argv[2], argv[1]);
  BS_DO_E_OUT(bsdiixtxrmfind_mtch (idx_ram, dicWrds, argv[2]))
  for ( int i = 0; i < dicWrds->size; i++ )
      { bslog_log(BSLONLYMSG, "dicWrds->vals[%d]->wrd->val = %s\n", i, dicWrds->vals[i]->wrd->val); }
out:
  bsdiixost_free (opSt);
  bsdifdwds_free (dicWrds);
  bsdiixtxrm_destroy(idx_ram);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    perror("WRONG params");
    return BSE_WRONG_PARAMS;
  }
  setlocale(LC_ALL, ""); //it set to default system locale, e.g. en_US.UTF-8
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(1), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDiIxFindBig.log");
  if (errno != 0) {
    bslf = bslogfiles_free(bslf);
    goto outlog;
  }
  bslog_init(bslf);
outlog:
  bsfatallog_init_fatal_signals();
  errno = 0;
  bslog_set_debug_floor(BS_DEBUGL_DICIDXFIND);
  bslog_set_debug_ceiling(BS_DEBUGL_DICIDXFIND);
  sf_test1(argc, argv);
  if (errno != 0) {
    BSLOG_ERR
  }
  bslog_destroy();
  return errno;
}
