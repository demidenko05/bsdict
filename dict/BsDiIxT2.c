/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ dictionary content search type#2 basic,
 * dictionary file strusture independent library.</p>
 * @author Yury Demidenko
 **/

#include "wchar.h"
#include "wctype.h"
#include "stdlib.h"
#include "string.h"

#include "BsError.h"
#include "BsFioWrap.h"
#include "BsDiIxT2.h"

//Counsructors/destructors/collection utils:
/**
 * <p>Constructor.</p>
 * @param pWrdc word to clone
 * @param pDiIx - DIC with IDX
 * @param pOfst - content rec. offset
 * @param pLen - content rec. length
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiDtT2*
  bsdidtt2_new (BsString *pWrdc, BsDiIxT2Bs *pDiIx, unsigned int pOfst,
                      unsigned int pLen)
{
  BsDiDtT2 *obj = malloc (sizeof (BsDiDtT2));
  if ( obj != NULL )
  {
    obj->wrd = bsstring_clone (pWrdc); //TODO reflib

    if ( obj->wrd == NULL )
    {
      obj = bsdidtt2_free (obj);
    } else {
      obj->ofst = pOfst;
      obj->len = pLen;
      obj->diIx = pDiIx;
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
 * <p>Destructor.</p>
 * @param pRcdDt record data, maybe NULL
 * @return always NULL
 **/
BsDiDtT2*
  bsdidtt2_free (BsDiDtT2 *pRcdDt)
{
  if ( pRcdDt != NULL )
  {
    if ( pRcdDt->wrd != NULL )
    {
      bsstring_free (pRcdDt->wrd);
    }
    free (pRcdDt);
  }
  return NULL;
}

/**
 * <p>Constructor.</p>
 * @param pIwrdc i.word to  copy
 * @param pOfst - content rec. offset
 * @param pLen - content rec. length
 * @param pIdxTrnz - index
 * @param pMxIrWdSz - max IRT word size 
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxT2IdxIrd*
  bsdiixt2idxird_new (BS_CHAR_T *pIwrdc, unsigned int pOfst,
                      unsigned int pLen, BS_IDX_T pIdxTrnz, int pMxIrWdSz)
{
  BsDiIxT2IdxIrd *obj = malloc (sizeof (BsDiIxT2IdxIrd));
  if ( obj != NULL )
  {
    obj->iwrd = malloc (pMxIrWdSz * sizeof (BS_CHAR_T));

    if ( obj->iwrd == NULL )
    {
      obj = bsdiixt2idxird_free (obj);
    } else {
      bsdicidx_istr_cpy (obj->iwrd, pIwrdc);
      obj->ofst = pOfst;
      obj->len = pLen;
      obj->idxTrnz = pIdxTrnz;
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
 * <p>Destructor.</p>
 * @param pRcd IWORD record
 * @return always NULL
 **/
BsDiIxT2IdxIrd*
  bsdiixt2idxird_free (BsDiIxT2IdxIrd *pRcd)
{
  if ( pRcd != NULL )
  {
    if ( pRcd->iwrd != NULL )
    {
      free (pRcd->iwrd);
    }
    free (pRcd);
  }
  return NULL;
}

/**
 * <p>Constructor.</p>
 * @param pIwrdc i.word to  copy
 * @param pOfst - content rec. offset
 * @param pLen - content rec. length
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxT2Ird*
  bsdiixt2ird_new (BS_CHAR_T *pIwrdc, unsigned int pOfst, unsigned int pLen)
{
  BsDiIxT2Ird *obj = malloc (sizeof (BsDiIxT2Ird));
  if ( obj != NULL )
  {
    int iwsz = bsdicidx_istr_len (pIwrdc) + 1;
    obj->iwrd = malloc (iwsz * sizeof (BS_CHAR_T));

    if ( obj->iwrd == NULL )
    {
      obj = bsdiixt2ird_free (obj);
    } else {
      bsdicidx_istr_cpy (obj->iwrd, pIwrdc);
      obj->ofst = pOfst;
      obj->len = pLen;
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
 * <p>Destructor.</p>
 * @param pRcd IWORD record
 * @return always NULL
 **/
