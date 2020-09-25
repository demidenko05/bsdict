/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

#include "stdlib.h"
#include "wctype.h"
#include "string.h"

#include "BsError.h"
#include "BsFioWrap.h"
#include "BsDiIx.h"

/**
 * <p>Beigesoft™ dictionary (text/audio/both...) with index  basic, dictionary file strusture independent library.</p>
 * @author Yury Demidenko
 **/

//1. Counsructors/destructors/collection utils:

  //1.1 IDX file structure:
/**
 * <p>Constructor from file data.</p>
 * @param schrsSz size of HIRT record's schars array
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHirtRd*
  bsdicidxhirtrd_new (int pSchrsSz)
{
  if ( pSchrsSz < 0 )
  {
    errno = BSE_ARR_WPSIZE;
    BSLOG_LOG (BSLERROR, "SIZE schrsSz %d", pSchrsSz)
    return NULL;
  }
  BsDiIxHirtRd *obj = malloc (sizeof (BsDiIxHirtRd));
  if ( obj != NULL )
  {
    obj->fchar = 0;
    obj->schrsSz = pSchrsSz;
    obj->irtIdx = BS_IDX_NULL;
    obj->schars = NULL;
    obj->irtIdxs = NULL;
    if ( pSchrsSz > 0 )
    {
      obj->schars = malloc (BS_CHAR_LEN * pSchrsSz);
      if ( obj->schars == NULL )
      {
        obj = bsdicidxhirtrd_free (obj);
        goto out;
      }
      obj->irtIdxs = malloc (BS_IDX_LEN * pSchrsSz);
      if ( obj->irtIdxs == NULL )
      {
        obj = bsdicidxhirtrd_free (obj);
        goto out;
      }
      for ( int i = 0; i < pSchrsSz; i++ )
      {
        obj->schars[i] = 0;
        obj->irtIdxs[i] = BS_IDX_NULL;
      }
    }
  }
out:
  if ( obj == NULL )
  {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Only dynamic destructor.</p>
 * @param pRcd - HIRT index file head fixed-size record
 * @return always NULL
 **/
BsDiIxHirtRd *bsdicidxhirtrd_free(BsDiIxHirtRd *pRcd) {
  if (pRcd != NULL) {
    if (pRcd->schars != NULL) {
      free(pRcd->schars);
    }
    if (pRcd->irtIdxs != NULL) {
      free(pRcd->irtIdxs);
    }
    free(pRcd);
  }
  return NULL;
}

/**
 * <p>Create and fill HIRTRD from given BsDiIxHirtRdMk.</p>
 * @param pRcdmk HIRT record's schars array
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHirtRd* bsdicidxhirtrd_new_fill(BsDiIxHirtRdMk *pRcdmk) {
  int i, schsz = pRcdmk->schrsSz;
  for(i = 0; i < pRcdmk->schrsSz; i++) {
    if (pRcdmk->schars[i] == 0) {
      schsz = i - 1;
      break;
    }
  }
  if (schsz < 1) {
    schsz = 0;
  }
  BsDiIxHirtRd *obj = bsdicidxhirtrd_new(schsz);
  if (obj != NULL) {
    obj->fchar = pRcdmk->fchar;
    obj->irtIdx = pRcdmk->irtIdx;
    for(i = 0; i < obj->schrsSz; i++) {
      obj->schars[i] = pRcdmk->schars[i];
      obj->irtIdxs[i] = pRcdmk->irtIdxs[i];
    }
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Generic consructor to load from IDX file.</p>
 * @param pObSz - object size, e.g. sizeof (BsDiIxHeadTx)
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHeadBs*
  bsdiixheadbs_new (size_t pObSz)
{
  BsDiIxHeadBs *obj = malloc (pObSz);
  if ( obj != NULL )
  {
    obj->nme = NULL;
    obj->ab = NULL;
    obj->hirt = NULL;
    obj->irtSz = BS_IDX_NULL;
    obj->isIxRm = false;
    obj->hirtSz = -1;
    obj->mxIrWdSz = -1;
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Generic destructor.</p>
 * @param pHead - pointer to IFH
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHeadBs*
  bsdiixheadbs_free (BsDiIxHeadBs *pHead)
{
  if ( pHead != NULL )
  {
    if ( pHead->hirt != NULL )
    {
      for ( int i = 0; i < pHead->hirtSz; i++ )
      {
        if ( pHead->hirt[i] != NULL )
              { bsdicidxhirtrd_free (pHead->hirt[i]); }
      }
      free(pHead->hirt);
    }
    if ( pHead->ab != NULL )
              { bsdicidxab_free (pHead->ab); }
    if ( pHead->nme != NULL )
              { bsstring_free (pHead->nme); }
    free (pHead);
  }
  return NULL;
}

  //1.2 Searching:

/**
 * <p>Only constructor.</p>
 * @param pDiIx opened dic with index NOT NULL
 * @param pOfst offset of d.word content
 * @return object or NULL when error
 * @set errno - ENOMEM 
 **/
