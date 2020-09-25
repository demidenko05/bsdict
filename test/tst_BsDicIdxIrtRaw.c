/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Tester for tst_dic1.dsl.</p>
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

//static const int s_max_wrd_len = 11;
static BS_FOFST_T s_offsets_orig[] = {BS_FOFST_NULL, BS_FOFST_NULL, BS_FOFST_NULL};
static BS_SMALL_T s_lens_orig[] = {BS_SMALL_NULL, BS_SMALL_NULL, BS_SMALL_NULL};
static BS_WCHAR_T s_words_orig[3][12];
static BsDicIdxAb *s_idx_ab;

static const int s_dic_entry_buffer_size = 99;

static void sf_print_all_iwords(BsDicIwrds *p_iwrds) {
  BS_WCHAR_T wstr[200];
  bslog_log(BSLONLYMSG, "iwrds sorted%p:\n", p_iwrds);
  for (BS_IDX_T l = BS_IDX_0; l < p_iwrds->size; l++) {
    bsdicidxab_istr_to_wstr(p_iwrds->vals[l]->iword, wstr, p_iwrds->idx_ab);
    bslog_log(BSLONLYMSG, "#"BS_IDX_FMT" %ls offset_dword=%ld length_dword=%d\n", l, wstr, p_iwrds->vals[l]->offset_dword, p_iwrds->vals[l]->length_dword);
  }
}

static void sf_print_all_i2words(BsDicI2wrds *p_iwrds) {
  BS_WCHAR_T wstr[200];
  bslog_log(BSLONLYMSG, "i2wrds sorted%p:\n", p_iwrds);
  for (BS_IDX_T l = BS_IDX_0; l < p_iwrds->size; l++) {
    bsdicidxab_istr_to_wstr(p_iwrds->vals[l]->iword, wstr, p_iwrds->idx_ab);
    bslog_log(BSLONLYMSG, "#"BS_IDX_FMT" %ls iphrase_idx="BS_IDX_FMT"\n", l, wstr, p_iwrds->vals[l]->iphrase_idx);
  }
}

static void sf_print_irt(BsDicIdxIrtRaw *p_irt, BsDicIdxIrtTots *pIrtTots, BsDicIdxAb *pIdx_ab) {
  BS_WCHAR_T wirtwrd[15];
  for (BS_IDX_T l = BS_IDX_0; l < p_irt->bsize; l++) {
    if (p_irt->vals[l] != NULL) {
      bsdicidxab_istr_to_wstr(p_irt->vals[l]->idx_subwrd, wirtwrd, pIdx_ab);
      bslog_log(BSLONLYMSG, "IRTDD#"BS_IDX_FMT" iwrd=%ls, dwolt_start="BS_IDX_FMT", i2wpt_quantity=%d\n", l, wirtwrd, p_irt->vals[l]->dwolt_start, p_irt->vals[l]->i2wpt_quantity);
      if (p_irt->vals[l]->i2wpt_quantity > 0) {
        for (int i = 0; i < p_irt->vals[l]->i2wpt_quantity; i++) {
          bslog_log(BSLONLYMSG, "  i=%d, i2wpt_dwolt_idx[i]="BS_IDX_FMT"\n", i, p_irt->vals[l]->i2wpt_dwolt_idx[i]);
        }
      }
    }
  }
}
int sf_bsdiciwrds_dwrd_csm(BsDicWord *p_dicwrd, BsDicIwrds *p_iwrdssort) {
  for (int i = 0; i < 3; i++) {
    if (s_offsets_orig[i]  == BS_FOFST_NULL) {
      s_offsets_orig[i] = p_dicwrd->offset;
      s_lens_orig[i] = p_dicwrd->lenChrs;
      wcscpy(s_words_orig[i], p_dicwrd->word);
//bslog_log(BSLTEST, "Word for DIWO=%ls, offset=%ld, len=%d\n", p_dicwrd->word, p_dicwrd->offset, p_dicwrd->lenChrs);
      break;
    }
  }
  int rez = bsdiciwrds_dwrd_csm(p_dicwrd, p_iwrdssort);
  return rez;
}

