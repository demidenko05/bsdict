/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

#include "stdlib.h"
#include "wctype.h"
#include "string.h"

#include "BsError.h"
#include "BsFioWrap.h"
#include "BsDicWordDsl.h"
#include "BsDiIxTx.h"

/**
 * <p>Beigesoft™ text dictionary with index final library.</p>
 * @author Yury Demidenko
 **/

//1. Counsructors/destructors/collection utils:
/**
 * <p>Dynamic constructor for further loading from IDX.
 * With empty IRT totals.
 * AB will be created further.
 * HIRT will be allocated by bsdiixheadtx_add_hirtrd
 * according IRT totals.</p>
 * @return pointer to IDX Head or NULL.
 **/
BsDiIxHeadTx*
  bsdiixheadtx_new_tl ()
{
  BsDiIxHeadTx *obj = (BsDiIxHeadTx*) bsdiixheadbs_new (sizeof (BsDiIxHeadTx));
  if ( obj != NULL )
  {
    obj->i2wptSz = BS_IDX_NULL;
    obj->dwoltSz = BS_IDX_NULL;
    obj->frmt = DFRM_UNKNOWN;
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Dynamic constructor for further filling from IWORDSSORTED and IRTRAW.</p>
 * @param p_edic_frmt dictionary format
 * @param p_iwrdssort - ordered d.i.words array
 * @param pIrtTots IRT totals
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHeadTx*
  bsdiixheadtx_new_tf (EBsDicFrmts p_edic_frmt, BsDicIwrds *p_iwrdssort,
                           BsDicIdxIrtTots *pIrtTots)
{
  BsDiIxHeadTx *obj = (BsDiIxHeadTx*) bsdiixheadbs_new (sizeof (BsDiIxHeadTx));
  if (obj != NULL) {
    obj->hirt = malloc (pIrtTots->hirtSz * sizeof(BsDiIxHirtRd*));
    if (obj->hirt == NULL)
    {
      obj = bsdiixheadtx_free (obj);
    } else {
      for ( int i = 0; i < pIrtTots->hirtSz; i++ )
                  { obj->hirt[i] = NULL; }
      obj->ab = p_iwrdssort->idx_ab;
      obj->irtSz = pIrtTots->irtSz;
      obj->hirtSz = pIrtTots->hirtSz;
      obj->mxIrWdSz = pIrtTots->mxIrWdSz;
      obj->i2wptSz = pIrtTots->i2wptSz;
      obj->dwoltSz = p_iwrdssort->dwoltSz;
      obj->frmt = p_edic_frmt;
    }
  }
  if ( obj == NULL )
  {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Dynamic destructor.
 * It frees foreign objects IRT totals and IAB.</p>
 * @param pHead - pointer to IFH
 * @return always NULL
 **/
BsDiIxHeadTx*
  bsdiixheadtx_free (BsDiIxHeadTx *pHead)
{
  if ( pHead != NULL )
          { bsdiixheadbs_free ((BsDiIxHeadBs*) pHead); }
  return NULL;
}

/**
 * <p>Add HIRT record.
 * It will allocate HIRT if it's NULL! TODO violence straight meaning.</p>
 * @param pHead - pointer to IFH
 * @param pRcd HIRT record
 * @set errno if error.
 **/
void
  bsdiixheadtx_add_hirtrd (BsDiIxHeadTx *pHead, BsDiIxHirtRd *pRcd)
{
  bsdiixheadbs_add_hirtrd ((BsDiIxHeadBs*) pHead, pRcd);
}

/**
 * <p>Constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIdxIrtRd *bsdicidxirtrd_new() {
  BsDicIdxIrtRd *obj = malloc(sizeof(BsDicIdxIrtRd));
  if (obj != NULL) {
    obj->idx_subwrd = NULL;
    obj->dwolt_start = BS_IDX_NULL;
    obj->i2wpt_quantity = 0;
    obj->i2wpt_start = BS_IDX_NULL;
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor.</p>
 * @param p_irt_rcd IRT record.
 * @return always NULL
 **/
BsDicIdxIrtRd *bsdicidxirtrd_free(BsDicIdxIrtRd *p_irt_rcd) {
  if (p_irt_rcd != NULL) {
    if (p_irt_rcd->idx_subwrd != NULL) {
      free(p_irt_rcd->idx_subwrd);
    }
    free(p_irt_rcd);
  }
  return NULL;
}

/**
 * <p>Constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDcIxDwoltRd *bsdiixdwoltrd_new() {
  BsDcIxDwoltRd *obj = malloc(sizeof(BsDcIxDwoltRd));
  if (obj != NULL) {
    obj->offset_dword = BS_IDX_NULL;
    obj->length_dword = BS_SMALL_NULL;
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor.</p>
 * @param p_dwolt_rcd DWOLT record.
 * @return always NULL
 **/
BsDcIxDwoltRd *bsdiixdwoltrd_free(BsDcIxDwoltRd *p_dwolt_rcd) {
  if (p_dwolt_rcd != NULL) {
    free(p_dwolt_rcd);
  }
  return NULL;
}

/**
 * <p>Constructor of IDX BASE fillits head from IDX file.</p>
 * @param pDicFl - dictionary
 * @param pIdx_file - dictionary's IDX
 * @param pHead - idx-head with totals and not-yet filled HIRT
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxTx *bsdiixtx_new(FILE *pDicFl, FILE *pIdx_file, BsDiIxHeadTx *pHead) {
  BsDiIxTx *obj = malloc(sizeof(BsDiIxTx));
  if (obj != NULL) {
    obj->head = pHead;
    obj->dicFl = pDicFl;
    obj->idxFl = pIdx_file;
    obj->irtOfst = ftell(pIdx_file);
    BS_IDX_T irtsz = pHead->irtSz * (BDI_IRTRD_FIXED_SIZE(pHead->mxIrWdSz));
    obj->i2wptOfst = obj->irtOfst + irtsz;
    obj->dwoltOfst = obj->i2wptOfst + (pHead->i2wptSz * BS_IDX_LEN);
    BSLOG_LOG(BSLINFO, "Created IDXBASE dicFl#%p idxf#%p irtofst=%ld i2wptofst=%ld dwoltofst=%ld\n", obj->dicFl, obj->idxFl, obj->irtOfst, obj->i2wptOfst, obj->dwoltOfst)
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor include foreign head and closing file.</p>
 * @param pDiIx IDX with head
 * @return always NULL
 **/
BsDiIxTx*
  bsdiixtx_destroy (BsDiIxTx *pDiIx)
{
  if (pDiIx != NULL) {
#ifdef BS_DEBUG_BASIC_ENABLED
    BSLOG_LOG (BSLDEBUG, "Destroying DICIDX#%p\n", pDiIx)
 #endif
    if (pDiIx->dicFl != NULL) {
      fclose(pDiIx->dicFl);
    }
    if (pDiIx->idxFl != NULL) {
      fclose(pDiIx->idxFl);
    }
    if (pDiIx->head != NULL) {
      bsdiixheadtx_free(pDiIx->head);
    }
    free(pDiIx);
  }
  return NULL;
}

/**
 * <p>Constructor of IDX RAM (in memory) to fill from IDX file or IRTRAW and IWORDSSORT.</p>
 * @param pDicFl - dictionary
 * @param pHead - idx-head with totals and not-yet filled HIRT
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxTxRm *bsdiixtxrm_new(FILE *pDicFl, BsDiIxHeadTx *pHead) {
  BsDiIxTxRm *obj = malloc(sizeof(BsDiIxTxRm));
  if (obj != NULL) {
    obj->irt = malloc(pHead->irtSz * sizeof(BsDicIdxIrtRd*));
    if (obj->irt == NULL) {
      obj = bsdiixtxrm_destroy(obj);
      goto out;
    }
    obj->i2wpt = malloc(pHead->i2wptSz * sizeof(BS_IDX_T));
    if (obj->i2wpt == NULL) {
      obj = bsdiixtxrm_destroy(obj);
      goto out;
    }
    obj->dwolt = malloc(pHead->dwoltSz * sizeof(BsDcIxDwoltRd*));
    if (obj->dwolt == NULL) {
      obj = bsdiixtxrm_destroy(obj);
      goto out;
    }
    BS_IDX_T l;
    for (l = BS_IDX_0; l < pHead->irtSz; l++) {
      obj->irt[l] = NULL;
    }
    for (l = BS_IDX_0; l < pHead->i2wptSz; l++) {
      obj->i2wpt[l] = BS_IDX_NULL;
    }
    for (l = BS_IDX_0; l < pHead->dwoltSz; l++) {
      obj->dwolt[l] = NULL;
    }
    obj->head = pHead;
    obj->dicFl = pDicFl;
  }
out:
  if (obj == NULL) {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor of IDX RAM (in memory) and closing file.
 * It frees foreign object head.</p>
 * @param pDiIxRm IDX RAM
 * @return always NULL
 **/
BsDiIxTxRm*
  bsdiixtxrm_destroy (BsDiIxTxRm *pDiIxRm)
{
  if (pDiIxRm != NULL) {
#ifdef BS_DEBUG_BASIC_ENABLED
    BSLOG_LOG (BSLDEBUG, "Destroying DICIDXRAM#%p\n", pDiIxRm)
#endif
    if (pDiIxRm->dicFl != NULL) {
      fclose(pDiIxRm->dicFl);
    }
    BS_IDX_T l;
    if (pDiIxRm->irt != NULL) {
      for (l = 0; l < pDiIxRm->head->irtSz; l++) {
        if (pDiIxRm->irt[l] != NULL) {
          bsdicidxirtrd_free(pDiIxRm->irt[l]);
        }
      }
      free(pDiIxRm->irt);
    }
    if (pDiIxRm->dwolt != NULL) {
      for (l = 0; l < pDiIxRm->head->dwoltSz; l++) {
        if (pDiIxRm->dwolt[l] != NULL) {
          bsdiixdwoltrd_free(pDiIxRm->dwolt[l]);
        }
      }
      free(pDiIxRm->dwolt);
    }
    if (pDiIxRm->i2wpt != NULL) {
      free(pDiIxRm->i2wpt);
    }
    if (pDiIxRm->head != NULL) {
      bsdiixheadtx_free(pDiIxRm->head);
    }
    free(pDiIxRm);
  }
  return NULL;
}

//2. Making IDX services:

/**
 * <p>Fills IDX RAM (in memory) HIRT, IRT, I2WPT and DWOLT with data
 * from IRTRAW and DIWORDSSORTED.</p>
 * @param pDiIxRm IDX RAM.
 * @param p_iwrds - ordered d.i.words array
 * @param p_irtraw - IRT raw in memory
 * @set errno if error.
 **/
void bsdiixtxrm_fill(BsDiIxTxRm *pDiIxRm, BsDicIwrds *p_iwrds,
  BsDicIdxIrtRaw *p_irtraw) {
  BSDIIXHIRTRDMK_CREATE(hirtrdmk, pDiIxRm->head->ab->chrsTot)
  BS_IDX_T tcidx = BS_IDX_0, l;
  for (l = BS_IDX_0; l < pDiIxRm->head->irtSz; l++) {
    if (hirtrdmk.fchar != p_irtraw->vals[l]->idx_subwrd[0]) {
      if (hirtrdmk.fchar != 0) { //next first char
        BSDIIXHIRTRD_NEW_FILL_E_RET(hirtRd, &hirtrdmk)
        BS_DO_E_RET(bsdiixheadtx_add_hirtrd(pDiIxRm->head, hirtRd))
      }
      //new set of words with new first char
      bsdiixhirtrdmk_init(&hirtrdmk);
      hirtrdmk.fchar = p_irtraw->vals[l]->idx_subwrd[0];
      hirtrdmk.irtIdx = l;
    }
    if (p_irtraw->vals[l]->idx_subwrd[1] != 0) {
      BS_DO_E_RET(bsdiixhirtrdmk_make(&hirtrdmk, p_irtraw->vals[l]->idx_subwrd[1], l))
    }
    //IRT:
    BSDICIDXIRTRD_NEW_E_RET(irtrd)
    irtrd->idx_subwrd = p_irtraw->vals[l]->idx_subwrd;
    irtrd->idx_subwrd_size = p_irtraw->vals[l]->idx_subwrd_size;
    p_irtraw->vals[l]->idx_subwrd = NULL;
    irtrd->dwolt_start = p_irtraw->vals[l]->dwolt_start;
    irtrd->i2wpt_quantity = p_irtraw->vals[l]->i2wpt_quantity;
    pDiIxRm->irt[l] = irtrd;
    if (irtrd->i2wpt_quantity > 0) {
      irtrd->i2wpt_start = tcidx;
      //I2WPT:
      int i2wptquani = p_irtraw->vals[l]->i2wpt_quantity;
      for (int i = 0; i < i2wptquani; i++) {
        if (tcidx == pDiIxRm->head->i2wptSz) {
          errno = BSE_OUT_BUFFER_SIZE;
          BSLOG_LOG(BSLERROR, "I2WPT full on l="BS_IDX_FMT", idx="BS_IDX_FMT"\n", l, tcidx);
          return;
        }
        pDiIxRm->i2wpt[tcidx] = p_irtraw->vals[l]->i2wpt_dwolt_idx[i];
        tcidx++;
      }
    }
  }
  //last accumulated HIRT data:
  BSDIIXHIRTRD_NEW_FILL_E_RET(hirtRd, &hirtrdmk)
  BS_DO_E_RET(bsdiixheadtx_add_hirtrd(pDiIxRm->head, hirtRd))
  //DWOLT:
  tcidx = BS_IDX_0;
  for (l = BS_IDX_0; l < p_iwrds->size; l++) {
    if (p_iwrds->vals[l]->length_dword != 0) {
      if (tcidx == pDiIxRm->head->dwoltSz) {
        errno = BSE_OUT_BUFFER_SIZE;
        BSLOG_LOG(BSLERROR, "DWOLT full on l="BS_IDX_FMT", idx="BS_IDX_FMT"\n", l, tcidx);
        return;
      }
      BSDICIDXDWOLTRD_E_RET(dwoltrd)
      dwoltrd->offset_dword = p_iwrds->vals[l]->offset_dword;
      dwoltrd->length_dword = p_iwrds->vals[l]->length_dword;
      pDiIxRm->dwolt[tcidx] = dwoltrd;
      tcidx++;
    }
  }
  BSLOG_LOG(BSLINFO, "IDXRAM#%p has been successfully filled!\n", pDiIxRm);
}

/**
 * <p>Validate IDX RAM (in memory).</p>
 * @param pDiIxRm IDX RAM.
 * @set errno if error.
 **/
void bsdiixtxrm_validate(BsDiIxTxRm *pDiIxRm) {
  if (pDiIxRm == NULL) {
    errno = BSE_VALIDATE_ERR;
    BSLOG_LOG(BSLERROR, "IRTRAM == NULL\n")
    return;
  }
  if (pDiIxRm->dicFl == NULL) {
    errno = BSE_VALIDATE_ERR;
    BSLOG_LOG(BSLERROR, "IRTRAM-dicFl == NULL\n")
    return;
  }
  if (pDiIxRm->head == NULL) {
    errno = BSE_VALIDATE_ERR;
    BSLOG_LOG(BSLERROR, "IRTRAM-head == NULL\n")
    return;
  }
  if (pDiIxRm->head->ab == NULL) {
    errno = BSE_VALIDATE_ERR;
    BSLOG_LOG(BSLERROR, "IRTRAM-head-AB == NULL\n")
    return;
  }
  if (pDiIxRm->irt == NULL) {
    errno = BSE_VALIDATE_ERR;
    BSLOG_LOG(BSLERROR, "IDXRAM-irt == NULL\n")
    return;
  }
  if (pDiIxRm->i2wpt == NULL) {
    errno = BSE_VALIDATE_ERR;
    BSLOG_LOG(BSLERROR, "IRTRAM-i2wpt == NULL\n")
    return;
  }
  if (pDiIxRm->dwolt == NULL) {
    errno = BSE_VALIDATE_ERR;
    BSLOG_LOG(BSLERROR, "IRTRAM-dwolt == NULL\n")
    return;
  }
  BS_DO_E_RET (bsdicidxhirt_validate ( pDiIxRm->head->hirt, pDiIxRm->head->hirtSz, pDiIxRm->head->ab))
  BS_IDX_T l, i2wpt_idx_curr = BS_IDX_0;
  for (l = BS_IDX_0; l < pDiIxRm->head->irtSz; l++) {
    if (pDiIxRm->irt[l] == NULL) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "IDXRAM-irt["BS_IDX_FMT"] == NULL\n", l)
      return;
    }
    if (pDiIxRm->irt[l]->idx_subwrd == NULL) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "IDXRAM-irt["BS_IDX_FMT"]-idx_subwrd == NULL\n", l)
      return;
    }
    if (pDiIxRm->irt[l]->dwolt_start == BS_IDX_NULL && pDiIxRm->irt[l]->i2wpt_quantity <= 0) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "IDXRAM-irt["BS_IDX_FMT"]-dwolt_start == NULL && i2wpt_quantity <= 0\n", l)
      return;
    }
    if (pDiIxRm->irt[l]->i2wpt_start == BS_IDX_NULL && pDiIxRm->irt[l]->i2wpt_quantity > 0) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "IDXRAM-irt["BS_IDX_FMT"]-i2wpt_start == NULL && i2wpt_quantity > 0\n", l)
      return;
    }
    if (pDiIxRm->irt[l]->i2wpt_quantity > 0) {
      if (pDiIxRm->irt[l]->i2wpt_start != i2wpt_idx_curr) {
        errno = BSE_VALIDATE_ERR;
        BSLOG_LOG(BSLERROR, "IDXRAM-irt["BS_IDX_FMT"]-i2wpt_start is "BS_IDX_FMT", must be "BS_IDX_FMT"\n", l, pDiIxRm->irt[l]->i2wpt_start, i2wpt_idx_curr)
        return;
      }
      i2wpt_idx_curr += pDiIxRm->irt[l]->i2wpt_quantity;
    }
  }
  for (l = BS_IDX_0; l < pDiIxRm->head->i2wptSz; l++) {
    if (pDiIxRm->i2wpt[l] == BS_IDX_NULL) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "IRTRAM-i2wpt["BS_IDX_FMT"] == NULL\n", l)
      return;
    }
  }
  for (l = BS_IDX_0; l < pDiIxRm->head->dwoltSz; l++) {
    if (pDiIxRm->dwolt[l] == NULL) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "IRTRAM-dwolt["BS_IDX_FMT"] == NULL\n", l)
      return;
    }
    if (pDiIxRm->dwolt[l]->offset_dword == BS_IDX_NULL) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "IRTRAM-dwolt["BS_IDX_FMT"]-offset_dword == NULL\n", l)
      return;
    }
    if (pDiIxRm->dwolt[l]->length_dword == BS_SMALL_NULL) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "IRTRAM-dwolt["BS_IDX_FMT"]-length_dword == NULL\n", l)
      return;
    }
  }
  BSLOG_LOG(BSLINFO, "IDXRAM#%p has been successfully validated!\n", pDiIxRm);
}