BsDiSrDt1*
  bsdisrdt1_new (BsDiIxBs *pDiIx, BS_FOFST_T pOfst)
{
  BsDiSrDt1 *obj = malloc (sizeof (BsDiSrDt1));
  if (obj != NULL)
  {
    obj->diIx = pDiIx;
    obj->ofst = pOfst;
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor.</p>
 * @param pDicOfst maybe NULL
 * @return always NULL
 **/
BsDiSrDt1*
  bsdisrdt1_free (BsDiSrDt1 *pDicOfst)
{
  if ( pDicOfst != NULL )
  {
    free (pDicOfst);
  }
  return NULL;
}

/**
 * <p>Only constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiSrDt1s*
  bsdisrdt1s_new (BS_IDX_T pBufSz)
{
  return (BsDiSrDt1s*) bsdatasettus_new (sizeof (BsDiSrDt1s), pBufSz);
}

/**
 * <p>Destructor.</p>
 * @param pDicOfsts maybe NULL
 * @return always NULL
 **/
BsDiSrDt1s*
  bsdisrdt1s_free (BsDiSrDt1s *pDicOfsts)
{
  bsdatasettus_free ((BsDataSetTus*) pDicOfsts, (Bs_Destruct*) bsdisrdt1_free);
  return NULL;
}


/**
 * <p>Add object to the first null cell of collection.
 * It increases collection if it's full-filled.</p>
 * @param pDicOfstss - collection
 * @param pDiIx opened dic with index NOT NULL
 * @param pOfst offset of d.word content
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE or ENOMEM
 **/
BS_IDX_T
  bsdisrdt1s_add_inc (BsDiSrDt1s *pDicOfsts, BsDiIxBs *pDiIx, BS_FOFST_T pOfst)
{
  BsDiSrDt1 *obj = bsdisrdt1_new (pDiIx, pOfst);
  if ( obj == NULL )
              { return BS_IDX_NULL; }

  return bsdatasettus_add_inc ((BsDataSetTus*) pDicOfsts, (void*) obj, BS_IDX_2);
}

/**
 * <p>Only constructor.</p>
 * @param pDiIx opened dic with index NOT NULL
 * @param pOfst offset of word's content
 * @param pLen length of word's content
 * @return object or NULL when error
 * @set errno - ENOMEM 
 **/
BsDiSrDt2*
  bsdisrdt2_new (BsDiIxBs *pDiIx, unsigned int pOfst, unsigned int pLen)
{
  BsDiSrDt2 *obj = malloc (sizeof (BsDiSrDt2));
  if (obj != NULL)
  {
    obj->diIx = pDiIx;
    obj->ofst = pOfst;
    obj->len = pLen;
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor.</p>
 * @param pDiOfLn maybe NULL
 * @return always NULL
 **/
BsDiSrDt2*
  bsdisrdt2_free (BsDiSrDt2 *pDiOfLn)
{
  if ( pDiOfLn != NULL )
  {
    free (pDiOfLn);
  }
  return NULL;
}

/**
 * <p>Only constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiSrDt2s*
  bsdisrdt2s_new (BS_IDX_T pBufSz)
{
  return (BsDiSrDt2s*) bsdatasettus_new (sizeof (BsDiSrDt2s), pBufSz);
}

/**
 * <p>Destructor.</p>
 * @param pDiOfLns maybe NULL
 * @return always NULL
 **/
BsDiSrDt2s*
  bsdisrdt2s_free (BsDiSrDt2s *pDiOfLns)
{
  bsdatasettus_free ((BsDataSetTus*) pDiOfLns, (Bs_Destruct*) bsdisrdt2_free);
  return NULL;
}


/**
 * <p>Add object to the first null cell of collection.
 * It increases collection if it's full-filled.</p>
 * @param pDiOfLnss - collection
 * @param pDiIx opened dic with index NOT NULL
 * @param pOfst offset of word's content
 * @param pLen length of word's content
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE or ENOMEM
 **/
BS_IDX_T
  bsdisrdt2s_add_inc (BsDiSrDt2s *pDiOfLns, BsDiIxBs *pDiIx,
                      unsigned int pOfst, unsigned int pLen)
{
  BsDiSrDt2 *obj = bsdisrdt2_new (pDiIx, pOfst, pLen);
  if ( obj == NULL )
              { return BS_IDX_NULL; }

  return bsdatasettus_add_inc ((BsDataSetTus*) pDiOfLns, (void*) obj, BS_IDX_2);
}

/**
 * <p>Constructor.</p>
 * @param pCstr origin word in dictionary file to own without cloning
 * @return object or NULL when error
 * @set errno - ENOMEM 
 **/
BsDiFdWd*
  bsdifdwdtst_new (char *pCstr)
{
  BsDiFdWd *obj = malloc (sizeof (BsDiFdWd));
  if (obj != NULL) {
    obj->dicOfsts = NULL;
    obj->dicOfLns = NULL;
    obj->wrd = bsstring_new (pCstr);
    if ( obj->wrd == NULL ) {
      obj = bsdifdwd_free (obj);
    } else {
      obj->dicOfLns = bsdisrdt2s_new (BS_IDX_2);
      if ( obj->dicOfLns == NULL )
      {
        obj = bsdifdwd_free (obj);
      } else {
        obj->dicOfsts = bsdisrdt1s_new (BS_IDX_2);
        if ( obj->dicOfsts == NULL )
        {
          obj = bsdifdwd_free (obj);
        }
      }
    }
  }
  if (obj == NULL) {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor.</p>
 * @param pDicsWrd maybe NULL
 * @return always NULL
 **/
BsDiFdWd*
  bsdifdwd_free (BsDiFdWd *pDicsWrd)
{
  if ( pDicsWrd != NULL )
  {
    if ( pDicsWrd->wrd != NULL )
    {
      bsstring_free (pDicsWrd->wrd);
    }
    if ( pDicsWrd->dicOfsts != NULL )
    {
      bsdisrdt1s_free (pDicsWrd->dicOfsts);
    }
    if ( pDicsWrd->dicOfLns != NULL )
    {
      bsdisrdt2s_free (pDicsWrd->dicOfLns);
    }
    free (pDicsWrd);
  }
  return NULL;
}

/**
 * <p>Only constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiFdWds*
  bsdifdwds_new (BS_IDX_T pBufSz)
{
  return (BsDiFdWds*) bsdatasettus_new(sizeof (BsDiFdWds), pBufSz);
}

/**
 * <p>Destructor.</p>
 * @param pFdWrds maybe NULL
 * @return always NULL
 **/
BsDiFdWds*
  bsdifdwds_free (BsDiFdWds *pFdWrds)
{
  bsdatasettus_free ((BsDataSetTus*) pFdWrds,
                     (Bs_Destruct*) bsdifdwd_free);
  return NULL;
}

/**
 * <p>Clear all elements.</p>
 * @param pFdWrds NOT NULL
 **/
void
  bsdifdwds_clear (BsDiFdWds *pFdWrds)
{
  bsdatasettus_clear ((BsDataSetTus*) pFdWrds,
                     (Bs_Destruct*) bsdifdwd_free);
}


/**
 * <p>Find member by word.</p>
 * @param pFdWrds - collection
 * @return matched member when found, NULL if word NULL or not found
 **/
BsDiFdWd*
  bsdifdwds_find (BsDiFdWds *pFdWrds, char *pCstr)

{
  if ( pCstr == NULL )
  {
    return NULL;
  }
  for ( BS_IDX_T l = BS_IDX_0; l < pFdWrds->size; l++ )
  {
    if ( strcmp (pFdWrds->vals[l]->wrd->val, pCstr) == 0 )
    {
      return pFdWrds->vals[l];
    }
  }
  return NULL;
}

/**
 * <p>Add dics-word with data to search type#1 into collection,
 * it will add new d.word with data to search type#1 into
 * existed member, or create new one otherwise.
 * It increases collection if it's full-filled.</p>
 * @param pFdWrds - collection
 * @param pCstr origin word in dictionary file to own without cloning
 * @param pDiIx opened dic with index NOT NULL
 * @param pOfst offset of d.word content
 * @return index of added/modified member when OK
 * @set errno - BSE_WRONG_PARAM, BSE_ARR_OUT_MAX_SIZE or ENOMEM
 **/
BS_IDX_T
  bsdifdwds_add_inc1 (BsDiFdWds *pFdWrds, char *pCstr,
                      BsDiIxBs *pDiIx, BS_FOFST_T pOfst)
{
  if ( pCstr == NULL || pDiIx == NULL || pOfst == BS_IDX_NULL )
  {
    errno = BSE_WRONG_PARAMS;
    BSLOG_ERR
    return BS_IDX_NULL;
  }
  for ( BS_IDX_T l = BS_IDX_0; l < pFdWrds->size; l++ )
  {
    
    if ( strcmp (pFdWrds->vals[l]->wrd->val, pCstr) == 0 )
    {
      if ( bsdisrdt1s_add_inc (pFdWrds->vals[l]->dicOfsts, pDiIx, pOfst) == BS_IDX_NULL )
                      { return BS_IDX_NULL; }
      return l;
    }
  }

  BsDiFdWd *obj = bsdifdwdtst_new (pCstr);
  if ( obj == NULL )
                      { return BS_IDX_NULL; }

  if ( bsdisrdt1s_add_inc (obj->dicOfsts, pDiIx, pOfst) == BS_IDX_NULL )
                      { return BS_IDX_NULL; }
  
  return bsdatasettus_add_inc ((BsDataSetTus*) pFdWrds,
                               (void*) obj, BS_IDX_10);
}


/**
 * <p>Add dics-word with data to search type#2 into collection,
 * it will add new d.word with data to search type#1 into
 * existed member, or create new one otherwise.
 * It increases collection if it's full-filled.</p>
 * @param pFdWrds - collection
 * @param pCstr origin word in dictionary file to own without cloning
 * @param pDiIx opened dic with index NOT NULL
 * @param pOfst offset of word's content
 * @param pLen length of word's content
 * @return index of added/modified member when OK
 * @set errno - BSE_WRONG_PARAM, BSE_ARR_OUT_MAX_SIZE or ENOMEM
 **/
BS_IDX_T
  bsdifdwds_add_inc2 (BsDiFdWds *pFdWrds, char *pCstr, BsDiIxBs *pDiIx,
                      unsigned int pOfst, unsigned int pLen)
{
  if ( pCstr == NULL || pDiIx == NULL || pOfst == UINT_MAX || pLen == UINT_MAX )
  {
    errno = BSE_WRONG_PARAMS;
    BSLOG_ERR
    return BS_IDX_NULL;
  }
  for ( BS_IDX_T l = BS_IDX_0; l < pFdWrds->size; l++ )
  {
    
    if ( strcmp (pFdWrds->vals[l]->wrd->val, pCstr) == 0 )
    {
      if ( bsdisrdt2s_add_inc (pFdWrds->vals[l]->dicOfLns, pDiIx, pOfst, pLen) == BS_IDX_NULL )
                      { return BS_IDX_NULL; }
      return l;
    }
  }

  BsDiFdWd *obj = bsdifdwdtst_new (pCstr);
  if ( obj == NULL )
                      { return BS_IDX_NULL; }

  if ( bsdisrdt2s_add_inc (obj->dicOfLns, pDiIx, pOfst, pLen) == BS_IDX_NULL )
                      { return BS_IDX_NULL; }
  
  return bsdatasettus_add_inc ((BsDataSetTus*) pFdWrds,
                               (void*) obj, BS_IDX_10);
}

/**
 * <p>Constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxOst*
  bsdiixost_new ()
{
  BsDiIxOst *obj = malloc (sizeof (BsDiIxOst));
  if ( obj != NULL ) {
    obj->stt = EBSDS_OPENING;
    obj->prgr = 100;
    obj->stp = false;
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor.</p>
 * @param pOpSt object or NULL
 * @return always NULL
 **/
BsDiIxOst*
  bsdiixost_free (BsDiIxOst *pOpSt)
{
  if ( pOpSt != NULL )
  {
    free (pOpSt);
  }
  return NULL;
}

//2. Making IDX utils:
/**
 * <p>Add HIRT record.
 * It will allocate HIRT if it's NULL!</p>
 * @param pHead - pointer to IFH
 * @param pRcd HIRT record
 * @set errno if error.
 **/
void
  bsdiixheadbs_add_hirtrd (BsDiIxHeadBs *pHead, BsDiIxHirtRd *pRcd)
{
  if ( pHead->hirt == NULL )
  {
    pHead->hirt = malloc (pHead->hirtSz * sizeof (BsDiIxHirtRd*));
    if ( pHead->hirt == NULL )
    {
      if ( errno == 0 ) { errno = ENOMEM; }
      BSLOG_ERR
      return;
    } else {
      for ( int i = 0; i < pHead->hirtSz; i++ )
                      { pHead->hirt[i] = NULL; }
    }
  }
  for ( int i = 0; i < pHead->hirtSz; i++ )
  {
    if ( pHead->hirt[i] == NULL )
    {
      pHead->hirt[i] = pRcd;
      return;
    }
  }
  errno = BSE_OUT_BUFFER_SIZE;
  BSLOG_LOG(BSLERROR, "HIRT full!\n")
}

/**
 * <p>Make new schar in HIRT second chars</p>
 * @param pRcdmk HIRT second chars
 * @param p_schar second char
 * @param p_irt_idx word with second char IRT index
 * @set errno if error.
 **/
void
  bsdiixhirtrdmk_make (BsDiIxHirtRdMk *pRcdmk, BS_CHAR_T p_schar, BS_IDX_T p_irt_idx)
{
  int idx_null = -1, idx_in = -1;
  for (int i = 0; i < pRcdmk->schrsSz; i++) {
    if (pRcdmk->schars[i] == 0) {
      idx_null = i;
      break;
    } else if (pRcdmk->schars[i] == p_schar) {
      idx_in = i;
      break;
    }
  }
  if (idx_in == -1 && idx_null == -1) {
    errno = BSE_OUT_BUFFER_SIZE;
    BSLOG_ERR
    return;
  }
  if (idx_in == -1) {
    pRcdmk->schars[idx_null] = p_schar;
    pRcdmk->irtIdxs[idx_null] = p_irt_idx;
  }
}

/**
 * <p>Initialize/clear data in HIRT second chars because of new first letter word encountered.</p>
 * @param pRcdmk HIRT second chars
 **/
void
  bsdiixhirtrdmk_init(BsDiIxHirtRdMk *pRcdmk)
{
  pRcdmk->fchar = 0;
  pRcdmk->irtIdx = BS_IDX_NULL;
  for ( int i = 0; i < pRcdmk->schrsSz; i++ )
  {
    pRcdmk->schars[i] = 0;
    pRcdmk->irtIdxs[i] = BS_IDX_NULL;
  }
}

/**
 * <p>Validate HIRT.</p>
 * @param pHirt HIRT
 * @param pHSize HIRT size
 * @param pAb AB
 * @set errno if error.
 **/
void
  bsdicidxhirt_validate (BsDiIxHirtRd **pHirt, int pSize, BsDicIdxAb *pAb)
{
  for ( int i = 0; i < pSize; i++ )
  {
    if ( pHirt[i] == NULL )
    {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG (BSLERROR, "HIRT[%d] == NULL\n", i)
      return;
    }
    if (pHirt[i]->fchar == 0) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "HIRT[%d]-fchar == 0\n", i)
      return;
    }
    if (pHirt[i]->fchar > pAb->chrsTot + 1) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "HIRT[%d]-fchar > AB.chrsTot + 1: %hu > %hu\n", i, pHirt[i]->fchar, pAb->chrsTot + 1)
      return;
    }
    if (pHirt[i]->irtIdx == BS_IDX_NULL) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "HIRT[%d]-irtIdx == NULL\n", i)
      return;
    }
    if (pHirt[i]->schrsSz < 0) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "HIRT[%d]-schar_size < 0 = %d\n", i, pHirt[i]->schrsSz)
      return;
    }
    if (pHirt[i]->schrsSz > 0) {
      for (int j = 0; j < pHirt[i]->schrsSz; j++) {
        if (pHirt[i]->schars[j] == 0) {
          errno = BSE_VALIDATE_ERR;
          BSLOG_LOG(BSLERROR, "HIRT[%d]-schars[%d] == 0\n", i, j)
          return;
        }
        if (pHirt[i]->schars[j] > pAb->chrsTot + 1) {
          errno = BSE_VALIDATE_ERR;
          BSLOG_LOG(BSLERROR, "HIRT[%d]-schars[%d] > AB.chrsTot + 1: %hu > %hu\n", i, j, pHirt[i]->schars[j], pAb->chrsTot + 1)
          return;
        }
        if (pHirt[i]->irtIdxs[j] == BS_IDX_NULL) {
          errno = BSE_VALIDATE_ERR;
          BSLOG_LOG(BSLERROR, "HIRT[%d]-irtIdxs[%d] == NULL\n", i, j)
          return;
        }
      }
    }
  }
}

