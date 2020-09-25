/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

#include "stdlib.h"
#include "string.h"
#include "wctype.h"

#include "BsError.h"
#include "BsLog.h"
#include "BsStrings.h"
#include "BsFioWrap.h"
#include "BsDiIxFind.h"

/**
 * <p>Beigesoft™ dictionary index finder library.</p>
 * @author Yury Demidenko
 **/

/**
 * <p>Read word in given dictionary and IDX in RAM and added into given matched array.
 * IDX DWOLT offset is already pointed.
 * </p>
 * @param pDiIxRm - dictionary and its whole index in memory
 * @param pIwrd - word(sub) to match in AB coding
 * @param p_dwoltidx DWOLT idx
 * @param pFdWrds - collection to add found record
 * @set errno if error.
 **/
void
  bsdiixrm_read_wrd (BsDiIxTxRm *pDiIxRm, BS_CHAR_T *pIwrd,
                     BS_IDX_T p_dwoltidx, BsDiFdWds *pFdWrds)
{
  //read word in DIC:
  BS_DO_E_RET(bsfseek_goto(pDiIxRm->dicFl, pDiIxRm->dwolt[p_dwoltidx]->offset_dword))
  int wsz = pDiIxRm->dwolt[p_dwoltidx]->length_dword + 1;
  char wrd[wsz + 7];
  BS_DO_E_RET(bsfread_chars(wrd,
    pDiIxRm->dwolt[p_dwoltidx]->length_dword, pDiIxRm->dicFl))
  wrd[pDiIxRm->dwolt[p_dwoltidx]->length_dword] = 0;
  bsstring_escape_bslash(wrd);
  bsstring_escape_bounds_spaces(wrd);
  BS_WCHAR_T wstr[wsz];
  BS_CHAR_T istr[wsz];
  int rz = mbstowcs (wstr, wrd, wsz);
  if ( rz <= 0 || errno != 0)
  {
    if ( errno == 0 ) { errno = BSE_ERR; }
    BSLOG_LOG (BSLERROR, "mbstowcs fail for %s, rz=%d \n", wrd, rz)
    return;
  }
//BSLOG_LOG (BSLDEBUG, "wstr %ls\n", wstr)
  for ( int i = 0; ; i++ )
  {
    if ( wstr[i] == 0 )
                    { break; }
    if ( iswalpha(wstr[i]) )
                    { wstr[i] = towlower(wstr[i]); }
  }
  rz =  wcstombs (wrd, wstr, wsz + 7);
  if ( rz <= 0 || errno != 0 )
  {
    if ( errno == 0 ) { errno = BSE_ERR; }
    BSLOG_LOG (BSLERROR, "wcstombs fail for %ls, rz=%d\n", wstr, rz)
    return;
  }
  bsdicidxab_wstr_to_istr (wstr, istr, pDiIxRm->head->ab);
  if ( bsdicidx_istr_cont (istr, pIwrd) )
  { //filter first DWOLT words that belong to the same IRTRD and lower than requested word
    bsdifdwds_add_inc1 (pFdWrds, wrd, (BsDiIxBs*) pDiIxRm, pDiIxRm->dwolt[p_dwoltidx]->offset_dword);
  }
}

/**
 * <p>Read only word in given dictionary and IDX file.</p>
 * @param pDiIx - dictionary and its whole index in memory
 * @param p_dwoltidx DWOLT idx
 * @return read string or NULL if error
 * @set errno if error.
 **/
BsDicString*
  bsdiix_read_owrd (BsDiIxTx *pDiIx, BS_IDX_T p_dwoltidx)
{
  //read word in DIC:
  BS_FOFST_T dwofst;
  BS_SMALL_T dwlen;
  BS_DO_E_RETN (bsfseek_goto (pDiIx->idxFl, p_dwoltidx * (BDI_DWOLTRD_SIZE) + pDiIx->dwoltOfst))
  BS_DO_E_RETN (bsfread_bsfoffset (&dwofst, pDiIx->idxFl))
  BS_DO_E_RETN (bsfread_bssmall (&dwlen, pDiIx->idxFl))
  BS_DO_E_RETN (bsfseek_goto (pDiIx->dicFl, dwofst))
  char wrdb[dwlen + 8];
  BS_DO_E_RETN (bsfread_chars (wrdb, dwlen, pDiIx->dicFl))
  wrdb[dwlen] = 0;
  bsstring_escape_bslash (wrdb);
  bsstring_escape_bounds_spaces (wrdb);
  BS_WCHAR_T wstr[dwlen + 1];
  int rz = mbstowcs (wstr, wrdb, dwlen + 1);
  if ( rz <= 0 || errno != 0)
  {
    if ( errno == 0 ) { errno = BSE_ERR; }
    BSLOG_LOG (BSLERROR, "mbstowcs fail for %s, rz=%d \n", wrdb, rz)
    return NULL;
  }
  for ( int i = 0; ; i++ )
  {
    if ( wstr[i] == 0 )
                    { break; }
    if ( iswalpha(wstr[i]) )
                    { wstr[i] = towlower(wstr[i]); }
  }
  rz =  wcstombs (wrdb, wstr, dwlen + 8);
  if ( rz <= 0 || errno != 0)
  {
    if ( errno == 0 ) { errno = BSE_ERR; }
    BSLOG_LOG (BSLERROR, "wcstombs fail for %ls, rz=%d\n", wstr, rz)
    return NULL;
  }
  BsDicString *obj = bsdicstring_new(wrdb, dwofst);
  return obj;
}

