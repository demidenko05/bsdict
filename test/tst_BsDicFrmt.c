/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Tester of BsDicFrmt.c.</p>
 * @author Yury Demidenko
 **/

#include "wchar.h"
#include "locale.h"

#include "BsDicFrmt.h"
#include "BsError.h"
#include "BsFatalLog.h"

static void sf_test1() {
  char *path = "tst_dic1.dsl";
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    if (errno == 0) { errno = BSE_ERR; }
    BSLOG_LOG(BSLERROR, "Can't open %s\n", path);
    return;
  }
  EBsDicFrmts frm = bsdicfrmt_get_format(file);
  fclose(file);
  if (frm != DFRM_DSL) {
    errno = BSE_ERR;
    BSLOG_ERR
    return;
  }
  path = "tst_stardict1.dict";
  file = fopen(path, "r");
  if (file == NULL) {
    if (errno == 0) { errno = BSE_ERR; }
    BSLOG_LOG(BSLERROR, "Can't open %s\n", path);
    return;
  }
  frm = bsdicfrmt_get_format(file);
  fclose(file);
  if (frm != DFRM_STARDICT) {
    errno = BSE_ERR;
    BSLOG_ERR
  }
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "");
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(1), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDicFrmt.log");
  if (errno != 0) {
    bslf = bslogfiles_free(bslf);
    goto outlog;
  }
  bslog_init(bslf);
outlog:
  bsfatallog_init_fatal_signals();
  errno = 0;
  sf_test1(argc);
  if (errno != 0) {
    BSLOG_ERR
  }
  bslog_destroy();
  return errno;
}