/**
 * <p>Save (write/overwrite) IDX RAM's head base (in memory) into file.</p>
 * @param pDiIxRm IDX RAM.
 * @param pPth - dictionary path.
 * @return IDX file for further writing
 * @set errno if error.
 **/
FILE*
  bsdiixheadbs_save (BsDiIxHeadBs *pHead, char *pPth)
{
  //vars:
  int i;
  FILE *idxFl;
  char idxPth[strlen(pPth) + 10];
  //code:
  strcpy (idxPth, pPth);
  strcat (idxPth, BDI_IDX_FILE_EXT);
  idxFl = fopen(idxPth, "wb");
  BS_IF_EN_RETN (idxFl == NULL, BSE_OPEN_FILE)
  //HEAD name, format, AB, totals:
  BS_DO_E_OUTE (bsfwrite_bsstr_lenuchar (pHead->nme, idxFl))
  BS_DO_E_OUTE (bsfwrite_enum (pHead->frmt, idxFl))
  BS_DO_E_OUTE (bsfwrite_int (&pHead->ab->chrsTot, idxFl))
  BS_DO_E_OUTE (bsfwrite_bswchars (pHead->ab->wchars, pHead->ab->chrsTot, idxFl))
  BS_DO_E_OUTE (bsfwrite_int (&pHead->ab->ispace, idxFl))
  BS_DO_E_OUTE (bsfwrite_bsindex (&pHead->irtSz, idxFl))
  BS_DO_E_OUTE (bsfwrite_int (&pHead->mxIrWdSz, idxFl))
  BS_DO_E_OUTE (bsfwrite_bool (&pHead->isIxRm, idxFl))
  BS_DO_E_OUTE (bsfwrite_int (&pHead->hirtSz, idxFl))
    //HIRT:
  if ( pHead->hirt != NULL )
  {
    for ( i = 0; i < pHead->hirtSz; i++ )
    {
      BS_DO_E_OUTE (bsfwrite_bschar (&pHead->hirt[i]->fchar, idxFl))
      BS_DO_E_OUTE (bsfwrite_bsindex (&pHead->hirt[i]->irtIdx, idxFl))
      BS_DO_E_OUTE (bsfwrite_bssmall (&pHead->hirt[i]->schrsSz, idxFl))
      if ( pHead->hirt[i]->schrsSz > 0 )
      {
        BS_DO_E_OUTE (bsfwrite_bschars (pHead->hirt[i]->schars,
                                        pHead->hirt[i]->schrsSz, idxFl))
        BS_DO_E_OUTE (bsfwrite_bsindexs (pHead->hirt[i]->irtIdxs,
                                        pHead->hirt[i]->schrsSz, idxFl))
      }
    }
  }
  return idxFl;

oute:
  fclose(idxFl);
  return NULL;
}