/**
 * <p>Read word in given dictionary and IDX file and added into given matched array.
 * </p>
 * @param pDiIx - dictionary and its whole index in memory
 * @param pIwrd - word(sub) to match in AB coding
 * @param p_dwoltidx DWOLT idx
 * @param pFdWrds - collection to add found record
 * @set errno if error.
 **/
void
  bsdiix_read_wrd (BsDiIxTx *pDiIx, BS_CHAR_T *pIwrd,
                  BS_IDX_T p_dwoltidx, BsDiFdWds *pFdWrds)
{ //read word in DIC:
  BS_FOFST_T dwofst;
  BS_SMALL_T dwlen;
  BS_DO_E_RET (bsfseek_goto (pDiIx->idxFl, p_dwoltidx * (BDI_DWOLTRD_SIZE) + pDiIx->dwoltOfst))
  BS_DO_E_RET (bsfread_bsfoffset (&dwofst, pDiIx->idxFl))
  BS_DO_E_RET (bsfread_bssmall (&dwlen, pDiIx->idxFl))
  BS_DO_E_RET (bsfseek_goto (pDiIx->dicFl, dwofst))
  char wrdb[dwlen + 8];
  BS_DO_E_RET (bsfread_chars (wrdb, dwlen, pDiIx->dicFl))
  wrdb[dwlen] = 0;
//BSLOG_LOG (BSLDEBUG, "str was %s\n", wrdb)
  bsstring_escape_bslash(wrdb);
  bsstring_escape_bounds_spaces(wrdb);
  BS_WCHAR_T wstr[dwlen + 1];
  BS_CHAR_T istr[dwlen + 1];
  int rz = mbstowcs (wstr, wrdb, dwlen + 1);
  if ( rz <= 0 || errno != 0)
  {
    if ( errno == 0 ) { errno = BSE_ERR; }
    BSLOG_LOG (BSLERROR, "mbstowcs fail for %s, rz=%d \n", wrdb, rz)
    return;
  }
//BSLOG_LOG (BSLDEBUG, "wstr %ls\n", wstr)
  for ( int i = 0; ; i++ )
  {
    if ( wstr[i] == 0 )
                    { break; }
    if ( iswalpha(wstr[i]) )
                    { wstr[i] = towlower(wstr[i]); }
  }
  rz =  wcstombs (wrdb, wstr, dwlen + 8);
  if ( rz <= 0 || errno != 0)
  {
    if ( errno == 0 ) { errno = BSE_ERR; }
    BSLOG_LOG (BSLERROR, "wcstombs fail for %ls, rz=%d\n", wstr, rz)
    return;
  }
  bsdicidxab_wstr_to_istr (wstr, istr, pDiIx->head->ab);
  if ( bsdicidx_istr_cont (istr, pIwrd) )
  { //filter first DWOLT words that belong to the same IRTRD and lower than requested word
    bsdifdwds_add_inc1 (pFdWrds, wrdb, (BsDiIxBs*) pDiIx, dwofst);
  }
}

/**
 * <p>Find exactly matched word in given dictionary and IDX file.</p>
 * @param pDiIx - dictionary and its whole index in memory
 * @param pWrd - word to match
 * @param pIwrd - word to match in AB coding
 * @param p_irtrd first matched IRT record
 * @return d.string if found or NULL
 * @set errno if error.
 **/
