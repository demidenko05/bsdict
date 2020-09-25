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
#include "BsDicIdxAb.h"

static BsDicIdxAb *s_idx_ab;

static int s_dic_entry_buffer_size = 99;

static long s_words_count = 0;

static long s_words_max = BS_FOFST_NULL;

static long s_words_len = 0;

static int s_words_max_size = 0;

static int s_words_bslash_count = 0;

static int sf_bsdicword_consume(BsDicWord *p_dwrd, void *p_null) {
  s_words_count++;
  int len = wcslen(p_dwrd->word);
  BS_DO_E_RETE(bsdicidxab_add_wstr(p_dwrd->word, s_idx_ab))
  if (wcschr(p_dwrd->word, L'\\') != NULL) {
    s_words_bslash_count++;
    //bslog_log(BSLONLYMSG, "%ls  ", p_dwrd->word);
  }
  if (len > s_words_max_size) {
    s_words_max_size = len;
  }
  s_words_len += len;
  if (s_words_count < s_words_max) {
    if (s_words_max > 0) {
      bslog_log(BSLONLYMSG, "%ls\n", p_dwrd->word);
      if (s_words_count == 1) {
        bslog_log(BSLONLYMSG, "first 3 wchar: %x %x %x\n", p_dwrd->word[0], p_dwrd->word[1], p_dwrd->word[2]);
      }
    }
  }
  return 0;
}

static void sf_test1(int argc, char *argv[]) {
  FILE *dic = fopen(argv[1], "r");
  if (dic == NULL) {
    bslog_log(BSLERROR, argv[1]);
    return;
  }
  if (argc >= 3) {
    sscanf(argv[2], "%ld", &s_words_max);
  }
  if (argc >= 4) {
    sscanf(argv[3], "%d", &s_dic_entry_buffer_size);
  }
  s_idx_ab = bsdicidxab_new(300);
  int rez = bsdicworddsl_iter_tus(dic, s_dic_entry_buffer_size, sf_bsdicword_consume, NULL);
  if (!(rez == BSR_OK_ENOUGH || rez == BSR_OK_END)) {
    errno = BSE_ERR;
    BSLOG_LOG(BSLERROR, "!(rez == BSR_OK_ENOUGH || rez == BSR_OK_END) = %d\n", rez)
    goto out;
  }
  bslog_log(BSLONLYMSG, "\ns_words_bslash_count=%d \n", s_words_bslash_count);
out:
  fclose(dic);
  bsdicidxab_free(s_idx_ab);
}

int main(int argc, char *argv[]) {
  if (argc == 0) {
    errno = BSE_WRONG_PARAMS;
    fprintf(stderr, "%s %s\n", __func__, bserror_to_str(errno));
    return errno;
  }
  setlocale(LC_ALL, "");
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(1), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDicWordDslBigest.log");
  if (errno != 0) {
    bslf = bslogfiles_free(bslf);
    goto outlog;
  }
  bslog_init(bslf);
outlog:
  bsfatallog_init_fatal_signals();
  errno = 0;
  sf_test1(argc, argv);
  if (errno != 0) {
    BSLOG_ERR
  }
  bslog_destroy();
  return errno;
}