BsDiIxT2Ird*
  bsdiixt2ird_free (BsDiIxT2Ird *pRcd)
{
  if ( pRcd != NULL )
  {
    if ( pRcd->iwrd != NULL )
    {
      free (pRcd->iwrd);
    }
    free (pRcd);
  }
  return NULL;
}

/**
 * <p>Comparator.</p>
 * @param pRcd1 IWORD record1
 * @param pRcd2 IWORD record2
 * @return pRcd1 -1 less 0 equal 1 greater than pRcd2
 **/
int
  bsdiixt2ird_compare (BsDiIxT2Ird *pRcd1, BsDiIxT2Ird *pRcd2)
{
  return bsdicidx_istr_cmp (pRcd1->iwrd, pRcd2->iwrd);
}

/**
 * <p>Constructor.</p>
 * @param pBufSz total records
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxT2Irt*
  bsdiixt2irt_new (BS_IDX_T pBufSz)
{
  BS_DO_E_RETN (BsDiIxT2Irt *obj = (BsDiIxT2Irt *)
                bsdatasettus_new (sizeof (BsDiIxT2Irt), pBufSz))
  return obj;
}

/**
 * <p>Destructor WITHOUT transient AB.</p>
 * @param p_srr allocated array of records i-words or NULL
 * @return always NULL
 **/
BsDiIxT2Irt*
  bsdiixt2irt_free (BsDiIxT2Irt *pSet)
{
  if ( pSet != NULL )
  {
    bsdatasettus_free ((BsDataSetTus*) pSet, (Bs_Destruct*) &bsdiixt2ird_free);
  }
  return NULL;
}


/**
 * <p>Constructor.</p>
 * @param pBufSz total records
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiDtT2s*
  bsdidtt2s_new (BS_IDX_T pBufSz)
{
  BS_DO_E_RETN (BsDiDtT2s *obj = (BsDiDtT2s *)
                bsdatasettus_new (sizeof (BsDiDtT2s), pBufSz))
  return obj;
}

/**
 * <p>Destructor.</p>
 * @param pSet dataset or NULL
 * @return always NULL
 **/
BsDiDtT2s*
  bsdidtt2s_free (BsDiDtT2s *pSet)
{
  if ( pSet != NULL )
  {
    bsdatasettus_free ((BsDataSetTus*) pSet, (Bs_Destruct*) &bsdidtt2_free);
  }
  return NULL;
}

/**
 * <p>Clear all elements.</p>
 * @param pFdWrds NOT NULL
 **/
void
  bsdidtt2s_clear (BsDiDtT2s *pSet)
{
  bsdatasettus_clear ((BsDataSetTus*) pSet,
                     (Bs_Destruct*) bsdidtt2_free);
}