BsDicString *bsdiixfindtst_omtch(BsDiIxTx *pDiIx, BsString *pWrd,
  BS_CHAR_T *pIwrd, BsDicFindIrtRd *p_irtrd) {
  bool isdbg = bslog_is_debug(BS_DEBUGL_DICIDXFIND + 70);
  if(isdbg) {
    BSLOG_LOG(BSLDEBUG, "word: %s\n", pWrd->val)
  }
  if (p_irtrd->dwolt_start != BS_IDX_NULL) { //where is direct iwords:
    BS_IDX_T dwidx;
    BS_FOFST_T ofst;
    if (p_irtrd->idx == pDiIx->head->irtSz - BS_IDX_1) {
      for (dwidx = p_irtrd->dwolt_start;
        dwidx < pDiIx->head->dwoltSz; dwidx++) {
        BS_DO_E_RETN(BsDicString *candidate = bsdiix_read_owrd(pDiIx, dwidx))
        if(isdbg) {
          BSLOG_LOG(BSLDEBUG, "candidate: %s\n", candidate->val)
        }
        if (strcmp(candidate->val, pWrd->val) == 0) {
          return candidate;
        }
        bsdicstring_free(candidate);
      }
    } else {
      BS_IDX_T dwidxprev = p_irtrd->dwolt_start - BS_IDX_1;
      while (true) {
        bool is_str_match = bsdicidx_istr_how_match(pIwrd, p_irtrd->idx_subwrd) > 0;
        if (p_irtrd->dwolt_start != BS_IDX_NULL) {
          for (dwidx = dwidxprev + BS_IDX_1; dwidx <= p_irtrd->dwolt_start; dwidx++) {
            if (!is_str_match && dwidx == p_irtrd->dwolt_start) {
              //stop range from previous matched dwolt_start to current unmatched one
              return NULL;
            }
            BS_DO_E_RETN(BsDicString *candidate = bsdiix_read_owrd(pDiIx, dwidx))
            if(isdbg) {
              BSLOG_LOG(BSLDEBUG, "candidate: %s\n", candidate->val)
            }
            if (strcmp(candidate->val, pWrd->val) == 0) {
              return candidate;
            }
            bsdicstring_free(candidate);
            dwidxprev = p_irtrd->dwolt_start;
          }
        }
        if (!is_str_match) {
          return NULL;
        }
        p_irtrd->idx++;
        if (p_irtrd->idx == pDiIx->head->irtSz) {
          break;
        }
        ofst = p_irtrd->idx * (BDI_IRTRD_FIXED_SIZE(pDiIx->head->mxIrWdSz)) + pDiIx->irtOfst;
        BS_DO_E_RETN(bsfseek_goto(pDiIx->idxFl, ofst))
        BS_DO_E_RETN(bsfread_bschars(p_irtrd->idx_subwrd, pDiIx->head->mxIrWdSz, pDiIx->idxFl))
        BS_DO_E_RETN(bsfread_bsindex(&p_irtrd->dwolt_start, pDiIx->idxFl))
        BS_DO_E_RETN(bsfread_bssmall(&p_irtrd->i2wpt_quantity, pDiIx->idxFl))
        BS_DO_E_RETN(bsfread_bsindex(&p_irtrd->i2wpt_start, pDiIx->idxFl))
      }
    }
  }
  return NULL;
}

/**
 * <p>Find all matched words in given dictionary and IDX file.</p>
 * @param pDiIx - dictionary and its whole index in memory
 * @param pIwrd - word(sub) to match in AB coding
 * @param p_irtrd first matched IRT record
 * @param pFdWrds - collection to add found record
 * @set errno if error.
 **/
