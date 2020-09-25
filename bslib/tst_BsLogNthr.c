/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Test of BsLog non thread.</p>
 * @author Yury Demidenko
 **/

#include "stdlib.h"

#include "BsError.h"
#include "BsLog.h"


static BsLogConst *s_lconst = NULL;

/**
 * <p>Main program initializes log files
 * and passes file's indexes to clients.</p>
 **/
int main(int argc, char *argv[]) {
  BS_DO_E_RETE(BsLogFiles *bslf=bslogfiles_new(1))
  bslog_files_set_path(bslf, 0, "bslogtestnthr.log");
  if (errno != 0) {
    bslogfiles_free(bslf);
    return errno;
  }
  BS_DO_E_OUT(bslog_init(bslf))
  errno = 0;
  bslog_set_debug_floor(1);
  bslog_set_debug_ceiling(1);
  bslog_add_dbgrange(70, 70);
  BsLogDbgRanges *drngs = bslog_get_dbgranges();
  BS_IF_ENM_OUT(drngs == NULL, BSE_TEST_ERR, "Sub-range NULL!\n")
  BS_IF_ENM_OUT(drngs->size != 1, BSE_TEST_ERR, "Sub-range size != 1!\n")
  BS_IF_ENM_OUT(drngs->vals[0]->floor != 70, BSE_TEST_ERR, "Sub-range[0]floor != 70!\n")
  BS_IF_ENM_OUT(drngs->vals[0]->ceiling != 70, BSE_TEST_ERR, "Sub-range[0]ceiling != 70!\n")
  BS_IF_ENM_OUT(!bslog_is_debug(1), BSE_TEST_ERR, "Main range !1-1\n")
  BS_IF_ENM_OUT(!bslog_is_debug(70), BSE_TEST_ERR, "Sub-range !70-70\n")
  BsLogConst bslc = { .file_index=0, .tag="test2" };
  BS_DO_E_OUT(bslogconst_check(&bslc))
  s_lconst = &bslc;
  for (int i = 0; i < 100; i++) {
    BSLOG_LOG_TO(s_lconst, BSLINFO, "step#%d............................................................\n", i)
  }
out:
  bslog_destroy();
  return errno;
}