/**
 * <p>Save (write/overwrite) IDX RAM (in memory) into file.</p>
 * @param pDiIxRm IDX RAM.
 * @param pPth - dictionary path.
 * @set errno if error.
 **/
void
  bsdiixtxrm_save (BsDiIxTxRm *pDiIxRm, char *pPth)
{
  //vars:
  FILE *idxFl;
  BS_CHAR_T bschr0;
  BS_IDX_T l;
  int lenr;
  //vars init0:
  bschr0 = 0;
  //Head base:
  BS_DO_E_OUT (idxFl = bsdiixheadbs_save ((BsDiIxHeadBs*) pDiIxRm->head, pPth))
  //rest of totals:
  BS_DO_E_OUT (bsfwrite_bsindex (&pDiIxRm->head->dwoltSz, idxFl))
  BS_DO_E_OUT (bsfwrite_bsindex (&pDiIxRm->head->i2wptSz, idxFl))
  //IRT:
  for ( l = BS_IDX_0; l < pDiIxRm->head->irtSz; l++ )
  {
    if ( pDiIxRm->head->mxIrWdSz == 0 )
    {
      BS_DO_E_OUT (bsfwrite_uchar (&pDiIxRm->irt[l]->idx_subwrd_size, idxFl))
    }
    BS_DO_E_OUT (bsfwrite_bschars (pDiIxRm->irt[l]->idx_subwrd, pDiIxRm->irt[l]->idx_subwrd_size, idxFl))
    if ( pDiIxRm->head->mxIrWdSz > 0 )
    {
      lenr = pDiIxRm->head->mxIrWdSz - pDiIxRm->irt[l]->idx_subwrd_size;
      if ( lenr > 0 )
            { BS_DO_E_OUT (bsfwrite_bscharn(&bschr0, lenr, idxFl)) }
    }
    BS_DO_E_OUT (bsfwrite_bsindex (&pDiIxRm->irt[l]->dwolt_start, idxFl))
    BS_DO_E_OUT (bsfwrite_bssmall (&pDiIxRm->irt[l]->i2wpt_quantity, idxFl))
    BS_DO_E_OUT (bsfwrite_bsindex (&pDiIxRm->irt[l]->i2wpt_start, idxFl))
  }
  //I2WPT:
  for ( l = BS_IDX_0; l < pDiIxRm->head->i2wptSz; l++ )
  {
    BS_DO_E_OUT (bsfwrite_bsindex (&pDiIxRm->i2wpt[l], idxFl))
  }
  //DWOLT:
  for ( l = BS_IDX_0; l < pDiIxRm->head->dwoltSz; l++ )
  {
    BS_DO_E_OUT (bsfwrite_bsfoffset (&pDiIxRm->dwolt[l]->offset_dword, idxFl))
    BS_DO_E_OUT (bsfwrite_bssmall (&pDiIxRm->dwolt[l]->length_dword, idxFl))
  }
  BSLOG_LOG(BSLINFO, "%s with IDXRAM#%p has been successfully saved!\n", pPth, pDiIxRm);
out:
  fclose(idxFl);
}