/**
 * <p>Add object to the first null cell of array.
 * It increases array if it's full-filled.</p>
 * @param pSet - data set
 * @param pObj - object
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsdidtt2s_add_inc(BsDiDtT2s *pSet, BsDiDtT2 *pObj, BS_IDX_T pInc) {
  return bsdatasettus_add_inc((BsDataSetTus*) pSet, (void*) pObj, pInc);
}

/**
 * <p>Add i.word into array with sorting without duplicates.
 * It increases array if it's full-filled.</p>
 * @param pSet - data set
 * @param pObj - object NON-NULL
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T
  bsdiixt2irt_add_sort_inc (BsDiIxT2Irt *pSet, BsDiIxT2Ird *pObj, BS_IDX_T pInc)
{
  return bsdatasettus_add_sort_inc((BsDataSetTus*) pSet, (void*)pObj,
                          pInc, (Bs_Compare*) &bsdiixt2ird_compare, false);
}

/**
 * <p>New head with new AB to fill from dictionary and sorted words.
 * HIRT will be created further.</p>
 * @param pTot - total records from LSA file
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHeadT2*
  bsdiixheadt2_new_tf (BS_IDX_T pTot)
{
  BsDiIxHeadT2 *obj = (BsDiIxHeadT2*) bsdiixheadbs_new (sizeof (BsDiIxHeadT2));
  if ( obj != NULL )
  {
    obj->irtSz = pTot;
    obj->cntOfst = BS_IDX_NULL;
    obj->ab = bsdicidxab_new (BDI_AB_BUF_INITSIZE);
    if ( obj->ab == NULL )
        { obj = bsdiixheadt2_free (obj); }
  }
  if ( obj == NULL ) {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}


/**
 * <p>New head to load from IDX file.
 * AB will be created according IDX file's "chars total" property.
 * HIRT will be created further.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHeadT2*
  bsdiixheadt2_new_tl ()
{
  BsDiIxHeadT2 *obj = (BsDiIxHeadT2*) bsdiixheadbs_new (sizeof (BsDiIxHeadT2));
  if ( obj != NULL )
  {
    obj->cntOfst = BS_IDX_NULL;
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor.</p>
 * @param pHead object or NULL
 * @return always NULL
 **/
BsDiIxHeadT2*
  bsdiixheadt2_free (BsDiIxHeadT2* pHead)
{
  if ( pHead != NULL )
            { bsdiixheadbs_free ((BsDiIxHeadBs*) pHead); }
  return NULL;
}

/**
 * <p>Constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxT2Rm*
  bsdiixt2rm_new (BsDiIxHeadT2 *pHead, FILE* pDicFl)
{
  BsDiIxT2Rm *obj = malloc (sizeof (BsDiIxT2Rm));
  if ( obj != NULL )
  {
    obj->dicFl = pDicFl;
    obj->head = pHead;
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor with closing files.</p>
 * @param pDiIxRm - dictionary with index or NULL
 * @return always NULL
 **/
BsDiIxT2Rm*
  bsdiixt2rm_destroy (BsDiIxT2Rm *pDiIxRm)
{
  if ( pDiIxRm != NULL )
  {
    if ( pDiIxRm->dicFl != NULL )
                { fclose (pDiIxRm->dicFl); }
    if ( pDiIxRm->head != NULL )
                { bsdiixheadt2_free (pDiIxRm->head); }
    free (pDiIxRm);
  }
  return NULL;
}

/**
 * <p>Constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxT2*
  bsdiixt2_new (BsDiIxHeadT2 *pHead, FILE* pDicFl, FILE* pIdxFl)
{
  BsDiIxT2 *obj = malloc (sizeof (BsDiIxT2));
  if ( obj != NULL )
  {
    obj->dicFl = pDicFl;
    obj->idxFl = pIdxFl;
    obj->irtOfst = BS_IDX_NULL;
    obj->head = pHead;
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor with closing files.</p>
 * @param pDiIx - dictionary with index or NULL
 * @return always NULL
 **/
BsDiIxT2*
  bsdiixt2_destroy (BsDiIxT2 *pDiIx)
{
  if ( pDiIx != NULL )
  {
    if ( pDiIx->dicFl != NULL )
                { fclose (pDiIx->dicFl); }
    if ( pDiIx->idxFl != NULL )
                { fclose (pDiIx->idxFl); }
    if ( pDiIx->head != NULL )
                { bsdiixheadt2_free (pDiIx->head); }
    free (pDiIx);
  }
  return NULL;
}

/**
 * <p>Add HIRT record.
 * It will allocate HIRT if it's NULL!</p>
 * @param pHead - pointer to IFH
 * @param pRcd HIRT record
 * @set errno if error.
 **/