/**
 * <p>Load IDX head base from IDX file.</p>
 * @param pPth - dictionary path.
 * @param pHead - new head to fill
 * @param pDicFlRt - pointer to return opened dictionary
 * @param pIdxFlRt - pointer to return opened IDX file
 * @set errno if error. "IDX file not found" is not error!
 **/
void
  bsdiixheadbs_load (char *pPth, BsDiIxHeadBs* pHead,
                    FILE ** pDicFlRt, FILE **pIdxFlRt)
{
  //vars:
  int i, chrsTot;
  FILE *dicFl, *idxFl;
  BsDiIxHirtRd *hirtRd;
    //AD-buffers:
  char idxPth[strlen(pPth) + 10];
  //code:
    //var init0:
  dicFl = NULL; idxFl = NULL; hirtRd = NULL;
    //start:
  dicFl = fopen (pPth, "r");
  BS_IF_EN_RET (dicFl == NULL, BSE_OPEN_FILE)
  strcpy(idxPth, pPth);
  strcat(idxPth, BDI_IDX_FILE_EXT);
  idxFl = fopen (idxPth, "rb");
  if ( idxFl == NULL )
  {
    if ( errno != 0 ) { errno = 0; }
    BSLOG_LOG (BSLWARN, "IDX file not found: %s\n", idxPth)
    return;
  }
  //HEAD name, format, AB, totals:
  BS_DO_E_OUTE (pHead->nme = bsfread_bsstr_lenuchar (idxFl))
  BS_DO_E_OUTE (pHead->frmt = bsfread_enum (idxFl))
  BS_DO_E_OUTE (bsfread_int (&chrsTot, idxFl))
  BS_DO_E_OUTE (pHead->ab = bsdicidxab_new (chrsTot))
  pHead->ab->chrsTot = chrsTot;
  BS_DO_E_OUTE (bsfread_bswchars (pHead->ab->wchars,
                                  pHead->ab->chrsTot, idxFl))
  BS_DO_E_OUTE (bsfread_int (&pHead->ab->ispace, idxFl))

  BS_DO_E_OUTE (bsfread_bsindex (&pHead->irtSz, idxFl))
  BS_DO_E_OUTE (bsfread_int (&pHead->mxIrWdSz, idxFl))
  BS_DO_E_OUTE (bsfread_bool (&pHead->isIxRm, idxFl))
  BS_DO_E_OUTE (bsfread_int (&pHead->hirtSz, idxFl))
  //HIRT:
  if ( pHead->hirtSz > 0 )
  {
    BS_CHAR_T fchar; BS_SMALL_T schrsSz; BS_IDX_T irtIdx;
    for ( i = 0; i < pHead->hirtSz; i++ )
    {
      BS_DO_E_OUTE (bsfread_bschar (&fchar, idxFl))
      BS_DO_E_OUTE (bsfread_bsindex (&irtIdx, idxFl))
      BS_DO_E_OUTE (bsfread_bssmall (&schrsSz, idxFl))
      BS_DO_E_OUTE (hirtRd = bsdicidxhirtrd_new (schrsSz))
      BS_DO_E_OUTE (bsdiixheadbs_add_hirtrd (pHead, hirtRd))
      hirtRd = NULL;
      pHead->hirt[i]->fchar = fchar;
      pHead->hirt[i]->irtIdx = irtIdx;
      if ( pHead->hirt[i]->schrsSz > 0 )
      {
        BS_DO_E_OUTE(bsfread_bschars (pHead->hirt[i]->schars,
                                      pHead->hirt[i]->schrsSz, idxFl))
        BS_DO_E_OUTE(bsfread_bsindexes (pHead->hirt[i]->irtIdxs,
                                        pHead->hirt[i]->schrsSz, idxFl))
      }
    }
  }
  *pDicFlRt = dicFl;
  *pIdxFlRt = idxFl;
  return;

oute:
  bsdicidxhirtrd_free (hirtRd);
  if ( dicFl != NULL )
              { fclose(dicFl); }
  if ( idxFl != NULL )
              { fclose(idxFl); }
}