/**
 * <p>Create IDX RAM (in memory).</p>
 * @param pPth - dictionary path.
 * @param pOpSt - opening state data shared with client
 * @return object or NULL if stopped or error
 * @set errno if error.
 **/
BsDiIxTxRm*
  bsdiixtxrm_create (char *pPth, BsDiIxOst* pOpSt)
{
  char buf[300];
  BsString *nme = NULL;
  BsDicIwrds *iwrds = NULL;
  BsDicI2wrds *i2wrds = NULL;
  BsDicIdxIrtRaw *irt = NULL;
  BsDicIdxAb *idx_ab = NULL;
  BsDicIdxIrtTots *irt_tots = NULL;
  BsDiIxHeadTx *head = NULL;
  BsDiIxTxRm *idx_ram = NULL;
  FILE *dicFl = fopen(pPth, "r");
  BS_IF_EN_RETN (dicFl == NULL, BSE_OPEN_FILE)
  //making IDX:
  //0. Check DIC format
  //1. making AB - reading all d.words(by iterator) from dicFl, adding them into AB, converting them into idx-string
  //   to find max_idxword_length, no need to store them
  //2. making all di.words (in AB coding plus offset original) sorted:
  //3. making IRTRAW with all index words from ordered i/2words
  //4. making IDX-RAM - AB, HIRT, IRT, I2WPT, DWOLT from data in memory IRTRAW and IWRDSSORTEDALL
  //0. Check DIC format:
  BS_DO_E_OUTE(EBsDicFrmts dfmt = bsdicfrmt_get_format(dicFl))
  if ( dfmt != DFRM_DSL )
  {
    errno = BSE_UNIMPLEMENTED;
    BSLOG_LOG(BSLERROR, "Dic formar#%d unimplemented!\n", dfmt);
    goto oute;
  }  
  BsDicIdxAb_Iter_Dwrd_Fill *iter_ab_fill = NULL;

  BsDicWord_Iter_Tus *bsdicword_iter_tus = NULL;

  iter_ab_fill = bsdicidxab_iter_dsl_fill;

  bsdicword_iter_tus = bsdicworddsl_iter_tus;

  //1. making AB:
  BS_DO_E_OUTE(idx_ab = bsdicidxab_new(BDI_AB_BUF_INITSIZE))

  BSDICIDXABTOTALS_CREATE(ab_tots)
  
  if ( pOpSt->stp )
                { goto oute; }

  BS_DO_E_OUTE(iter_ab_fill(dicFl, BS_DIC_ENTRY_BUFFER_SIZE, idx_ab, &ab_tots))

  if ( pOpSt->stp )
                { goto oute; }

  rewind(dicFl);
  //2. making IWRDSSORTALL
  BS_DO_E_OUTE (bsdiciwrds_iter(dicFl, BS_DIC_ENTRY_BUFFER_SIZE,
          &ab_tots, idx_ab, bsdicword_iter_tus, bsdiciwrds_dwrd_csm, &iwrds))

  pOpSt->prgr = 60;

  if ( pOpSt->stp )
                { goto oute; }

  if ( ab_tots.i2wptSz > BS_IDX_0 )
  {
    BS_DO_E_OUTE(i2wrds = bsdici2wrds_new_fill(ab_tots.i2wptSz, iwrds))
  }

  if ( pOpSt->stp )
                { goto oute; }
  //3. making IRTRAW
  BS_DO_E_OUTE(irt_tots = bsdicidxirttotals_new())

  irt_tots->ab_chars_total = idx_ab->chrsTot;

  BS_DO_E_OUTE(irt = bsdicidxirtraw_create(iwrds, i2wrds, irt_tots))

  if ( pOpSt->stp )
                { goto oute; }

  BS_DO_E_OUTE(bsdicidxirtraw_validate(irt, irt_tots->irtSz))

  //4. making IDXRAM
  BS_DO_E_OUTE(head = bsdiixheadtx_new_tf(dfmt, iwrds, irt_tots))
  fclose (dicFl); //reopen for further char reading
  dicFl = fopen(pPth, "r");
  if ( dfmt == DFRM_DSL )
  {
    rewind (dicFl);
    int rz = fscanf (dicFl, "%*299[^#]");
    rz = fscanf (dicFl, "#NAME \"%299[^\"\n]", buf);
    if ( rz == 1 )
            { BS_DO_E_OUTE (nme = bsstring_new (buf)) }
  }
  if ( nme == NULL )
  { //file name:
    char *nm = strrchr (pPth, '/');
    if ( nm != NULL )
    {
      nm = nm + 1;
    } else {
      nm = pPth;
    }
    BS_DO_E_OUTE (nme = bsstring_new (nm))
  }
  head->nme = nme;
  nme = NULL;

  if ( pOpSt->stp )
                { goto oute; }

  BS_DO_E_OUTE(idx_ram = bsdiixtxrm_new(dicFl, head))

  BS_DO_E_OUTE(bsdiixtxrm_fill(idx_ram, iwrds, irt))

  BSLOG_LOG (BSLINFO, "Created DIC IDX RAM #%p, name=%s\n", idx_ram, idx_ram->head->nme->val)
  return idx_ram;

oute: //or canceled
  bsstring_free (nme);
  bsdiciwrds_free (iwrds);
  bsdici2wrds_free (i2wrds);
  bsdicidxirtraw_free (irt);

  if ( idx_ram != NULL )
  {
    bsdiixtxrm_destroy (idx_ram);
  } else {
    fclose(dicFl);
    if ( head != NULL )
    {
      bsdiixheadtx_free (head);
    } else {
      bsdicidxab_free (idx_ab);
      bsdicidxirttotals_free (irt_tots);
    }
  }
  return NULL;
}