void
  bsdiixheadt2_add_hirtrd (BsDiIxHeadT2 *pHead, BsDiIxHirtRd *pRcd)
{
  bsdiixheadbs_add_hirtrd ((BsDiIxHeadBs*) pHead, pRcd);
}

//Making DIC with IDX main lib:

/**
 * <p>Consumer of just read d.word sound offset and length.
 * This is 1-st phase consumer to make AB.</p>
 * @param pDiIxRm opened rewind dictionary bundle
 * @param pDwrd - d.word
 * @param pOfst - content rec. offset
 * @param pLen - content rec. length
 * @param pFstChrs - first chars instrument
 * @return 0 to continue iteration, otherwise stop iteration
 * @set errno if error.
 **/
int
  bsdiixt2_csm_ab (BsDiIxT2Rm* pDiIxRm, BS_WCHAR_T *pDwrd,
            unsigned int pOfst, unsigned int pLen, BS_WCHAR_T *pFstChrs)
{
  BS_DO_E_RETE (bsdicidxab_add_wstr (pDwrd, pDiIxRm->head->ab))
  for ( int i = 0; i < BSDIAU_FIRSTCHARARRSZ; i++ )
  { 
    if ( pFstChrs[i] == pDwrd[0] )
                      { break; }
    if ( pFstChrs[i] == 0 )
    { 
      pFstChrs[i] = pDwrd[0];
      break;
    }
  }    
  int sz = wcslen (pDwrd) + 1;
  if ( sz > pDiIxRm->head->mxIrWdSz )
                  { pDiIxRm->head->mxIrWdSz = sz; }
  return 0;
}

/**
 * <p>Consumer of just read d.word sound offset and length.
 * This is 2-nd phase consumer to make IRT.</p>
 * @param pDiIxRm opened rewind dictionary bundle
 * @param pDwrd - d.word
 * @param pOfst - content rec. offset
 * @param pLen - content rec. length
 * @param BsDiIxT2Irt - sorted i.words with sound data
 * @return 0 to continue iteration, otherwise stop iteration
 * @set errno if error.
 **/
int
  bsdiixt2_csm_irt (BsDiIxT2Rm* pDiIxRm, BS_WCHAR_T *pDwrd, unsigned int pOfst,
                    unsigned int pLen, BsDiIxT2Irt *pIrt)
{
  BS_CHAR_T iwrd[pDiIxRm->head->mxIrWdSz];
  bsdicidxab_wstr_to_istr (pDwrd, iwrd, pDiIxRm->head->ab);
  BS_DO_E_RETE (BsDiIxT2Ird *rcd = bsdiixt2ird_new (iwrd, pOfst, pLen))
  BS_DO_E_OUTE (bsdiixt2irt_add_sort_inc (pIrt, rcd, BS_IDX_100)) //records number is actually fixed - i.e. LSA total
  return 0;
oute:
  bsdiixt2ird_free (rcd);
  return errno;
}

/**
 * <p>Load IDX head audio from IDX file.</p>
 * @param pPth - dictionary path.
 * @param pHeadRt - pointer to return filled head
 * @param pDicFlRt - pointer to return opened dictionary
 * @param pIdxFlRt - pointer to return opened IDX file
 * @set errno if error. "IDX file not found" is not error!
 **/
void
  bsdiixheadt2_load (char *pPth, BsDiIxHeadT2 **pHeadRt,
                     FILE **pDicFlRt, FILE **pIdxFlRt)
{
  //vars:
  FILE *dicFl, *idxFl;
  BsDiIxHeadT2 *head;
  //code:
    //init0 vars:
  dicFl = NULL; idxFl = NULL;
  BS_DO_E_OUTE (head = bsdiixheadt2_new_tl ())
  //Head base:
  BS_DO_E_OUTE (bsdiixheadbs_load (pPth, (BsDiIxHeadBs*) head, &dicFl, &idxFl))
  if ( idxFl == NULL ) //not found
  {
    bsdiixheadt2_free (head);
    return;
  }
  //Audio offset:
  BS_DO_E_OUTE (bsfread_bsfoffset (&head->cntOfst, idxFl))

  *pHeadRt = head;
  *pDicFlRt = dicFl;
  *pIdxFlRt = idxFl;
  return;

oute:
  if ( dicFl != NULL )
              { fclose(dicFl); }
  if ( idxFl != NULL )
              { fclose(idxFl); }
  if ( head != NULL )
              { bsdiixheadt2_free (head); }
}

