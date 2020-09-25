/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Tester of BsDiIxFind.c.</p>
 * @author Yury Demidenko
 **/

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "locale.h"

#include "BsFatalLog.h"
#include "BsError.h"
#include "BsStrings.h"
#include "BsDiIxFind.h"

static BsDiIxTxRm * sDiIxRm = NULL;

static int sf_str_cmp(char *pCstr1, char *pCstr2, int p_len) {
  for (int i = 0; i < p_len; i++) {
    if (pCstr1[i] != pCstr2[i]) {
      return FALSE;
    }
  }
  return TRUE;
}

static void sf_prn_idx(BsDiIxTxRm *pDiIxRm) {
  bslog_log(BSLONLYMSG, "idx-ram#%p\n", pDiIxRm);
  bslog_log(BSLONLYMSG, "  hirt:\n");
  for (int i = 0; i < pDiIxRm->head->hirtSz; i++) {
    bslog_log(BSLONLYMSG, "    #%d fchar=%lc irtIdx="BS_IDX_FMT" sch_sz=%d\n", i, pDiIxRm->head->ab->wchars[pDiIxRm->head->hirt[i]->fchar - 1], pDiIxRm->head->hirt[i]->irtIdx, pDiIxRm->head->hirt[i]->schrsSz);
    if (pDiIxRm->head->hirt[i]->schrsSz > 0) {
      for (int j = 0; j < pDiIxRm->head->hirt[i]->schrsSz; j++) {
        bslog_log(BSLONLYMSG, "      schar=%lc irtIdx="BS_IDX_FMT"\n", pDiIxRm->head->ab->wchars[pDiIxRm->head->hirt[i]->schars[j] - 1], pDiIxRm->head->hirt[i]->irtIdxs[j]);
      }
    }
  }
  BS_IDX_T l;
  bslog_log(BSLONLYMSG, "  irt:\n", pDiIxRm);
  BS_WCHAR_T wstr[pDiIxRm->head->mxIrWdSz];
  for (l = BS_IDX_0; l < pDiIxRm->head->irtSz; l++) {
    bsdicidxab_istr_to_wstr(pDiIxRm->irt[l]->idx_subwrd, wstr, pDiIxRm->head->ab);
    bslog_log(BSLONLYMSG, "    #"BS_IDX_FMT" %ls dwolt_start="BS_IDX_FMT" i2wpt_quantity=%d i2wpt_start="BS_IDX_FMT"\n", l, wstr, pDiIxRm->irt[l]->dwolt_start, pDiIxRm->irt[l]->i2wpt_quantity, pDiIxRm->irt[l]->i2wpt_start);
  }
  bslog_log(BSLONLYMSG, "  i2wpt:\n", pDiIxRm);
  for (l = BS_IDX_0; l < pDiIxRm->head->i2wptSz; l++) {
    bslog_log(BSLONLYMSG, "    #"BS_IDX_FMT" = "BS_IDX_FMT"\n", l, pDiIxRm->i2wpt[l]);
  }
  bslog_log(BSLONLYMSG, "  dwolt:\n", pDiIxRm);
  for (l = BS_IDX_0; l < pDiIxRm->head->dwoltSz; l++) {
    bslog_log(BSLONLYMSG, "    #"BS_IDX_FMT" ofst=%ld len=%d\n", l, pDiIxRm->dwolt[l]->offset_dword, pDiIxRm->dwolt[l]->length_dword);
  }
}

static void sf_test2() {
  char *dic_pth = "tst_dic4.dsl";
  BsDiIxOst *opSt = NULL; BsDiFdWds *dicWrds = NULL;
  BS_DO_E_OUT (opSt = bsdiixost_new ())
  BS_DO_E_OUT (sDiIxRm = bsdiixtxrm_create (dic_pth, opSt))
  sf_prn_idx (sDiIxRm);
  BS_DO_E_OUT (bsdiixtxrm_validate(sDiIxRm))
  BS_DO_E_OUT (bsdiixtxrm_save(sDiIxRm, dic_pth))
  BS_DO_E_OUT (dicWrds = bsdifdwds_new (BS_IDX_100))
  char *subwrd = "sen";
  //BSLOG_LOG(BSLTEST, "dic=%p, ofst=%ld\n", sDiIxRm->dicFl, ftell(sDiIxRm->dicFl))
  BS_IF_EN_OUT (sDiIxRm->dicFl == NULL, BSE_OPEN_FILE)
  BS_DO_E_OUT (bsdiixtxrmfind_mtch (sDiIxRm, dicWrds, subwrd))
  //0sent 1send 2"sense of humor" 3"Common sense" 4Sendy 5Sena
  // 3    5         1                4             2      0
  //"Common sense" Sena send Sendy "sense of humor" sent
  for ( int i = 0; i < dicWrds->size; i++ )
          { bslog_log(BSLONLYMSG, "dicWrds->vals[%d]->wrd->val = %s\n", i, dicWrds->vals[i]->wrd->val); }
  if ( dicWrds->size != 6 )
          { errno = BSE_ERR; BSLOG_LOG(BSLERROR, "Wrong matched size=%d != 6 (sen)\n", dicWrds->size); }
out:
  bsdiixost_free (opSt);
  fclose (sDiIxRm->dicFl);
  sDiIxRm->dicFl = NULL;
  bsdifdwds_free (dicWrds);
}