/**
 * <p>Load IDX RAM (in memory) from IDX file.</p>
 * @param pPth - dictionary path.
 * @return object or NULL if error or if IDX file not found
 * @set errno if error. "IDX file not found" is not error!
 **/
BsDiIxTxRm* bsdiixtxrm_load(char *pPth) {
  FILE *dicFl = NULL;
  FILE *idxFl = NULL;
  BsDiIxHeadTx *head = NULL;
  BS_DO_E_RETN(bsdiixheadtx_load(pPth, &head, &dicFl, &idxFl))
  if (head == NULL) { //no IDX
    return NULL;
  }
  BS_DO_E_OUTE(BsDiIxTxRm *idx_ram = bsdiixtxrm_new(dicFl, head))
  BS_IDX_T l;
  //IRT:
  for (l = BS_IDX_0; l < idx_ram->head->irtSz; l++) {
    BS_DO_E_OUTE(idx_ram->irt[l] = bsdicidxirtrd_new())
    if ( head->mxIrWdSz > 0 )
    {
      idx_ram->irt[l]->idx_subwrd_size = head->mxIrWdSz;
    } else {
      BS_DO_E_OUTE (bsfread_uchar(&idx_ram->irt[l]->idx_subwrd_size, idxFl))
    }
    idx_ram->irt[l]->idx_subwrd = malloc(idx_ram->irt[l]->idx_subwrd_size * BS_WCHAR_LEN);
    if (idx_ram->irt[l]->idx_subwrd == NULL) {
      if ( errno == 0 ) { errno = ENOMEM; }
      BSLOG_ERR
      goto oute;
    }
    BS_DO_E_OUTE(bsfread_bschars(idx_ram->irt[l]->idx_subwrd, idx_ram->irt[l]->idx_subwrd_size, idxFl))
    if ( head->mxIrWdSz > 0 )
    {
      idx_ram->irt[l]->idx_subwrd_size = bsdicidx_istr_len(idx_ram->irt[l]->idx_subwrd + 1);
    }
    BS_DO_E_OUTE(bsfread_bsindex(&idx_ram->irt[l]->dwolt_start, idxFl))
    BS_DO_E_OUTE(bsfread_bssmall(&idx_ram->irt[l]->i2wpt_quantity, idxFl))
    BS_DO_E_OUTE(bsfread_bsindex(&idx_ram->irt[l]->i2wpt_start, idxFl))
  }
  //I2WPT:
  for (l = BS_IDX_0; l < idx_ram->head->i2wptSz; l++) {
    BS_DO_E_OUTE(bsfread_bsindex(&idx_ram->i2wpt[l], idxFl))
  }
  //DWOLT:
  for (l = BS_IDX_0; l < idx_ram->head->dwoltSz; l++) {
    BS_DO_E_OUTE(idx_ram->dwolt[l] = bsdiixdwoltrd_new())
    BS_DO_E_OUTE(bsfread_bsfoffset(&idx_ram->dwolt[l]->offset_dword, idxFl))
    BS_DO_E_OUTE(bsfread_bssmall(&idx_ram->dwolt[l]->length_dword, idxFl))
  }
  fclose(idxFl);
  return idx_ram;
oute:
  if (idx_ram != NULL) {
    bsdiixtxrm_destroy(idx_ram);
  } else {
    if (dicFl != NULL) {
      fclose(dicFl);
    }
    if (head != NULL) {
      bsdiixheadtx_free(head);
    }
  }
  if (idxFl != NULL) {
    fclose(idxFl);
  }
  return NULL;
}