/**
 * <p>Save (write/overwrite) IDX RAM (in memory) into file.</p>
 * @param pDiIxRm IDX RAM.
 * @param pPth - dictionary path.
 * @set errno if error.
 **/
void
  bsdiixt2rm_save (BsDiIxT2Rm *pDiIxRm, char *pPth)
{
  //vars:
  FILE *idxFl;
  BS_CHAR_T bschr0;
  BS_IDX_T l;
  int wsz, lenr;
  //vars init0:
  bschr0 = 0;
  //Head base:
  BS_DO_E_OUT (idxFl = bsdiixheadbs_save ((BsDiIxHeadBs*) pDiIxRm->head, pPth))
  //Audio offset:
  BS_DO_E_OUT (bsfwrite_bsfoffset (&pDiIxRm->head->cntOfst, idxFl))
  //IRT fixed record size:
  for ( l = BS_IDX_0; l < pDiIxRm->head->irtSz; l++)
  {
    wsz = bsdicidx_istr_len (pDiIxRm->irt->vals[l]->iwrd) + 1;
    BS_DO_E_OUT (bsfwrite_bschars (pDiIxRm->irt->vals[l]->iwrd, wsz, idxFl))
    lenr = pDiIxRm->head->mxIrWdSz - wsz;
    if ( lenr > 0 )
            { BS_DO_E_OUT(bsfwrite_bscharn(&bschr0, lenr, idxFl)) }
    BS_DO_E_OUT (bsfwrite_uint (&pDiIxRm->irt->vals[l]->ofst, idxFl))
    BS_DO_E_OUT (bsfwrite_uint (&pDiIxRm->irt->vals[l]->len, idxFl))
  }
  BSLOG_LOG(BSLINFO, "%s with IDXRAM#%p has been successfully saved!\n", pPth, pDiIxRm);
out:
  fclose(idxFl);
}

/**
 * <p>Load LSA with index from dictionary file.</p>
 * @param pPth - dictionary path.
 * @return opened dictionary with index bundle or NULL if index file doesn't exist
 * @set errno if error.
 **/
BsDiIxT2*
  bsdiixt2_load (char *pPth)
{
  FILE *dicFl = NULL;
  FILE *idxFl = NULL;
  BsDiIxHeadT2 *head = NULL;
  BS_DO_E_RETN (bsdiixheadt2_load (pPth, &head, &dicFl, &idxFl))
  if ( head == NULL )
  { //no IDX
    return NULL;
  }
  BS_DO_E_OUTE (BsDiIxT2 *diIx = bsdiixt2_new (head, dicFl, idxFl))
  diIx->irtOfst = ftell (idxFl);
  return diIx;

oute:
  if ( diIx != NULL )
  {
    bsdiixt2_destroy (diIx);
  } else {
    if ( dicFl != NULL )
          { fclose (dicFl); }
    if ( idxFl != NULL )
          { fclose(idxFl); }
    if ( head != NULL )
          { bsdiixheadt2_free (head); }
  }
  return NULL;
}

//Find lib:
/**
 * <p>Find IRT record maximum matched given word inside given range in given dictionary.</p>
 * @param pDiIxRm - dictionary and its index in file
 * @param pIwrd - word(sub) to match in AB coding
 * @param pIrtStart index IRT start
 * @param pIrtEnd index IRT end
 * @return IRT record matched to word or NULL if not found.
 * @set errno if error.
 **/
