/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Tester of BsDicWordDsl.c.</p>
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
#include "BsDicWordDsl.h"

//static const mbstate_t s_mbs;

static const int s_dic_entry_buffer_size = 99;

static void sf_print_wstr(BS_WCHAR_T *pCstr) {
  bslog_log(BSLTEST, "%ls", pCstr);
  bslog_log(BSLTEST, " size=%ld", sizeof(pCstr));
  int str_len = wcslen(pCstr);
  bslog_log(BSLTEST, " len=%d, bytes:\n", str_len);
  int i;
  for (i = 0; i < str_len; i++) {
    bslog_log(BSLTEST, " %x", pCstr[i]);
  }
  unsigned char *lst = (unsigned char*) &pCstr[i - 1];
  bslog_log(BSLTEST, " bytes=%hhx %hhx %hhx %hhx", *lst, *(lst + 1), *(lst + 2), *(lst + 3));
  wint_t upch = towupper(pCstr[i - 1]);
  bslog_log(BSLTEST, " upper=%lc", upch);
  //mbstate_t s_mbs;
  //int mbsr = 0;
  //mbsinit(&s_mbs);
  char chr[3] = {' ', ' ', '\0'};
  //wcrtomb(chr, pCstr[i - 1], &s_mbs);
  wctomb(chr, pCstr[i - 1]);
  bslog_log(BSLTEST, " char=%s\n", chr);
}

static int sf_wstr_cmp(BS_WCHAR_T *pCstr1, BS_WCHAR_T *pCstr2) {
  int str_len = wcslen(pCstr1);
  if (str_len != wcslen(pCstr2)) {
    return FALSE;
  }
  for (int i = 0; i < str_len; i++) {
    if (pCstr1[i] != pCstr2[i]) {
      return FALSE;
    }
  }
  return TRUE;
}

static int sf_str_cmp(char *pCstr1, char *pCstr2, int p_len) {
  for (int i = 0; i < p_len; i++) {
    if (pCstr1[i] != pCstr2[i]) {
      return FALSE;
    }
  }
  return TRUE;
}

static int sf_bsdicword_consume_tsf(BsDicWord *p_dwrd, BsDicWord p_bsdwords[]) {
  for (int i = 0; i < 3; i++) {
    if (p_bsdwords[i].word == NULL) {
      int strl = wcslen(p_dwrd->word);
      p_bsdwords[i].word = malloc((strl + 1) * BS_WCHAR_LEN);
      wcscpy(p_bsdwords[i].word, p_dwrd->word);
      p_bsdwords[i].offset = p_dwrd->offset;
      p_bsdwords[i].lenChrs = p_dwrd->lenChrs;
bslog_log(BSLTEST, "p_dwrd->word=%ls p_dwrd->offset=%ld p_dwrd->lenChrs=%d\n", p_dwrd->word, p_dwrd->offset, p_dwrd->lenChrs);
      return 0;
    }
  }
  errno = BSE_ERR;
  BSLOG_LOG(BSLERROR, "Consume alg err dwrd=%ls, p_bsdwords[0].=%ls, p_bsdwords[0].offset=%ld\n", p_dwrd->word, p_bsdwords[0].word, p_bsdwords[0].offset);
  return errno;
}

static void sf_bsdicworddsl_iter_all(FILE *pDicFl, BsDicWord p_bsdwords[]) {
  bsdicworddsl_iter_tus(pDicFl, s_dic_entry_buffer_size, (BsDicWord_Consume_Tus*) sf_bsdicword_consume_tsf, (void*) p_bsdwords);
}

static void sf_test1() {
  BS_WCHAR_T wrtoe[17]; //"/X\\\(';..;'\)/X\\"
  wrtoe[0] = L'/';
  wrtoe[1] = L'X';
  wrtoe[2] = L'\\';
  wrtoe[3] = L'\\';
  wrtoe[4] = L'\\';
  wrtoe[5] = L'(';
  wrtoe[6] = L';';
  wrtoe[7] = L'.';
  wrtoe[8] = L'.';
  wrtoe[9] = L';';
  wrtoe[10] = L'\'';
  wrtoe[11] = L')';
  wrtoe[12] = L'/';
  wrtoe[13] = L'X';
  wrtoe[14] = L'\\';
  wrtoe[15] = L'\\';
  wrtoe[16] = 0;
  BS_WCHAR_T escp[14]; //"/X\(';..;')/X\"
  escp[0] = L'/';
  escp[1] = L'X';
  escp[2] = L'\\';
  escp[3] = L'(';
  escp[4] = L';';
  escp[5] = L'.';
  escp[6] = L'.';
  escp[7] = L';';
  escp[8] = L'\'';
  escp[9] = L')';
  escp[10] = L'/';
  escp[11] = L'X';
  escp[12] = L'\\';
  escp[13] = 0;
  bslog_log(BSLTEST, "To escape - %ls\n", wrtoe);
  bslog_log(BSLTEST, "Escaped should be - %ls\n", escp);
  bsdicword_escape_bslash(wrtoe);
  if (!sf_wstr_cmp(wrtoe, escp)) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Escaped wrong - %ls\n", wrtoe)
    return;
  }
  /*bslog_log(BSLTEST, "size BS_WCHAR_T=%ld\n", BS_WCHAR_LEN);
  BS_WCHAR_T *sp = L" ";
  unsigned char *lst = (unsigned char*) &sp[0];
  bslog_log(BSLTEST, "space=%x, bytes=%hhx %hhx %hhx %hhx\n", sp[0], *lst, *(lst + 1), *(lst + 2), *(lst + 3));
  BS_WCHAR_T *ap = L"'";
  lst = (unsigned char*) &ap[0];
  bslog_log(BSLTEST, " apostrophe=%x, bytes=%hhx %hhx %hhx %hhx\n", ap[0], *lst, *(lst + 1), *(lst + 2), *(lst + 3));*/
}