/**
 * <p>Load IDX Base (head) from IDX file.</p>
 * @param pPth - dictionary path.
 * @param pHeadRt - pointer to return filled head
 * @param pDicFlRt - pointer to return opened dictionary
 * @param pIdxFlRt - pointer to return opened IDX file
 * @set errno if error. "IDX file not found" is not error!
 **/
void
  bsdiixheadtx_load (char *pPth, BsDiIxHeadTx **pHeadRt,
                     FILE **pDicFlRt, FILE **pIdxFlRt)
{
  //vars:
  FILE *dicFl, *idxFl;
  BsDiIxHeadTx *head;
    //init0 vars:
  dicFl = NULL; idxFl = NULL;
  //code:
  BS_DO_E_OUTE (head = bsdiixheadtx_new_tl ())
  BS_DO_E_OUTE (bsdiixheadbs_load (pPth, (BsDiIxHeadBs*) head, &dicFl, &idxFl))
  if ( idxFl == NULL ) //not found
  {
    bsdiixheadtx_free (head);
    return;
  }
  //rest totals:
  BS_DO_E_OUTE (bsfread_bsindex (&head->dwoltSz, idxFl))
  BS_DO_E_OUTE (bsfread_bsindex (&head->i2wptSz, idxFl))

  *pHeadRt = head;
  *pDicFlRt = dicFl;
  *pIdxFlRt = idxFl;
  return;

oute:
  if (dicFl != NULL)
              { fclose (dicFl); }
  if (idxFl != NULL)
              { fclose (idxFl); }
  if (head != NULL)
              { bsdiixheadtx_free (head); }
}

