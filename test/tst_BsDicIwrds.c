/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Tester of BsDicIwrds.c.</p>
 * @author Yury Demidenko
 **/

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "locale.h"
#include "wctype.h"

#include "BsFatalLog.h"
#include "BsError.h"
#include "BsDicIwrds.h"
#include "BsDicWordDsl.h"

#define MAX_WRD_LEN 14
static BS_FOFST_T s_offsets_orig[] = {BS_FOFST_NULL, BS_FOFST_NULL, BS_FOFST_NULL, BS_FOFST_NULL, BS_FOFST_NULL, BS_FOFST_NULL};
static BS_SMALL_T s_lens_orig[] = {BS_SMALL_NULL, BS_SMALL_NULL, BS_SMALL_NULL, BS_SMALL_NULL, BS_SMALL_NULL, BS_SMALL_NULL};
static BS_WCHAR_T s_words_orig[4][15];
//static BS_WCHAR_T s_words_orig2[6][15];
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

static int sf_bsdiciwrds_dwrd_csm1(BsDicWord *p_dword, BsDicIwrds *p_iwrds) {
  int rez = bsdiciwrds_dwrd_csm(p_dword, p_iwrds);
  BS_IDX_T l = p_iwrds->size - BS_IDX_1;
  wcscpy(s_words_orig[l], p_dword->word);
  for (int i = 0; i < wcslen(s_words_orig[l]); i++)  {
    s_words_orig[l][i] = towlower(s_words_orig[l][i]);
  }
  s_offsets_orig[l] = p_dword->offset;
  s_lens_orig[l] = p_dword->lenChrs;
  bslog_log(BSLONLYMSG, "  DW_CSM   Added origin #"BS_IDX_FMT" %ls offset=%ld len=%d\n",
    l, s_words_orig[l], s_offsets_orig[l], s_lens_orig[l]);
  return rez;
}