BsDiIxT2IdxIrd*
  bsdiixt2tst_find_irtrd (BsDiIxT2 *pDiIx, BS_CHAR_T *pIwrd,
                         BS_IDX_T pIrtStart, BS_IDX_T pIrtEnd)
{
  //vars:
  BS_FOFST_T ofst;
  BS_IDX_T irtStart, irtEnd;
  unsigned int ofstRc, lenRc, ofstRcn, lenRcn;
  BS_CHAR_T irtstr[pDiIx->head->mxIrWdSz];
  BS_CHAR_T irtstrn[pDiIx->head->mxIrWdSz];
  BS_WCHAR_T wwrd[pDiIx->head->mxIrWdSz];
  int hcmpr, lenIrtwrd, lenIwrd;
  //code:
  ofst = BS_IDX_NULL;
  irtStart = pIrtStart;
  irtEnd = pIrtEnd;
  lenIwrd = bsdicidx_istr_len (pIwrd);
  bool isDbg = bslog_is_debug (BS_DEBUGL_DIIXT2 + 90);
  bool isDbg1 = bslog_is_debug (BS_DEBUGL_DIIXT2 + 99);
  if ( isDbg )
  {
    bsdicidxab_istrn_to_wstr (pIwrd, wwrd, lenIwrd, pDiIx->head->ab);
    BSLOG_LOG (BSLDEBUG, "Searching word=%ls, len=%d, start=%ld, end=%ld, mxIrWdSz=%d, IRT ofst=%ld\n", wwrd, lenIwrd, pIrtStart, pIrtEnd, pDiIx->head->mxIrWdSz, pDiIx->irtOfst)
  }
  while ( true )
  {
    BS_IDX_T midIdx = irtStart + ( irtEnd - irtStart ) / 2;
    ofst = midIdx * ( BSDIIXT2IRTRD_FIX_SZ (pDiIx->head->mxIrWdSz) ) + pDiIx->irtOfst;
    BS_DO_E_RETN (bsfseek_goto (pDiIx->idxFl, ofst))
    BS_DO_E_RETN (bsfread_bschars (irtstr, pDiIx->head->mxIrWdSz, pDiIx->idxFl))
    BS_DO_E_RETN (bsfread_uint (&ofstRc, pDiIx->idxFl))
    BS_DO_E_RETN (bsfread_uint (&lenRc, pDiIx->idxFl))
    hcmpr = bsdicidx_istr_how_match (pIwrd, irtstr);
    lenIrtwrd = bsdicidx_istr_len (irtstr);
    if ( isDbg1 )
    {
      bsdicidxab_istrn_to_wstr (irtstr, wwrd, lenIrtwrd, pDiIx->head->ab);
      BSLOG_LOG (BSLDEBUG, "IRT.word=%ls, len=%d, rofst=%u, rlen=%u, start=%ld, end=%ld, middle=%ld, cmpCNT=%d\n", wwrd, lenIrtwrd, ofstRc, lenRc, irtStart, irtEnd, midIdx, hcmpr)
    }
    if ( hcmpr > 0 )
    { //found 1st matched
      if ( isDbg )
      {
        bsdicidxab_istrn_to_wstr (irtstr, wwrd, lenIrtwrd, pDiIx->head->ab);
        BSLOG_LOG (BSLDEBUG, " Matched IRT.word=%ls, len=%d, start=%ld, end=%ld, middle=%ld, cmpCNT=%d\n", wwrd, lenIrtwrd, irtStart, irtEnd, midIdx, hcmpr)
      }
      if ( midIdx > BS_IDX_0 )
      { //check lower words
        BS_IDX_T irtidxn = midIdx;
        while ( --irtidxn >= BS_IDX_0 )
        {
          ofst = irtidxn * ( BSDIIXT2IRTRD_FIX_SZ (pDiIx->head->mxIrWdSz) ) + pDiIx->irtOfst;
          BS_DO_E_RETN (bsfseek_goto (pDiIx->idxFl, ofst))
          BS_DO_E_RETN (bsfread_bschars (irtstrn, pDiIx->head->mxIrWdSz, pDiIx->idxFl))
          BS_DO_E_RETN (bsfread_uint (&ofstRcn, pDiIx->idxFl))
          BS_DO_E_RETN (bsfread_uint (&lenRcn, pDiIx->idxFl))
          int hcmprn = bsdicidx_istr_how_match (pIwrd, irtstrn);
          if ( hcmprn >= hcmpr )
          {
            hcmpr = hcmprn;
            midIdx = irtidxn;
            bsdicidx_istr_cpy (irtstr, irtstrn);
            ofstRc = ofstRcn;
            lenRc = lenRcn;
            if ( isDbg )
            {
              lenIrtwrd = bsdicidx_istr_len (irtstr);
              bsdicidxab_istrn_to_wstr (irtstr, wwrd, lenIrtwrd, pDiIx->head->ab);
              BSLOG_LOG (BSLDEBUG, "Found IRT.word=%ls, len, start=%ld, end=%ld, middle=%ld, cmpCNT=%d\n", wwrd, lenIrtwrd, irtStart, irtEnd, midIdx, hcmpr)
            }
          } else {
            break;
          }
        }
      }
      if ( midIdx < pDiIx->head->irtSz - BS_IDX_1 )
      { //check higher words
        BS_IDX_T irtidxn = midIdx;
        while ( --irtidxn < pDiIx->head->irtSz )
        {
          ofst = irtidxn * ( BSDIIXT2IRTRD_FIX_SZ (pDiIx->head->mxIrWdSz) ) + pDiIx->irtOfst;
          BS_DO_E_RETN (bsfseek_goto (pDiIx->idxFl, ofst))
          BS_DO_E_RETN (bsfread_bschars (irtstrn, pDiIx->head->mxIrWdSz, pDiIx->idxFl))
          BS_DO_E_RETN (bsfread_uint (&ofstRcn, pDiIx->idxFl))
          BS_DO_E_RETN (bsfread_uint (&lenRcn, pDiIx->idxFl))
          int hcmprn = bsdicidx_istr_how_match (pIwrd, irtstrn);
          if ( hcmprn >= hcmpr )
          {
            hcmpr = hcmprn;
            midIdx = irtidxn;
            bsdicidx_istr_cpy (irtstr, irtstrn);
            ofstRc = ofstRcn;
            lenRc = lenRcn;
            if ( isDbg )
            {
              lenIrtwrd = bsdicidx_istr_len (irtstr);
              bsdicidxab_istrn_to_wstr (irtstr, wwrd, lenIrtwrd, pDiIx->head->ab);
              BSLOG_LOG (BSLDEBUG, "Found IRT.word=%ls, len, start=%ld, end=%ld, middle=%ld, cmpCNT=%d\n", wwrd, lenIrtwrd, irtStart, irtEnd, midIdx, hcmpr)
            }
          } else {
            break;
          }
        }
      }
      return bsdiixt2idxird_new (irtstr, ofstRc, lenRc, midIdx, pDiIx->head->mxIrWdSz);
    }
    if ( irtStart == irtEnd )
    {
      break;
    }
    if ( hcmpr == 0 )
    {
      if ( irtStart == midIdx )
                       { break; }
      irtStart = midIdx;
    } else { //LESS
      if ( irtEnd == midIdx )
                       { break; }
      irtEnd = midIdx;
    }
  }
  return NULL; //not found
}

