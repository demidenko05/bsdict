/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Tester#1 of BsDiIxTxRm.c.</p>
 * @author Yury Demidenko
 **/

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "locale.h"

#include "BsFatalLog.h"
#include "BsError.h"
#include "BsDiIxTx.h"

static void sf_test_idx_data_dic1dsl(BsDiIxTxRm *pDiIxRm) {
  if (pDiIxRm->head->dwoltSz != 3) {
    errno = BSE_ERR; bslog_log(BSLERROR, "Wrong dicidxdata.head->dwoltSz != 3 == %d\n", pDiIxRm->head->dwoltSz);
    return;
  }
  if (pDiIxRm->head->hirtSz != 3) {
    errno = BSE_ERR; bslog_log(BSLERROR, "Wrong pDiIxRm->head->hirtSz != 3 == %d\n", pDiIxRm->head->hirtSz);
    return;
  }
  if (pDiIxRm->head->irtSz != 3) {
    errno = BSE_ERR; bslog_log(BSLERROR, "Wrong pDiIxRm->head->irtSz != 3 == %d\n", pDiIxRm->head->irtSz);
    return;
  }
  if (pDiIxRm->head->mxIrWdSz != 2) {
    errno = BSE_ERR; bslog_log(BSLERROR, "Wrong pDiIxRm->head->mxIrWdSz != 2 == %d\n", pDiIxRm->head->mxIrWdSz);
    return;
  }
  if (pDiIxRm->head->i2wptSz != 0) {
    errno = BSE_ERR; bslog_log(BSLERROR, "Wrong pDiIxRm->head->i2wptSz != 0 == %d\n", pDiIxRm->head->i2wptSz);
    return;
  }
  if (pDiIxRm->head->hirt[0]->schrsSz != 0) {
    errno = BSE_ERR; bslog_log(BSLERROR, "Wrong pDiIxRm->head->hirt[0]->schrsSz!= 0 == %d\n", pDiIxRm->head->hirt[0]->schrsSz);
    return;
  }
  if (pDiIxRm->head->hirt[0]->schars != NULL) {
    errno = BSE_ERR; bslog_log(BSLERROR, "Wrong pDiIxRm->head->hirt[0]->schars!= NULL\n");
    return;
  }
  if (pDiIxRm->head->hirt[0]->irtIdxs != NULL) {
    errno = BSE_ERR; bslog_log(BSLERROR, "Wrong pDiIxRm->head->hirt[0]->irtIdxs != NULL\n");
    return;
  }
}

static BsDiIxTxRm *sDiIxRm = NULL;

static char *s_dic_pth = "tst_dic1.dsl";

static void sf_test_write() {
  BS_DO_E_RET(BsDiIxOst *opSt = bsdiixost_new ())
  BS_DO_E_OUT(sDiIxRm= bsdiixtxrm_create(s_dic_pth, opSt))
  BS_DO_E_OUT(bsdiixtxrm_validate(sDiIxRm))
  BS_DO_E_OUT(bsdiixtxrm_save(sDiIxRm, s_dic_pth))
  BS_DO_E_OUT(sf_test_idx_data_dic1dsl(sDiIxRm))
out:
  bsdiixost_free (opSt);
}