static void sf_test1() {
  char *dic_pth = "tst_dic1.dsl";
  FILE *dic = fopen(dic_pth, "r");
  if (dic == NULL) {
    if (errno == 0) { errno = BSE_ERR; }
    BSLOG_LOG(BSLERROR, "Can't open %s\n", dic_pth);
    return;
  }
  BsDicIwrds *iwrds = NULL;
  BsDicI2wrds *i2wrds = NULL;
  BsDicIdxIrtRaw *irt = NULL;
  BsDicIdxIrtTots *irt_tots = NULL;
  BS_DO_E_OUT(s_idx_ab = bsdicidxab_new(15))
  BSDICIDXABTOTALS_CREATE(ab_tots)
  BS_DO_E_RET(bsdicidxab_iter_dsl_fill(dic, s_dic_entry_buffer_size, s_idx_ab, &ab_tots))
  rewind(dic);
  BS_DO_E_OUT(bsdiciwrds_iter(dic, s_dic_entry_buffer_size, &ab_tots, s_idx_ab, bsdicworddsl_iter_tus,
    sf_bsdiciwrds_dwrd_csm, &iwrds))
  sf_print_all_iwords(iwrds);
  if (ab_tots.i2wptSz > BS_IDX_0) {
    BS_DO_E_OUT(i2wrds = bsdici2wrds_new_fill(ab_tots.i2wptSz, iwrds))
    sf_print_all_i2words(i2wrds);
    sf_print_all_iwords(iwrds);
  }
  //IWORDS
  BS_WCHAR_T dwrds_diwo[3][12];
  BS_FOFST_T fofsts_orig_diwo[3];
  //BS_SMALL_T lens_orig_diwo[3];
  for (BS_IDX_T l = BS_IDX_0; l < iwrds->size; l++) { //without duplicates
    bsdicidxab_istr_to_wstr(iwrds->vals[l]->iword, dwrds_diwo[l], s_idx_ab);
    fofsts_orig_diwo[l] = iwrds->vals[l]->offset_dword;
    //lens_orig_diwo[l] = iwrds->vals[l]->length_dword;
  }
  if (fofsts_orig_diwo[0] != s_offsets_orig[2]) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Offset wrong DIWO 0=%ld ORIG 2=%ld\n", fofsts_orig_diwo[0], s_offsets_orig[2]);
    goto out;
  }
  if (fofsts_orig_diwo[1] != s_offsets_orig[0]) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Offset wrong DIWO 1=%ld ORIG 0=%ld\n", fofsts_orig_diwo[1], s_offsets_orig[0]);
    goto out;
  }
  if (fofsts_orig_diwo[2] != s_offsets_orig[1]) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Offset wrong DIWO 2=%ld ORIG 1=%ld\n", fofsts_orig_diwo[2], s_offsets_orig[1]);
    goto out;
  }
  if (bsdicidx_wstr_cmp(dwrds_diwo[0], s_words_orig[2]) != 0) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Word wrong DIWO 0=%ls ORIG 2=%ls\n", dwrds_diwo[0], s_words_orig[2]);
    goto out;
  }
  if (bsdicidx_wstr_cmp(dwrds_diwo[1], s_words_orig[0]) != 0) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Word wrong DIWO 1=%ls ORIG 0=%ls\n", dwrds_diwo[1], s_words_orig[0]);
    goto out;
  }
  if (bsdicidx_wstr_cmp(dwrds_diwo[2], s_words_orig[1]) != 0) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Word wrong DIWO 2=%ls ORIG 1=%ls\n", dwrds_diwo[2], s_words_orig[1]);
    goto out;
  }
  BS_DO_E_OUT(irt_tots = bsdicidxirttotals_new())
  irt_tots->ab_chars_total = s_idx_ab->chrsTot;
  BS_DO_E_OUT(irt = bsdicidxirtraw_create(iwrds, i2wrds, irt_tots))
  sf_print_irt(irt, irt_tots, s_idx_ab);
  BS_DO_E_OUT(bsdicidxirtraw_validate(irt, irt_tots->irtSz))
out:
  bsdicidxirtraw_free(irt);
  bsdicidxirttotals_free(irt_tots);
  bsdiciwrds_free(iwrds);
  bsdici2wrds_free(i2wrds);
  fclose(dic);
}