/**
 * <p>Add matched word into array.</p>
 * @param pIrtRd record with matched i.word
 * @param pDiIx - dictionary and its index
 * @param pFdWrds - collection to add found record
 * @set errno if error.
 **/
void
  bsdiixt2tst_add_wrd (BsDiIxT2IdxIrd *pIrtRd, BsDiIxT2 *pDiIx, BsDiFdWds *pFdWrds)
{
  //vars:
  int rz;
  char cstr[pDiIx->head->mxIrWdSz + 7];
  BS_WCHAR_T wcstr[pDiIx->head->mxIrWdSz];
  //code:
  bsdicidxab_istrn_to_wstr (pIrtRd->iwrd, wcstr, pDiIx->head->mxIrWdSz, pDiIx->head->ab);
  rz =  wcstombs (cstr, wcstr, pDiIx->head->mxIrWdSz + 7);
  if ( rz <= 0 || errno != 0 )
  {
    if ( errno == 0 ) { errno = BSE_ERR; }
    BSLOG_LOG (BSLERROR, "wcstombs fail for %ls, rz=%d\n", wcstr, rz)
    return;
  }
  bsdifdwds_add_inc2 (pFdWrds, cstr, (BsDiIxBs*) pDiIx, pIrtRd->ofst, pIrtRd->len);
}