/**
 * <p>Load IDX Base (head) from IDX file.</p>
 * @param pPth - dictionary path.
 * @return object or NULL if error or if IDX file not found
 * @set errno if error. "IDX file not found" is not error!
 **/
BsDiIxTx*
  bsdiixtx_load (char *pPth)
{
  FILE *dicFl = NULL;
  FILE *idxFl = NULL;
  BsDiIxHeadTx *head = NULL;
  BS_DO_E_RETN (bsdiixheadtx_load (pPth, &head, &dicFl, &idxFl))
  if (head == NULL) { //no IDX
    return NULL;
  }
  BS_DO_E_OUTE (BsDiIxTx *diIx = bsdiixtx_new (dicFl, idxFl, head))
  return diIx;

oute:
  if ( dicFl != NULL )
        { fclose (dicFl); }
  if ( idxFl != NULL )
        { fclose(idxFl); }
  if ( head != NULL )
        { bsdiixheadtx_free (head); }
  return NULL;
}

/**
 * <p>Open DIC IDX, i.e. load or create then load.</p>
 * @param pPth - dictionary path.
 * @param pOpSt - opening state data shared with client
 * @param pIsIxRm - client prefers IRT (index records table) in memory (RAM) than in file
 * @return object or NULL if error
 * @set errno if error.
 **/