static void sf_test2() {
  char *dic_pth = "tst_dic4.dsl";
  FILE *dic = fopen(dic_pth, "r");
  if (dic == NULL) {
    if (errno == 0) { errno = BSE_ERR; }
    BSLOG_LOG(BSLERROR, "Can't open %s\n", dic_pth);
    return;
  }
  BsDicIwrds *iwrds = NULL;
  BsDicI2wrds *i2wrds = NULL;
  BsDicIdxIrtRaw *irt = NULL;
  BsDicIdxIrtTots *irt_tots = NULL;
  BS_DO_E_OUT(BsDicIdxAb *idx_ab = bsdicidxab_new(15))
  BSDICIDXABTOTALS_CREATE(ab_tots)
  BS_DO_E_OUT(bsdicidxab_iter_dsl_fill(dic, s_dic_entry_buffer_size, idx_ab, &ab_tots))
  rewind(dic);
  BS_DO_E_OUT(bsdiciwrds_iter(dic, s_dic_entry_buffer_size, &ab_tots, idx_ab, bsdicworddsl_iter_tus,
    bsdiciwrds_dwrd_csm, &iwrds))
  sf_print_all_iwords(iwrds);
  if (ab_tots.i2wptSz > BS_IDX_0) {
    BS_DO_E_OUT(i2wrds = bsdici2wrds_new_fill(ab_tots.i2wptSz, iwrds))
    sf_print_all_i2words(i2wrds);
    sf_print_all_iwords(iwrds);
  }
  BS_DO_E_OUT(irt_tots = bsdicidxirttotals_new())
  irt_tots->ab_chars_total = idx_ab->chrsTot;
  BS_DO_E_OUT(irt = bsdicidxirtraw_create(iwrds, i2wrds, irt_tots))
  sf_print_irt(irt, irt_tots, idx_ab);
  BS_DO_E_OUT(bsdicidxirtraw_validate(irt, irt_tots->irtSz))
  BS_WCHAR_T wirtwrd[4];
  int adji2wn = 0;
  if (iwrds->size < BDI_MAX_IWORDS_TO_AVOIDI2W) {
    adji2wn = 1;
  }
  bsdicidxab_istr_to_wstr(irt->vals[0]->idx_subwrd, wirtwrd, idx_ab);
  BS_IF_ENM_OUT(bsdicidx_wstr_cmp(wirtwrd, L"c") != 0, BSE_ALG_ERR, "idx subword not c!\n")
  bsdicidxab_istr_to_wstr(irt->vals[1+adji2wn]->idx_subwrd, wirtwrd, idx_ab);
  BS_IF_ENM_OUT(bsdicidx_wstr_cmp(wirtwrd, L"s") != 0, BSE_ALG_ERR, "idx subword not s!\n")
  BS_IF_ENM_OUT(irt->vals[0]->i2wpt_quantity!=0, BSE_ALG_ERR, "irt->vals[0]->i2wpt_quantity!=0\n")
  BS_IF_ENM_OUT(irt->vals[1+adji2wn]->i2wpt_quantity!=1, BSE_ALG_ERR, "irt->vals[1]->i2wpt_quantity!=1\n")
  BS_IF_ENM_OUT(irt->vals[1+adji2wn]->i2wpt_dwolt_idx[0]!=0L, BSE_ALG_ERR, "irt->vals[1]->i2wpt_dwolt_idx[0]!=0L\n")
  //0sent 1send 2"sense of humor" 3"Common sense" 4Sendy 5Sena
  // 3    5         1                4             2      0
  //"Common sense" Sena send Sendy "sense of humor" sent
out:
  bsdicidxirttotals_free(irt_tots);
  bsdicidxirtraw_free(irt);
  bsdiciwrds_free(iwrds);
  bsdici2wrds_free(i2wrds);
  bsdicidxab_free(idx_ab);
  fclose(dic);
}


int main(int argc, char *argv[]) {
  setlocale(LC_ALL, ""); //it set to default system locale, e.g. en_US.UTF-8
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(1), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDicIdxIrtRaw.log");
  if (errno != 0) {
    bslf = bslogfiles_free(bslf);
    goto outlog;
  }
  bslog_init(bslf);
outlog:
  bsfatallog_init_fatal_signals();
  errno = 0;
  //bslog_set_debug_floor(6000);
  //bslog_set_debug_ceiling(6000);
  BS_DO_E_OUT(sf_test1())
  sf_test2(argc);
out:
  if (errno != 0) {
    BSLOG_ERR
  }
  if (s_idx_ab != NULL) {
    bsdicidxab_free(s_idx_ab);
  }
  bslog_destroy();
  return errno;
}