/**
 * <p>Fill mathched words with search data.</p>
 * @param pDiIxRm - dictionary and its index in file
 * @param pIwrd - word(sub) to match in AB coding
 * @param pIrtRd record with maximum matched i.word
 * @param pFdWrds - collection to add found record
 * @set errno if error.
 **/

void bsdiixt2tst_find_mtch (BsDiIxT2 *pDiIx, BS_CHAR_T *pIwrd,
                            BsDiIxT2IdxIrd *pIrtRd, BsDiFdWds *pFdWrds)
{
  //vars:
  BS_FOFST_T ofst;
  BS_WCHAR_T wcstr[pDiIx->head->mxIrWdSz];
  //code:
  ofst = BS_IDX_NULL;
  bool isDbg = bslog_is_debug (BS_DEBUGL_DIIXT2 + 80);
    //1-st word:
  if ( isDbg )
  {
    bsdicidxab_istrn_to_wstr (pIrtRd->iwrd, wcstr, pDiIx->head->mxIrWdSz, pDiIx->head->ab);
    BSLOG_LOG (BSLDEBUG, "Found IRT.word=%ls, middle=%ld, mxIrWdSz=%d, IRTSZ=%ld\n", wcstr, pIrtRd->idxTrnz, pDiIx->head->mxIrWdSz, pDiIx->head->irtSz)
  }
  BS_DO_E_RET (bsdiixt2tst_add_wrd (pIrtRd, pDiIx, pFdWrds))
  if ( pIrtRd->idxTrnz < pDiIx->head->irtSz - BS_IDX_1 )
  { //check higher words
    while ( ++pIrtRd->idxTrnz < pDiIx->head->irtSz )
    {
      ofst = pIrtRd->idxTrnz * ( BSDIIXT2IRTRD_FIX_SZ (pDiIx->head->mxIrWdSz) ) + pDiIx->irtOfst;
      BS_DO_E_RET (bsfseek_goto (pDiIx->idxFl, ofst))
      BS_DO_E_RET (bsfread_bschars (pIrtRd->iwrd, pDiIx->head->mxIrWdSz, pDiIx->idxFl))
      BS_DO_E_RET (bsfread_uint (&pIrtRd->ofst, pDiIx->idxFl))
      BS_DO_E_RET (bsfread_uint (&pIrtRd->len, pDiIx->idxFl))
      if ( bsdicidx_istr_cont (pIrtRd->iwrd, pIwrd) )
      {
        BS_DO_E_RET (bsdiixt2tst_add_wrd (pIrtRd, pDiIx, pFdWrds))
        if ( isDbg )
        {
          bsdicidxab_istrn_to_wstr (pIrtRd->iwrd, wcstr, pDiIx->head->mxIrWdSz, pDiIx->head->ab);
          BSLOG_LOG (BSLDEBUG, "Found IRT.word=%ls, middle=%ld\n", wcstr, pIrtRd->idxTrnz)
        }
      } else {
        return;
      }
    }
  }
}