/**
 * <p>Reveal initial IRT range to seek for given word(sub) to match in AB coding.</p>
 * @param pHirt - HIRT
 * @param pHirtSz - HIRT size
 * @param pIwrd - word(sub) to match in AB coding
 * @param pIrtStartRt - pointer to return index start
 * @param pIrtEndRt - pointer to return index end
 * @set errno if error.
 **/
void
  bsdicidx_find_irtrange (BsDiIxHirtRd **pHirt, int pHirtSz, BS_CHAR_T *pIwrd,
                          BS_IDX_T *pIrtStartRt, BS_IDX_T *pIrtEndRt)
{
  //1.find irtStRn of first up to two chars and irtEnRn:
  BsDiIxHirtRd *hirtRd = NULL;
  int i, hirtrdIdx;
  for ( i = 0; i < pHirtSz; i++ )
  {
    if ( pHirt[i]->fchar == pIwrd[0] ) {
      hirtRd = pHirt[i];
      hirtrdIdx = i;
      break;
    }
  }
  if ( hirtRd == NULL )
  { //first char never happen in this dictionary
    return;
  }
  //find second char:
  BS_IDX_T irtStart = BS_IDX_NULL;
  BS_IDX_T irtEnd = BS_IDX_NULL;
  if ( bsdicidx_istr_len(pIwrd) == 1 || hirtRd->schrsSz == 0 )
  {
    irtStart = hirtRd->irtIdx;
    //irtEnd = last irtIdx for fchar
  } else {
    for ( i = 0; i < hirtRd->schrsSz; i++ )
    {
      irtStart = hirtRd->irtIdxs[i];
      if ( hirtRd->schars[i] == pIwrd[1] )
      {
        if ( i + 1 < hirtRd->schrsSz )
        {
          irtEnd = hirtRd->irtIdxs[i + 1];
        } // else irtEnd = last irtIdx for fchar
        break;
      } else if ( hirtRd->schars[i] > pIwrd[1] ) {
        irtEnd = hirtRd->irtIdxs[i];
        break;
      }
    }
  }
  if ( irtEnd == BS_IDX_NULL )
  {
    //irtEnd = last irtIdx for fchar:
    if ( hirtrdIdx != pHirtSz - 1 )
    {
      irtEnd = pHirt[hirtrdIdx + 1]->irtIdx - 1;
    } //else the last, that already set
      // irtEnd = pHead->irtSz - 1;
  }
  *pIrtStartRt = irtStart;
  if ( irtEnd != BS_IDX_NULL )
          { *pIrtEndRt = irtEnd; }
}