BsDiIxTxBs*
  bsdiixtx_open (char *pPth, BsDiIxOst* pOpSt, bool pIsIxRm)
{
  BsDiIxTx *diIx;
  BsDiIxTxRm *diIxRm;
  diIx = NULL;
  diIxRm = NULL;
  if ( !pIsIxRm )
  {
    BS_DO_E_OUTE (diIx = bsdiixtx_load (pPth))
    if ( diIx != NULL )
    {
      pOpSt->prgr = 100;
      pOpSt->stt = EBSDS_OPENED;
      return (BsDiIxTxBs *) diIx;
    }
  } else {
    BS_DO_E_OUTE (diIxRm = bsdiixtxrm_load (pPth))
    if ( diIxRm != NULL )
    {
      pOpSt->prgr = 100;
      pOpSt->stt = EBSDS_OPENED;
      return (BsDiIxTxBs *) diIxRm;
    }
  }

  pOpSt->stt = EBSDS_INDEXING;

  BS_DO_E_OUTE (diIxRm = bsdiixtxrm_create (pPth, pOpSt))

  BS_DO_E_OUTE (bsdiixtxrm_validate (diIxRm))

  BS_DO_E_OUTE (bsdiixtxrm_save (diIxRm, pPth))

  diIxRm = bsdiixtxrm_destroy (diIxRm);

  
  if ( !pIsIxRm )
  {
    BS_DO_E_OUTE (diIx = bsdiixtx_load (pPth))
    if ( diIx != NULL )
    {
      pOpSt->prgr = 100;
      pOpSt->stt = EBSDS_OPENED;
      return (BsDiIxTxBs *) diIx;
    }
  } else {
    BS_DO_E_OUTE (diIxRm = bsdiixtxrm_load (pPth))
    if ( diIx != NULL )
    {
      pOpSt->prgr = 100;
      pOpSt->stt = EBSDS_OPENED;
      return (BsDiIxTxBs *) diIxRm;
    }
  }

oute:
  bsdiixtxrm_destroy (diIxRm);
  bsdiixtx_destroy (diIx);
  pOpSt->stt = EBSDS_ERROR;
  return NULL;
}
