/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Tester of BsDiIxFind.c file on outer big dic.</p>
 * @author Yury Demidenko
 **/

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "locale.h"

#include "BsFatalLog.h"
#include "BsError.h"
#include "BsDiIxFind.h"

static void sf_prn_idx(BsDiIxTx *pDiIx) {
  bslog_log(BSLONLYMSG, "idx-ram#%p, BS_CHAR_MAX=%hu\n", pDiIx, BS_CHAR_MAX);
  bslog_log(BSLONLYMSG, "  hirt:\n");
  for (int i = 0; i < pDiIx->head->hirtSz; i++) {
    bslog_log(BSLONLYMSG, "    #%d fchar=%hu irtIdx="BS_IDX_FMT" sch_sz=%d\n", i, pDiIx->head->hirt[i]->fchar, pDiIx->head->hirt[i]->irtIdx, pDiIx->head->hirt[i]->schrsSz);
    if (pDiIx->head->hirt[i]->schrsSz > 0) {
      for (int j = 0; j < pDiIx->head->hirt[i]->schrsSz; j++) {
        
        bslog_log(BSLONLYMSG, "      schar=%hu irtIdx="BS_IDX_FMT"\n", pDiIx->head->hirt[i]->schars[j], pDiIx->head->hirt[i]->irtIdxs[j]);
      }
    }
  }
}

static void sf_test1(int argc, char *argv[]) {
  BS_DO_E_RET (BsDiIxOst *opSt = bsdiixost_new ())
  BS_DO_E_OUT (BsDiFdWds *dicWrds = bsdifdwds_new (BS_IDX_100))
  BS_DO_E_OUT (BsDiIxTx *diIx = (BsDiIxTx*) bsdiixtx_open (argv[1], opSt, false))
  BS_IF_ENM_OUT (diIx == NULL, BSE_TEST_ERR, "NULL opened without error!\n");
  if ( argc > 3 )
          { sf_prn_idx(diIx); }
  bslog_log (BSLTEST, "Try to find matched to '%s' in '%s'...\n", argv[2], argv[1]);
  BS_DO_E_OUT (bsdiixtxfind_mtch (diIx, dicWrds, argv[2]))
  bslog_set_debug_floor(0);
  bslog_set_debug_ceiling(0);
  for ( int i = 0; i < dicWrds->size; i++ )
  {
    BS_DO_E_OUT(BsString *str = bsstring_new (dicWrds->vals[i]->wrd->val))
    BS_DO_E_OUT(BsDicString *dstr = bsdicidxfind_exactly (diIx, str))
    if ( dstr != NULL ) 
    {
      bslog_log (BSLONLYMSG, "#%d %s, offset=%ld\n", i, dstr->val, dstr->offset);
    } else {
      errno = BSE_TEST_ERR;
      BSLOG_LOG (BSLERROR, "Can't found exactly %s\n", dicWrds->vals[i]->wrd->val)
    }
    bsstring_free (str);
    bsdicstring_free (dstr);
  }
out:
  bsdiixost_free (opSt);
  bsdifdwds_free (dicWrds);
  bsdiixtx_destroy(diIx);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    perror("WRONG params");
    return BSE_WRONG_PARAMS;
  }
  setlocale(LC_ALL, ""); //it set to default system locale, e.g. en_US.UTF-8
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(1), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDiIxFindBigFile.log");
  if (errno != 0) {
    bslf = bslogfiles_free(bslf);
    goto outlog;
  }
  bslog_init(bslf);
outlog:
  bsfatallog_init_fatal_signals();
  errno = 0;
  bslog_set_debug_floor(BS_DEBUGL_DICIDXFIND + 80);
  bslog_set_debug_ceiling(BS_DEBUGL_DICIDXFIND + 80);
  //bslog_add_dbgrange(BS_DEBUGL_DICIDXFIND + 70, BS_DEBUGL_DICIDXFIND + 70);
  sf_test1(argc, argv);
  if (errno != 0) {
    BSLOG_ERR
  }
  bslog_destroy();
  return errno;
}