static void sf_test_read() {
  BSDICIDXRAM_OPEN_E_RET(idx_ram, s_dic_pth)
  int i;
  if (sDiIxRm->head->frmt != idx_ram->head->frmt) {
    errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->frmt != idx_ram->head->frmt: %d!=%d\n", sDiIxRm->head->frmt, idx_ram->head->frmt);
    goto out;
  }
  if (DFRM_DSL != idx_ram->head->frmt) {
    errno = BSE_ERR; bslog_log(BSLERROR, "DFRM_DSL != idx_ram->head->frmt: %d!=%d\n", DFRM_DSL, idx_ram->head->frmt);
    goto out;
  }
  if (sDiIxRm->head->ab->chrsTot != idx_ram->head->ab->chrsTot) {
    errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->ab->chrsTot != idx_ram->head->ab->chrsTot: %d!=%d\n", sDiIxRm->head->ab->chrsTot, idx_ram->head->ab->chrsTot);
    goto out;
  } else {
    if (sDiIxRm->head->ab->ispace != idx_ram->head->ab->ispace) {
      errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->ab->ispace != idx_ram->head->ab->ispace: %d!=%d\n", sDiIxRm->head->ab->ispace, idx_ram->head->ab->ispace);
      goto out;
    }
    for (i = 0; i < idx_ram->head->ab->chrsTot; i++) {
      if (sDiIxRm->head->ab->wchars[i] != idx_ram->head->ab->wchars[i]) {
        errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->ab->wchars[i] != idx_ram->head->ab->wchars[i]: %d!=%d i=%d\n", sDiIxRm->head->ab->wchars[i], idx_ram->head->ab->wchars[i], i);
        goto out;
        break;
      }
    }
  }
  if (sDiIxRm->head->hirtSz != idx_ram->head->hirtSz) {
    errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->hirtSz != idx_ram->head->hirtSz: %d!=%d\n", sDiIxRm->head->hirtSz, idx_ram->head->hirtSz);
    goto out;
  }
  if (sDiIxRm->head->irtSz != idx_ram->head->irtSz) {
    errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->irtSz != idx_ram->head->irtSz: %d!=%d\n", sDiIxRm->head->irtSz, idx_ram->head->irtSz);
    goto out;
  }
  if (sDiIxRm->head->i2wptSz != idx_ram->head->i2wptSz) {
    errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->i2wptSz != idx_ram->head->i2wptSz: %d!=%d\n", sDiIxRm->head->i2wptSz, idx_ram->head->i2wptSz);
    goto out;
  }
  if (sDiIxRm->head->dwoltSz != idx_ram->head->dwoltSz) {
    errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->dwoltSz != idx_ram->head->dwoltSz: %d!=%d\n", sDiIxRm->head->dwoltSz, idx_ram->head->dwoltSz);
    goto out;
  }
  for (i = 0; i < idx_ram->head->hirtSz; i++) {
    if (sDiIxRm->head->hirt[i]->fchar != idx_ram->head->hirt[i]->fchar) {
      errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->hirt[i]->fchar != idx_ram->head->hirt[i]->fchar: %d!=%d i=%d\n", sDiIxRm->head->hirt[i]->fchar, idx_ram->head->hirt[i]->fchar, i);
      goto out;
    }
    if (sDiIxRm->head->hirt[i]->irtIdx != idx_ram->head->hirt[i]->irtIdx) {
      errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->hirt[i]->irtIdx != idx_ram->head->hirt[i]->irtIdx: %d!=%d i=%d\n", sDiIxRm->head->hirt[i]->irtIdx, idx_ram->head->hirt[i]->irtIdx, i);
      goto out;
    }
    if (BS_IDX_NULL == idx_ram->head->hirt[i]->irtIdx) {
      errno = BSE_ERR; bslog_log(BSLERROR, "BS_IDX_NULL == idx_ram->head->hirt[i]->irtIdx: %d!=%d i=%d\n", BS_IDX_NULL, idx_ram->head->hirt[i]->irtIdx, i);
      goto out;
    }
    for (int j = 0; j < idx_ram->head->hirt[i]->schrsSz; j++) {
      if (sDiIxRm->head->hirt[i]->schars[j] != idx_ram->head->hirt[i]->schars[j]) {
        errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->hirt[i]->schars[j] != idx_ram->head->hirt[i]->schars[j]: %d!=%d i=%d j=%d\n", sDiIxRm->head->hirt[i]->schars[j], idx_ram->head->hirt[i]->schars[j], i, j);
        goto out;
      }
      if (sDiIxRm->head->hirt[i]->irtIdxs[j] != idx_ram->head->hirt[i]->irtIdxs[j]) {
        errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->hirt[i]->irtIdxs[j] != idx_ram->head->hirt[i]->irtIdxs[j]: %d!=%d i=%d j=%d\n", sDiIxRm->head->hirt[i]->irtIdxs[j], idx_ram->head->hirt[i]->irtIdxs[j], i, j);
        goto out;
      }
    }
  }
  if (sDiIxRm->head->irtSz != idx_ram->head->irtSz) {
    errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->irtSz != idx_ram->head->irtSz: %d!=%d\n", sDiIxRm->head->irtSz, idx_ram->head->irtSz);
    goto out;
  }
  if (sDiIxRm->head->i2wptSz != idx_ram->head->i2wptSz) {
    errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->i2wptSz != idx_ram->head->i2wptSz: %d!=%d\n", sDiIxRm->head->i2wptSz, idx_ram->head->i2wptSz);
    goto out;
  }
  if (sDiIxRm->head->dwoltSz != idx_ram->head->dwoltSz) {
    errno = BSE_ERR; bslog_log(BSLERROR, "sDiIxRm->head->dwoltSz != idx_ram->head->dwoltSz: %d!=%d\n", sDiIxRm->head->dwoltSz, idx_ram->head->dwoltSz);
    goto out;
  }
out:
  bsdiixtxrm_destroy(idx_ram);
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, ""); //it set to default system locale, e.g. en_US.UTF-8
  BS_DO_E_GOTO(BsLogFiles *bslf=bslogfiles_new(1), outlog)
  bslog_files_set_path(bslf, 0, "tst_BsDiIxTx.log");
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
  BS_DO_E_OUT(sf_test_write())
  sf_test_read();
out:
  if (errno != 0) {
    BSLOG_ERR
  }
  bsdiixtxrm_destroy(sDiIxRm);
  bslog_destroy();
  return errno;
}