void bsdiixfindtst_mtch(BsDiIxTx *pDiIx,
  BS_CHAR_T *pIwrd, BsDicFindIrtRd *p_irtrd, BsDiFdWds *pFdWrds)
{
  BS_IDX_T dwidx;
  BS_FOFST_T ofst;
  if (p_irtrd->dwolt_start != BS_IDX_NULL) { //where is direct iwords:
    if (p_irtrd->idx == pDiIx->head->irtSz - BS_IDX_1) {
      for (dwidx = p_irtrd->dwolt_start;
        dwidx < pDiIx->head->dwoltSz; dwidx++) {
        BS_DO_E_RET (bsdiix_read_wrd (pDiIx, pIwrd, dwidx, pFdWrds))
        if (pFdWrds->size >= BDI_MAX_MATCHED_WORDS) {
          return;
        }
      }
      if (p_irtrd->i2wpt_quantity > 0) {
        BS_IDX_T i2wptcl = p_irtrd->i2wpt_quantity;
        for (BS_IDX_T l = BS_IDX_0; l < i2wptcl; l++) {
          ofst = (p_irtrd->i2wpt_start + l) * (BDI_I2WPTRD_SIZE) + pDiIx->i2wptOfst;
          BS_DO_E_RET(bsfseek_goto(pDiIx->idxFl, ofst))
          BS_DO_E_RET(bsfread_bsindex(&dwidx, pDiIx->idxFl))
          BS_DO_E_RET(bsdiix_read_wrd (pDiIx, pIwrd, dwidx, pFdWrds))
          if (pFdWrds->size >= BDI_MAX_MATCHED_WORDS) {
            return;
          }
        }
      }
    } else {
      BS_IDX_T dwidxprev = p_irtrd->dwolt_start - BS_IDX_1;
      while (true) {
        bool is_str_match = bsdicidx_istr_how_match(pIwrd, p_irtrd->idx_subwrd) > 0;
        if (p_irtrd->dwolt_start != BS_IDX_NULL) {
          for (dwidx = dwidxprev + BS_IDX_1; dwidx <= p_irtrd->dwolt_start; dwidx++) {
            if (!is_str_match && dwidx == p_irtrd->dwolt_start) {
              //stop range from previous matched dwolt_start to current unmatched one
              return;
            }
            BS_DO_E_RET(bsdiix_read_wrd (pDiIx, pIwrd, dwidx, pFdWrds))
            if ( pFdWrds->size >= BDI_MAX_MATCHED_WORDS ) {
              return;
            }
            dwidxprev = p_irtrd->dwolt_start;
          }
        }
        if (!is_str_match) {
          return;
        }
        if (p_irtrd->i2wpt_quantity > 0) {
          BS_IDX_T i2wptcl = p_irtrd->i2wpt_quantity;
          for (BS_IDX_T l = BS_IDX_0; l < i2wptcl; l++) {
            ofst = (p_irtrd->i2wpt_start + l) * (BDI_I2WPTRD_SIZE) + pDiIx->i2wptOfst;
            BS_DO_E_RET(bsfseek_goto(pDiIx->idxFl, ofst))
            BS_DO_E_RET(bsfread_bsindex(&dwidx, pDiIx->idxFl))
            BS_DO_E_RET(bsdiix_read_wrd (pDiIx, pIwrd, dwidx, pFdWrds))
            if (pFdWrds->size >= BDI_MAX_MATCHED_WORDS) {
              return;
            }
          }
        }
        p_irtrd->idx++;
        if (p_irtrd->idx == pDiIx->head->irtSz) {
          break;
        }
        ofst = p_irtrd->idx * (BDI_IRTRD_FIXED_SIZE(pDiIx->head->mxIrWdSz)) + pDiIx->irtOfst;
        BS_DO_E_RET(bsfseek_goto(pDiIx->idxFl, ofst))
        BS_DO_E_RET(bsfread_bschars(p_irtrd->idx_subwrd, pDiIx->head->mxIrWdSz, pDiIx->idxFl))
        BS_DO_E_RET(bsfread_bsindex(&p_irtrd->dwolt_start, pDiIx->idxFl))
        BS_DO_E_RET(bsfread_bssmall(&p_irtrd->i2wpt_quantity, pDiIx->idxFl))
        BS_DO_E_RET(bsfread_bsindex(&p_irtrd->i2wpt_start, pDiIx->idxFl))
      }
    }
  } else { //only i2words:
    if (p_irtrd->i2wpt_quantity <= 0) { //previous validation must fail
      errno = BSE_VALIDATE_ERR;
      BSLOG_ERR
      return;
    }
    BS_IDX_T i2wptcl = p_irtrd->i2wpt_quantity;
    for (BS_IDX_T l = BS_IDX_0; l < i2wptcl; l++) {
      ofst = (p_irtrd->i2wpt_start + l) * (BDI_I2WPTRD_SIZE) + pDiIx->i2wptOfst;
      BS_DO_E_RET(bsfseek_goto(pDiIx->idxFl, ofst))
      BS_DO_E_RET(bsfread_bsindex(&dwidx, pDiIx->idxFl))
      BS_DO_E_RET(bsdiix_read_wrd (pDiIx, pIwrd, dwidx, pFdWrds))
      if (pFdWrds->size >= BDI_MAX_MATCHED_WORDS) {
        return;
      }
    }
  }
}

/**
 * <p>Find all matched words in given dictionary and IDX in RAM by given IRT record's index.</p>
 * @param pDiIxRm - dictionary and its whole index in memory
 * @param pIwrd - word(sub) to match in AB coding
 * @param p_irtidx IRT record index
 * @param pFdWrds - collection to add found record
 * @set errno if error.
 **/