static void sf_test2(int argc) {
  char *dic_pth = "tst_dic1.dsl";
  FILE *dic = fopen(dic_pth, "r");
  if (dic == NULL) {
    if (errno == 0) { errno = BSE_ERR; }
    BSLOG_LOG(BSLERROR, "Can't open %s\n", dic_pth);
    return;
  }
  BsDicWord bsdwords[] = { {.word=NULL, .offset=BS_FOFST_NULL, .lenChrs=BS_FOFST_NULL}, {.word=NULL, .offset=BS_FOFST_NULL, .lenChrs=BS_FOFST_NULL}, {.word=NULL, .offset=BS_FOFST_NULL, .lenChrs=BS_FOFST_NULL} };
  //type-safe client
  BS_DO_E_OUT(sf_bsdicworddsl_iter_all(dic, bsdwords))
  BS_WCHAR_T *w123[3] = { L"валяние", L"ящур", L"бюллетенить" };
  BS_SMALL_T wl[3] = { 14, 8, 22 };
  int i;
  for (i = 0; i < 3; i++) {
    if (argc > 1) {
      sf_print_wstr(w123[i]);
      sf_print_wstr(bsdwords[i].word);
    }
    //if (wcscmp(w123[i], bsdwords[i].word)) { //doesn't work
    if (!sf_wstr_cmp(w123[i], bsdwords[i].word)) {
      errno = BSE_ERR;
      BSLOG_LOG(BSLERROR, "Words different w123[i]=%ls bsdwords[i].word=%ls\n", w123[i], bsdwords[i].word);
      goto out;
    }
    if (wl[i] != bsdwords[i].lenChrs) {
      errno = BSE_ERR;
      BSLOG_LOG(BSLERROR, "Length doesn't match i=%d wl[i]=%d bsdwords[i].lenChrs=%d\n", i, wl[i], bsdwords[i].lenChrs);
      goto out;
    }
  }
  //close and reopen dic to switch from WCHAR to CHAR:
  fclose(dic);
  dic = fopen(dic_pth, "r");
  if (dic == NULL) {
    if (errno == 0) { errno = BSE_ERR; }
    BSLOG_LOG(BSLERROR, "Can't reopen %s\n", dic_pth);
    goto out;
  }
  int rz = fseek(dic, bsdwords[1].offset, SEEK_SET);
  if (rz != 0) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Can't seek bsdwords[1].offset=%ld\n", bsdwords[1].offset);
    goto out;
  }
  char wordch[30];
  int len = bsdwords[1].lenChrs;
  rz = fread(wordch, sizeof(char), len, dic);
  if (rz != len) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Can't read bsdwords[1].lenChrs=%d rz=%d\n", len, rz);
    goto out;
  }
  char *wch[3] = { "валяние", "ящур", "бюллетенить" };
  wordch[len] = 0;
  if (!sf_str_cmp(wordch, wch[1], len)) { //strncmp doesn't work
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Wrong chars word wordch=%s, wch[1]=%s, len=%d \n", wordch, wch[1], len);
    goto out;
  }
  BS_WCHAR_T wwch[3][15];
  for (i = 0; i < 3;  i++) {
    int len = strlen(wch[i]);
    int wlen = mbstowcs(wwch[i], wch[i], len);
    int wliov2 = wl[i]/2;
    if (wlen != wliov2) {
      errno = BSE_ERR;
      BSLOG_LOG(BSLERROR, "Wrong wlen=%d, wliov2=%d i=%d\n", wlen, wliov2, i);
      goto out;
    }
    if (!sf_wstr_cmp(wwch[i], w123[i])) {
      errno = BSE_ERR;
      BSLOG_LOG(BSLERROR, "Wrong wwch[i]=%ls, w123[i]=%ls i=%d\n", wwch[i], w123[i], i);
      goto out;
    }
  }
  rz = fseek(dic, bsdwords[2].offset, SEEK_SET);
  if (rz != 0) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Can't seek bsdwords[2].offset=%ld\n", bsdwords[2].offset);
    goto out;
  }
  len = bsdwords[2].lenChrs;
  rz = fread(wordch, sizeof(char), len, dic);
  if (rz != len) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Can't read bsdwords[2].lenChrs=%d rz=%d\n", len, rz);
    goto out;
  }
  wordch[len] = 0;
  if (!sf_str_cmp(wordch, wch[2], len)) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "Wrong chars word wordch=%s, wch[2]=%s, len=%d \n", wordch, wch[2], len);
  }
out:
  if (dic != NULL) {
    fclose(dic);
  }
  for (i = 0; i < 3; i++) {
    if (bsdwords[i].word != NULL) {
      free(bsdwords[i].word);
    }
  }
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "");
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(1), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDicWordDsl.log");
  if (errno != 0) {
    bslf = bslogfiles_free(bslf);
    goto outlog;
  }
  bslog_init(bslf);
outlog:
  bsfatallog_init_fatal_signals();
  errno = 0;
  bslog_set_debug_ceiling(999999);
  BS_DO_E_OUT(sf_test1())
  sf_test2(argc);
out:
  if (errno != 0) {
    BSLOG_ERR
  }
  bslog_destroy();
  return errno;
}