static void sf_test3() {
  char *dic_pth = "tst_dic4.dsl";
  BSDICIDXBASE_OPEN_E_RET(diIx, dic_pth)
  BS_DO_E_OUT (BsDiFdWds *dicWrds = bsdifdwds_new (BS_IDX_100))
  char *subwrd = "sen";
  BS_DO_E_OUT(bsdiixtxfind_mtch(diIx, dicWrds, subwrd))
  //0sent 1send 2"sense of humor" 3"Common sense" 4Sendy 5Sena
  // 3    5         1                4             2      0
  //"Common sense" Sena send Sendy "sense of humor" sent
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
  if (dicWrds->size != 6) {
   errno = BSE_ERR; BSLOG_LOG(BSLERROR, "Wrong matched size=%d != 6 (sen)\n", dicWrds->size);
  }
  BS_IF_ENM_OUT (bsstring_compare (diIx->head->nme, sDiIxRm->head->nme), BSE_TEST_ERR, "head->nme wrong!\n")
out:
  bsdifdwds_free (dicWrds);
  bsdiixtx_destroy(diIx);
}

static void sf_test1() {
  char *dic_pth = "tst_dic1.dsl";
  BSDICIDXRAM_OPEN_E_RET (diIxRm, dic_pth)
  BS_DO_E_OUT (BsDiFdWds *dicWrds = bsdifdwds_new (BS_IDX_100))
  char *subwrd = "ящур";
  BS_CHAR_T isubwrd[6];
  BS_DO_E_OUT (bsdicidxab_str_to_istr (subwrd, isubwrd, diIxRm->head->ab))
  if (bsdicidx_istr_len(isubwrd) != 4) {
    errno = BSE_ERR; BSLOG_LOG(BSLERROR, "1Wrong bsdicidx_istr_len(isubwrd) != 4\n");
    goto out;
  }
  BS_IDX_T irtStRn = BS_IDX_0;
  BS_IDX_T irtEnRn = diIxRm->head->irtSz - BS_IDX_1;
  BS_DO_E_OUT(bsdicidx_find_irtrange(diIxRm->head->hirt, diIxRm->head->hirtSz, isubwrd, &irtStRn, &irtEnRn))
  //1-"валяние", 2-"ящур", 0-"бюллетенить"
  BS_IF_ENM_OUT(irtStRn != irtEnRn || irtStRn != 2L || irtEnRn != 2L,
    BSE_ERR, "Wrong irtStRn=irtEnRn!= 2 (ящур)\n")
  BS_DO_E_OUT(BS_IDX_T irtidx = bsdiixrmfindtst_irtix(diIxRm, isubwrd, irtStRn, irtEnRn))
  if (irtidx != 2L) {
    errno = BSE_ERR; BSLOG_LOG(BSLERROR, "Wrong irtIdx=%lu != 2 (ящур)\n", irtidx);
    goto out;
  }
  BS_DO_E_OUT (bsdiixrmfindtst_mtch (diIxRm, isubwrd, irtidx, dicWrds))
  for (int i = 0; i < dicWrds->size; i++) {
    bslog_log(BSLONLYMSG, "dicWrds->vals[%d]->wrd->val = %s\n", i, dicWrds->vals[i]->wrd->val);
  }
  if (!sf_str_cmp("ящур", dicWrds->vals[0]->wrd->val, 8)) {
    errno = BSE_ERR; BSLOG_LOG(BSLERROR, "Wrong ящур != matched->words[0] = %s\n", dicWrds->vals[0]->wrd->val);
  }
out:
  bsdifdwds_free (dicWrds);
  bsdiixtxrm_destroy(diIxRm);
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, ""); //it set to default system locale, e.g. en_US.UTF-8
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(1), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDiIxFind.log");
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
  BS_DO_E_OUT(sf_test1())
  BS_DO_E_OUT(sf_test2())
  sf_test3();
out:
  if (errno != 0) {
    BSLOG_ERR
  }
  bsdiixtxrm_destroy(sDiIxRm);
  bslog_destroy();
  return errno;
}