void
  bsdiixrmfindtst_mtch (BsDiIxTxRm *pDiIxRm, BS_CHAR_T *pIwrd,
    BS_IDX_T p_irtidx, BsDiFdWds *pFdWrds)
{
  if (pDiIxRm->irt[p_irtidx]->dwolt_start != BS_IDX_NULL) { //where is direct iwords:
    BS_IDX_T dwidx;
    if (p_irtidx == pDiIxRm->head->irtSz - BS_IDX_1) {
      for (dwidx = pDiIxRm->irt[p_irtidx]->dwolt_start;
        dwidx < pDiIxRm->head->dwoltSz; dwidx++) {
        BS_DO_E_RET (bsdiixrm_read_wrd (pDiIxRm, pIwrd, dwidx, pFdWrds))
        if (pFdWrds->size >= BDI_MAX_MATCHED_WORDS) {
          return;
        }
      }
      if (pDiIxRm->irt[p_irtidx]->i2wpt_quantity > 0) {
        BS_IDX_T i2wptcl = pDiIxRm->irt[p_irtidx]->i2wpt_quantity;
        for (BS_IDX_T l = BS_IDX_0; l < i2wptcl; l++) {
          BS_DO_E_RET (bsdiixrm_read_wrd (pDiIxRm, pIwrd,
            pDiIxRm->i2wpt[pDiIxRm->irt[p_irtidx]->i2wpt_start + l], pFdWrds))
          if (pFdWrds->size >= BDI_MAX_MATCHED_WORDS) {
            return;
          }
        }
      }
    } else {
      BS_IDX_T irtidxn = p_irtidx;
      BS_IDX_T dwidxprev = pDiIxRm->irt[p_irtidx]->dwolt_start - BS_IDX_1;
      do {
        bool is_str_match = bsdicidx_istr_how_match(pIwrd, pDiIxRm->irt[irtidxn]->idx_subwrd) > 0;
        if (pDiIxRm->irt[irtidxn]->dwolt_start != BS_IDX_NULL) {
          for (dwidx = dwidxprev + BS_IDX_1; dwidx <= pDiIxRm->irt[irtidxn]->dwolt_start; dwidx++) {
            if (!is_str_match && dwidx == pDiIxRm->irt[irtidxn]->dwolt_start) {
              return;
            }
            BS_DO_E_RET (bsdiixrm_read_wrd (pDiIxRm, pIwrd, dwidx, pFdWrds))
            if (pFdWrds->size >= BDI_MAX_MATCHED_WORDS) {
              return;
            }
            dwidxprev = pDiIxRm->irt[irtidxn]->dwolt_start;
          }
        }
        if (!is_str_match) {
          return;
        }
        if (pDiIxRm->irt[irtidxn]->i2wpt_quantity > 0) {
          BS_IDX_T i2wptcl = pDiIxRm->irt[irtidxn]->i2wpt_quantity;
          for (BS_IDX_T l = BS_IDX_0; l < i2wptcl; l++) {
            BS_DO_E_RET (bsdiixrm_read_wrd (pDiIxRm, pIwrd,
              pDiIxRm->i2wpt[pDiIxRm->irt[irtidxn]->i2wpt_start + l], pFdWrds))
            if (pFdWrds->size >= BDI_MAX_MATCHED_WORDS) {
              return;
            }
          }
        }
        irtidxn++;
      } while (irtidxn < pDiIxRm->head->irtSz);
    }
  } else { //only i2words:
    if (pDiIxRm->irt[p_irtidx]->i2wpt_quantity <= 0) { //previous validation must fail
      errno = BSE_VALIDATE_ERR;
      BSLOG_ERR
      return;
    }
    BS_IDX_T i2wptcl = pDiIxRm->irt[p_irtidx]->i2wpt_quantity;
    for (BS_IDX_T l = BS_IDX_0; l < i2wptcl; l++) {
      BS_DO_E_RET (bsdiixrm_read_wrd (pDiIxRm, pIwrd,
        pDiIxRm->i2wpt[pDiIxRm->irt[p_irtidx]->i2wpt_start + l], pFdWrds))
      if (pFdWrds->size >= BDI_MAX_MATCHED_WORDS) {
        return;
      }
    }
  }
}

/**
 * <p>Find MAXIMUM matched given word IRT record inside given range in given dictionary.</p>
 * @param pDiIxRm - dictionary and its whole index in memory
 * @param pIwrd - word(sub) to match in AB coding
 * @param pIrtStart index IRT start
 * @param pIrtEnd index IRT end
 * @return IRT record matched to word or NULL if not found.
 * @set errno if error.
 **/