//Find/history utils;

#define BS_HIWRD "#WRD="

#define BS_HISRDT1CNT "#DT1CNT="

#define BS_HISRDT1DIC "#DT1DIC="

#define BS_HISRDT1OFST "#DT1OFST="

/**
 * <p>Save found words with data to search into given file.</p>
 * @param pFdWrds not NULL
 * @param pFile not NULL
 * @set errno if error
 **/
void
  bsdifdwds_save (BsDiFdWds *pFdWrds, FILE *pFile)
{
  for ( BS_IDX_T l = BS_IDX_0; l < pFdWrds->size; l++ )
  {
    fprintf (pFile, BS_HIWRD"%s\n", pFdWrds->vals[l]->wrd->val);
    fprintf (pFile, BS_HISRDT1CNT""BS_IDX_FMT"\n", pFdWrds->vals[l]->dicOfsts->size);
    for ( BS_IDX_T j = BS_IDX_0; j < pFdWrds->vals[j]->dicOfsts->size; j++ )
    {
      fprintf (pFile, BS_HISRDT1DIC"%s\n", pFdWrds->vals[l]->dicOfsts->vals[j]->diIx->head->nme->val);
      fprintf (pFile, BS_HISRDT1OFST""BS_FOFST_FMT"\n", pFdWrds->vals[l]->dicOfsts->vals[j]->ofst);
    }
  }
}