static void sf_test1() {
  char *dic_pth = "tst_dic3.dsl";
  FILE *dic = fopen(dic_pth, "r");
  if (dic == NULL) {
    if (errno == 0) { errno = BSE_ERR; }
    BSLOG_LOG(BSLERROR, "Can't open %s\n", dic_pth);
    return;
  }
  BsDicIwrds *iwrds = NULL;
  BsDicI2wrds *i2wrds = NULL;
  BS_DO_E_OUT(s_idx_ab = bsdicidxab_new(15))
  BSDICIDXABTOTALS_CREATE(ab_tots)
  BS_DO_E_OUT(bsdicidxab_iter_dsl_fill(dic, s_dic_entry_buffer_size, s_idx_ab, &ab_tots))
  if (ab_tots.max_iword_len != MAX_WRD_LEN) {
    errno = BSE_ALG_ERR; BSLOG_LOG(BSLERROR, "ab_tots.max_iword_len != MAX_WRD_LEN!\n");
    goto out;
  } else if (ab_tots.dwoltSz != 4) {
    errno = BSE_ALG_ERR; BSLOG_LOG(BSLERROR, "ab_tots.dwoltSz != 4!\n");
    goto out;
  } else if (ab_tots.i2wptSz != 3) {
    errno = BSE_ALG_ERR; BSLOG_LOG(BSLERROR, "1totals.i2wptSz != 3 == %d!\n", ab_tots.i2wptSz);
    goto out;
  }
  //check create_tmpno
  rewind(dic);
  BS_DO_E_OUT(bsdiciwrds_iter(dic, s_dic_entry_buffer_size, &ab_tots, s_idx_ab, bsdicworddsl_iter_tus,
    sf_bsdiciwrds_dwrd_csm1, &iwrds))
  sf_print_all_iwords(iwrds);
  if (ab_tots.i2wptSz > BS_IDX_0) {
    BS_DO_E_OUT(i2wrds = bsdici2wrds_new_fill(ab_tots.i2wptSz, iwrds))
    sf_print_all_i2words(i2wrds);
    sf_print_all_iwords(iwrds);
  }
  int adji2wn = 0;
  if (iwrds->size < BDI_MAX_IWORDS_TO_AVOIDI2W) {
    adji2wn = 1;
  }
  if (iwrds->size != 4+adji2wn) {
    errno = BSE_ALG_ERR; BSLOG_LOG(BSLERROR, "iwrds_sort->total done != 5 "BS_IDX_FMT"!=5!\n", iwrds->size);
    goto out;
  }
  //check array:
  //BSLOG_LOG(BSLTEST, "check array...\n");
  BS_WCHAR_T dwrds_srt[6][MAX_WRD_LEN + 1];
  //dwrds_srt = malloc((4+adji2wn+MAX_WRD_LEN+1)*BS_WCHAR_LEN);
  for (int i = 0; i < 4+adji2wn; i++) {
    if (iwrds->vals[i]->iword == NULL) {
      errno = BSE_ALG_ERR;
      BSLOG_LOG(BSLERROR, "NULL sorted word #%d\n", i);
      goto out;
    } else {
      bsdicidxab_istr_to_wstr(iwrds->vals[i]->iword, dwrds_srt[i], s_idx_ab);
      //BSLOG_LOG(BSLTEST, "sorted word - %ls sorted idx="BS_IDX_FMT"\n", dwrds_srt[i], iwrds_sort->vals[i]->sorted_index);
    }
  }
  //sent send "sense of humor" "Common sense"
  if (iwrds->vals[0L]->offset_dword != s_offsets_orig[3]) {
    errno = BSE_ALG_ERR; BSLOG_LOG(BSLERROR, "Offset wrong IWRDS_SORT 0 ORIG 3\n");
    goto out;
  }
  if (iwrds->vals[1+adji2wn]->offset_dword != s_offsets_orig[1]) {
    errno = BSE_ALG_ERR; BSLOG_LOG(BSLERROR, "Offset wrong IWRDS_SORT 1+adji2wn="BS_IDX_FMT" ORIG 1="BS_IDX_FMT"\n", iwrds->vals[1]->offset_dword, s_offsets_orig[1]);
    goto out;
  }
  if (iwrds->vals[2+adji2wn]->offset_dword != s_offsets_orig[2]) {
    errno = BSE_ALG_ERR; BSLOG_LOG(BSLERROR, "Offset wrong IWRDS_SORT 2+adji2wn ORIG 2\n");
    goto out;
  }
  if (iwrds->vals[3+adji2wn]->offset_dword != s_offsets_orig[0]) {
    errno = BSE_ALG_ERR; BSLOG_LOG(BSLERROR, "Offset wrong IWRDS_SORT 3+adji2wn ORIG 0\n");
    goto out;
  }
  if (bsdicidx_wstr_cmp(dwrds_srt[0], s_words_orig[3]) != 0) {
    errno = BSE_ALG_ERR; BSLOG_LOG(BSLERROR, "Word wrong IWRDS_SORT 0=%ls ORIG 3=%ls\n", dwrds_srt[0], s_words_orig[3]);
    goto out;
  }
  if (bsdicidx_wstr_cmp(dwrds_srt[1+adji2wn], s_words_orig[1]) != 0) {
    errno = BSE_ALG_ERR; BSLOG_LOG(BSLERROR, "Word wrong IWRDS_SORT 1+adji2wn=%ls ORIG 1=%ls\n", dwrds_srt[1+adji2wn], s_words_orig[1]);
    goto out;
  }
  if (bsdicidx_wstr_cmp(dwrds_srt[2+adji2wn], s_words_orig[2]) != 0) {
    errno = BSE_ALG_ERR; BSLOG_LOG(BSLERROR, "Word wrong IWRDS_SORT 2+adji2wn=%ls ORIG 2=%ls\n", dwrds_srt[2+adji2wn], s_words_orig[2]);
    goto out;
  }
  if (bsdicidx_wstr_cmp(dwrds_srt[3+adji2wn], s_words_orig[0]) != 0) {
    errno = BSE_ALG_ERR; BSLOG_LOG(BSLERROR, "Word wrong IWRDS_SORT 3=%ls ORIG 0=%ls\n", dwrds_srt[3+adji2wn], s_words_orig[0]);
    goto out;
  }
out:
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
  BS_DO_E_OUT(BsDicIdxAb *idx_ab = bsdicidxab_new(15))
  BSDICIDXABTOTALS_CREATE(ab_tots)
  BS_DO_E_OUT(bsdicidxab_iter_dsl_fill(dic, s_dic_entry_buffer_size, idx_ab, &ab_tots))
  if (ab_tots.i2wptSz != 5) { //with duplicates
    errno = BSE_ALG_ERR;
    bslog_log(BSLERROR, "2totals.i2wptSz != 5 == %d!\n", ab_tots.i2wptSz);
    goto out;
  }
  //check create_tmpno
  rewind(dic);
  BS_DO_E_OUT(bsdiciwrds_iter(dic, s_dic_entry_buffer_size, &ab_tots, idx_ab, bsdicworddsl_iter_tus, bsdiciwrds_dwrd_csm, &iwrds))
  sf_print_all_iwords(iwrds);
  if (ab_tots.i2wptSz > BS_IDX_0) {
    BS_DO_E_OUT(i2wrds = bsdici2wrds_new_fill(ab_tots.i2wptSz, iwrds))
    sf_print_all_i2words(i2wrds);
    sf_print_all_iwords(iwrds);
  }
  int adji2wn = 0;
  if (iwrds->size < BDI_MAX_IWORDS_TO_AVOIDI2W) {
    adji2wn = 1;
  }
  if (iwrds->size != 6+adji2wn) {
    errno = BSE_ALG_ERR;
    BSLOG_LOG(BSLERROR, "iwrds_sort->total done != 7 "BS_IDX_FMT"!=7!\n", iwrds->size);
    goto out;
  }
  //0sent 1send 2"sense of humor" 3"Common sense" 4Sendy 5Sena
  BS_CHAR_T iswrd[5];
  bsdicidxab_wstr_to_istr(L"sen", iswrd, idx_ab);
  BS_IDX_T idx = bsdiciwrds_find_first(iswrd, iwrds);
  if (idx != 1+adji2wn) { //1->5 sena
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "bsdiciwrds_find_first(sen) !=1 +adji2wn "BS_IDX_FMT"!=1+adji2wn! WORDS:\n", idx);
  }
out:    
  bsdicidxab_free(idx_ab);
  bsdiciwrds_free(iwrds);
  bsdici2wrds_free(i2wrds);
  fclose(dic);
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, ""); //it set to default system locale, e.g. en_US.UTF-8
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(1), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDicIwrds.log");
  if (errno != 0) {
    bslf = bslogfiles_free(bslf);
    goto outlog;
  }
  bslog_init(bslf);
outlog:
  bsfatallog_init_fatal_signals();
  errno = 0;
  //bslog_set_debug_ceiling(999999);
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