BsDicFindIrtRd*
  bsdiixfindtst_irtrd (BsDiIxTx *pDiIx, BS_CHAR_T *pIwrd,
                            BS_IDX_T pIrtStart, BS_IDX_T pIrtEnd)
{
#define BSULD1_CHR_CNT 100
  BS_WCHAR_T wrd[BSULD1_CHR_CNT];
  bool isDbg = bslog_is_debug (BS_DEBUGL_DICIDXFIND + 80);
  BS_IDX_T irtStart = pIrtStart;
  BS_IDX_T irtEnd = pIrtEnd;
  BS_CHAR_T irtstr[pDiIx->head->mxIrWdSz];
  BS_FOFST_T ofst = BS_IDX_NULL;
  if ( isDbg )
  {
    bsdicidxab_istrn_to_wstr (pIwrd, wrd, BSULD1_CHR_CNT, pDiIx->head->ab);
    BSLOG_LOG (BSLDEBUG, "Try to find the most matched IRT word to %ls\n",wrd)
  }
  while ( true )
  {
    BS_IDX_T midIdx = irtStart + ( irtEnd - irtStart ) / 2;
    ofst = midIdx * ( BDI_IRTRD_FIXED_SIZE (pDiIx->head->mxIrWdSz) ) + pDiIx->irtOfst;
    BS_DO_E_RETN (bsfseek_goto (pDiIx->idxFl, ofst))
    BS_DO_E_RETN (bsfread_bschars (irtstr, pDiIx->head->mxIrWdSz, pDiIx->idxFl))
    int hcmpr = bsdicidx_istr_how_match (pIwrd, irtstr);
    if ( hcmpr > 0 )
    { //found 1st matched
      if ( isDbg )
      {
        bsdicidxab_istrn_to_wstr (irtstr, wrd, BSULD1_CHR_CNT, pDiIx->head->ab);
        BSLOG_LOG (BSLDEBUG, " found %ls, cntm=%d\n", wrd, hcmpr)
      }
      BSDICFINDIRTRD_NEW_E_RETN (irtrd, midIdx, irtstr, pDiIx->head->mxIrWdSz)
      BS_DO_E_RETN (bsfread_bsindex (&irtrd->dwolt_start, pDiIx->idxFl))
      BS_DO_E_RETN (bsfread_bssmall (&irtrd->i2wpt_quantity, pDiIx->idxFl))
      BS_DO_E_RETN (bsfread_bsindex (&irtrd->i2wpt_start, pDiIx->idxFl))
      if ( midIdx > BS_IDX_0 )
      { //check lower words
        BS_IDX_T irtidxn = midIdx;
        while ( --irtidxn >= BS_IDX_0 )
        {
          ofst = irtidxn * ( BDI_IRTRD_FIXED_SIZE(pDiIx->head->mxIrWdSz) ) + pDiIx->irtOfst;
          BS_DO_E_RETN (bsfseek_goto (pDiIx->idxFl, ofst))
          BS_DO_E_RETN (bsfread_bschars (irtstr, pDiIx->head->mxIrWdSz, pDiIx->idxFl))
          int hcmprn = bsdicidx_istr_how_match (pIwrd, irtstr);
          if ( isDbg )
          {
            bsdicidxab_istrn_to_wstr (irtstr, wrd, BSULD1_CHR_CNT, pDiIx->head->ab);
            BSLOG_LOG (BSLDEBUG, " found back %ls, cntm=%d\n", wrd, hcmprn)
          }
          if ( hcmprn >= hcmpr )
          { //found equal or more matched
            hcmpr = hcmprn;
            midIdx = irtidxn;
            irtrd->idx = irtidxn;
            bsdicidx_istr_cpy (irtrd->idx_subwrd, irtstr);
            BS_DO_E_RETN (bsfread_bsindex (&irtrd->dwolt_start, pDiIx->idxFl))
            BS_DO_E_RETN (bsfread_bssmall (&irtrd->i2wpt_quantity, pDiIx->idxFl))
            BS_DO_E_RETN (bsfread_bsindex (&irtrd->i2wpt_start, pDiIx->idxFl))
          } else {
            break;
          }
        }
      }
      if ( midIdx < pDiIx->head->irtSz - BS_IDX_1 )
      { //check higher words
        BS_IDX_T irtidxn = midIdx;
        while ( ++irtidxn < pDiIx->head->irtSz )
        {
          ofst = irtidxn * ( BDI_IRTRD_FIXED_SIZE (pDiIx->head->mxIrWdSz) ) + pDiIx->irtOfst;
          BS_DO_E_RETN (bsfseek_goto (pDiIx->idxFl, ofst))
          BS_DO_E_RETN (bsfread_bschars (irtstr, pDiIx->head->mxIrWdSz, pDiIx->idxFl))
          int hcmprn = bsdicidx_istr_how_match (pIwrd, irtstr);
          if ( isDbg )
          {
            bsdicidxab_istrn_to_wstr (irtstr, wrd, BSULD1_CHR_CNT, pDiIx->head->ab);
            BSLOG_LOG (BSLDEBUG, " found forth %ls, cntm=%d\n", wrd, hcmprn)
          }
          if ( hcmprn >= hcmpr )
          { //found equal or more matched
            hcmpr = hcmprn;
            midIdx = irtidxn;
            irtrd->idx = irtidxn;
            bsdicidx_istr_cpy (irtrd->idx_subwrd, irtstr);
            BS_DO_E_RETN (bsfread_bsindex (&irtrd->dwolt_start, pDiIx->idxFl))
            BS_DO_E_RETN (bsfread_bssmall (&irtrd->i2wpt_quantity, pDiIx->idxFl))
            BS_DO_E_RETN (bsfread_bsindex (&irtrd->i2wpt_start, pDiIx->idxFl))
          } else {
            break;
          }
        }
      }
      return irtrd;
    }
    if ( irtStart == irtEnd )
                { break; }

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
 * <p>Find matched given word IRT record index inside given range in given dictionary.</p>
 * @param pDiIxRm - dictionary and its whole index in memory
 * @param pIwrd - word(sub) to match in AB coding
 * @param pIrtStart index IRT start
 * @param pIrtEnd index IRT end
 * @return Index in IRT with matched word or BS_IDX_NULL if not found.
 * @set errno if error.
 **/
BS_IDX_T bsdiixrmfindtst_irtix(BsDiIxTxRm *pDiIxRm, BS_CHAR_T *pIwrd,
  BS_IDX_T pIrtStart, BS_IDX_T pIrtEnd) {
  BS_IDX_T irtStart = pIrtStart;
  BS_IDX_T irtEnd = pIrtEnd;
  while (TRUE) {
    BS_IDX_T midIdx = irtStart + (irtEnd - irtStart) / 2;
    int hcmpr = bsdicidx_istr_how_match(pIwrd, pDiIxRm->irt[midIdx]->idx_subwrd);
    if (hcmpr > 0) { //found 1st matched
      BS_IDX_T irtidxn = midIdx;
      if (midIdx > BS_IDX_0) { //check lower words
        while (--irtidxn >= BS_IDX_0) {
          hcmpr = bsdicidx_istr_how_match(pIwrd, pDiIxRm->irt[irtidxn]->idx_subwrd);
          if (hcmpr > 0) {
            midIdx = irtidxn;
          } else {
            break;
          }
        }
      }
      return midIdx;
    }
    if (irtStart == irtEnd) {
      break;
    }
    if (hcmpr == 0) {
      if ( irtStart == midIdx )
                { break; }
      irtStart = midIdx;
    } else { //LESS
      if ( irtEnd == midIdx )
                { break; }
      irtEnd = midIdx;
    }
  }
  return BS_IDX_NULL; //not found
}

//public lib:

/**
 * <p>Constructor.</p>
 * @param pIdx - index
 * @param pIdxsubwrd - sub-word
 * @param p_max_irtwrd_size -  max IRT sub-word size
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicFindIrtRd *bsdicfindirtrd_new(BS_IDX_T pIdx, BS_CHAR_T *pIdxsubwrd,
  int p_max_irtwrd_size) {
  BsDicFindIrtRd *obj = malloc(sizeof(BsDicFindIrtRd));
  if (obj != NULL) {
    obj->idx_subwrd_size = bsdicidx_istr_len(pIdxsubwrd) + 1;
    obj->idx_subwrd = malloc(p_max_irtwrd_size * BS_CHAR_LEN);
    if (obj->idx_subwrd == NULL) {
      obj = bsdicfindirtrd_free(obj);
    } else {
      bsdicidx_istr_cpy(obj->idx_subwrd, pIdxsubwrd);
      obj->dwolt_start = BS_IDX_NULL;
      obj->i2wpt_quantity = 0;
      obj->i2wpt_start = BS_IDX_NULL;
      obj->idx = pIdx;
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
 * @param p_irt_rcd IRT record.
 * @return always NULL
 **/
BsDicFindIrtRd *bsdicfindirtrd_free(BsDicFindIrtRd *p_irt_rcd) {
  if (p_irt_rcd != NULL) {
    if (p_irt_rcd->idx_subwrd != NULL) {
      free(p_irt_rcd->idx_subwrd);
    }
    free(p_irt_rcd);
  }
  return NULL;
}

/**
 * <p>Find all matched words in given dictionary and IDX.</p>
 * @param pDiIx - DIC with IDX
 * @param pFdWrds - collection to add found record
 * @param pSbwrd - sub-word to match
 * @set errno if error.
 **/
void
  bsdiixtxfind_mtch (BsDiIxTx *pDiIx, BsDiFdWds *pFdWrds, char *pSbwrd)
{
  bool isdbg = bslog_is_debug (BS_DEBUGL_DICIDXFIND);
  int len = strlen (pSbwrd);
  if ( len < 1 )
  { 
    errno = BSE_WRONG_PARAMS;
    BSLOG_ERR
    return;
  }
  BS_CHAR_T isubwrd[len + 1];
  BS_DO_E_RET (bsdicidxab_str_to_istr (pSbwrd, isubwrd, pDiIx->head->ab))
  //1.find irtStRn of first up to two chars and irtEnRn:
  BS_IDX_T irtStRn = BS_IDX_0;
  BS_IDX_T irtEnRn = pDiIx->head->irtSz - BS_IDX_1;
  BS_DO_E_RET (bsdicidx_find_irtrange(pDiIx->head->hirt, pDiIx->head->hirtSz, isubwrd, &irtStRn, &irtEnRn))
  //2.find IRT record inside irtStRn-irtEnRn:
  BsDicFindIrtRd *irtrd = bsdiixfindtst_irtrd(pDiIx, isubwrd, irtStRn, irtEnRn);
  if ( irtrd == NULL ) {
    if ( errno != 0 )
    {
      if ( pDiIx->head->nme != NULL && pDiIx->head->nme->val != NULL )
      {
        BSLOG_LOG (BSLERROR, "Error on sub-word=%s dic=%s\n", pSbwrd, pDiIx->head->nme->val)
      } else {
        BSLOG_LOG (BSLERROR, "Error on sub-word=%s dic#%p\n", pSbwrd, pDiIx)
      }
    }
    return;
  }
  if (isdbg) {
    BSLOG_LOG (BSLDEBUG, "irtStRn="BS_IDX_FMT", irtEnRn="BS_IDX_FMT", irtidx="BS_IDX_FMT"\n", irtStRn, irtEnRn, irtrd->idx);
  }
  //3.read all DWOLT records and fill result:
  bsdiixfindtst_mtch (pDiIx, isubwrd, irtrd, pFdWrds); //without err.prop.
  bsdicfindirtrd_free(irtrd);
}

/**
 * <p>Find all matched words in given dictionary and IDX.</p>
 * @param pDiIxRm - DIC with IDX in RAM
 * @param pFdWrds - collection to add found record
 * @param pSbwrd - sub-word to match
 * @set errno if error.
 **/
void
  bsdiixtxrmfind_mtch (BsDiIxTxRm *pDiIxRm, BsDiFdWds *pFdWrds, char *pSbwrd)
{
  bool isdbg = bslog_is_debug (BS_DEBUGL_DICIDXFIND);
  int len = strlen (pSbwrd);
  BS_IF_EN_RET ( len < 1, BSE_WRONG_PARAMS)
  BS_CHAR_T isubwrd[len + 1];
  BS_DO_E_RET (bsdicidxab_str_to_istr (pSbwrd, isubwrd, pDiIxRm->head->ab))
  //1.find irtStRn of first up to two chars and irtEnRn:
  BS_IDX_T irtStRn = BS_IDX_0;
  BS_IDX_T irtEnRn = pDiIxRm->head->irtSz - BS_IDX_1;
  BS_DO_E_RET (bsdicidx_find_irtrange(pDiIxRm->head->hirt, pDiIxRm->head->hirtSz, isubwrd, &irtStRn, &irtEnRn))
  BS_DO_E_RET (BS_IDX_T irtidx = bsdiixrmfindtst_irtix(pDiIxRm, isubwrd, irtStRn, irtEnRn))
  if (isdbg)
    {  BSLOG_LOG(BSLDEBUG, "irtStRn="BS_IDX_FMT", irtEnRn="BS_IDX_FMT", irtidx="BS_IDX_FMT"\n", irtStRn, irtEnRn, irtidx); }
  if (irtidx == BS_IDX_NULL)
                      { return; }
  //3.read all DWOLT records and fill result:
  bsdiixrmfindtst_mtch (pDiIxRm, isubwrd, irtidx, pFdWrds);
}

/**
 * <p>Find exactly matched word (lower case) in given dictionary and IDX.</p>
 * @param pDiIx IDX with head, opened DIC and IDX
 * @param pWrd - word to match
 * @return dic.string or NULL if not found or error
 * @set errno if error.
 **/
BsDicString*
  bsdicidxfind_exactly (BsDiIxTx *pDiIx, BsString *pWrd)
{
  int len = strlen (pWrd->val);
  if ( len < 1 )
        { return NULL; }
  BS_CHAR_T iwrd[len + 1];
  BS_DO_E_RETN (bsdicidxab_str_to_istr(pWrd->val, iwrd, pDiIx->head->ab))
  //1.find idxStartr of first up to two chars and idxEndr:
  BS_IDX_T idxStartr = BS_IDX_0;
  BS_IDX_T idxEndr = pDiIx->head->irtSz - BS_IDX_1;
  BS_DO_E_RETN (bsdicidx_find_irtrange (pDiIx->head->hirt, pDiIx->head->hirtSz, iwrd, &idxStartr, &idxEndr))
  //2.find IRT record inside idxStartr-idxEndr:
  BS_DO_E_RETN (BsDicFindIrtRd *irtrd = bsdiixfindtst_irtrd (pDiIx, iwrd, idxStartr, idxEndr))
  if ( irtrd == NULL )
  {
    free (iwrd);
    return NULL;
  }
  bool isdbg = bslog_is_debug (BS_DEBUGL_DICIDXFIND + 1);
  if (isdbg)
  {
    BSLOG_LOG(BSLDEBUG, "irtStRn="BS_IDX_FMT", irtEnRn="BS_IDX_FMT", irtidx="BS_IDX_FMT"\n", idxStartr, idxEndr, irtrd->idx);
  }
  //3.read all DWOLT records and fill result:
  BsDicString *res = bsdiixfindtst_omtch (pDiIx, pWrd, iwrd, irtrd); //without err.prop.
  bsdicfindirtrd_free (irtrd);
  return res;
}

/**
 * <p>Constructor of initialized with 0 d.string for further filling
 * from file. It validates params that was just read from IDX file.</p>
 * @param pCstr source to clone
 * @param pOfst offset
 * @return string or NULL when error
 * @set errno - ENOMEM, BSE_WRONG_PARAMS 
 **/
BsDicString *bsdicstring_new(char *pCstr, BS_FOFST_T pOfst) {
  if (pOfst < BS_FOFST_0 || pOfst < BS_FOFST_NULL) {
    errno = BSE_WRONG_PARAMS;
    BSLOG_LOG(BSLERROR, "wrong offset="BS_FOFST_FMT"\n", pOfst);
    return NULL;
  }
  BsDicString *obj = malloc(sizeof(BsDicString));
  if (obj != NULL) {
    obj->len = strlen (pCstr);
    obj->val = malloc((obj->len + 1) * sizeof(char));
    if (obj->val == NULL) {
      obj = bsdicstring_free(obj);
    } else {
      obj->offset = pOfst;
      strcpy (obj->val, pCstr);
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
 * @param pStr string, maybe NULL
 * @return always NULL
 **/
BsDicString *bsdicstring_free(BsDicString *pStr) {
  if (pStr != NULL) {
    if (pStr->val != NULL) {
      free(pStr->val);
    }
    free(pStr);
  }
  return NULL;
}
