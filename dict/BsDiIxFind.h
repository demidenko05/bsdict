/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ dictionary index finder library.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICIDXFIND
#define BS_DEBUGL_DICIDXFIND 30700

#include "BsStrings.h"
#include "BsDiIxTx.h"
#include "BsDiIxT2.h"

/**
 * <p>Index file's IRT record model with its index for finding purposes.</p>
 * @extends BSIRTRCDBASE
 * @member BS_IDX_T i2wpt_start - index (start) in I2WPT, so end = i2wpt_start + i2wpt_quantity - 1
 **/
typedef struct {
  BSIRTRCDBASE
  BS_IDX_T i2wpt_start;
  BS_IDX_T idx;
} BsDicFindIrtRd;

/**
 * <p>Constructor.</p>
 * @param pIdx - index
 * @param pIdxsubwrd - sub-word
 * @param p_max_irtwrd_size -  max IRT sub-word size
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicFindIrtRd *bsdicfindirtrd_new (BS_IDX_T pIdx, BS_CHAR_T *pIdxsubwrd, int p_max_irtwrd_size);

#define BSDICFINDIRTRD_NEW_E_RETN(p_als,pIdx,pIdxsubwrd,p_maxiwrdsz) BsDicFindIrtRd *p_als=bsdicfindirtrd_new(pIdx,pIdxsubwrd,p_maxiwrdsz);\
  if (errno != 0) { BSLOG_ERR return NULL; }

/**
 * <p>Destructor.</p>
 * @param p_irt_rcd IRT record.
 * @return always NULL
 **/
BsDicFindIrtRd *bsdicfindirtrd_free (BsDicFindIrtRd *p_irt_rcd);

//for tests:
/**
 * <p>Find matched given word IRT record index inside given range in given dictionary.</p>
 * @param pDiIxRm - dictionary and its whole index in memory
 * @param pIwrd - word(sub) to match in AB coding
 * @param pIrtStart index IRT start
 * @param pIrtEnd index IRT end
 * @return Index in IRT with matched word or BS_IDX_NULL if not found.
 * @set errno if error.
 **/
BS_IDX_T bsdiixrmfindtst_irtix (BsDiIxTxRm *pDiIxRm, BS_CHAR_T *pIwrd,
  BS_IDX_T pIrtStart, BS_IDX_T pIrtEnd);

/**
 * <p>Find all matched words in given dictionary and IDX file.</p>
 * @param pDiIx - dictionary and its whole index in memory
 * @param pIwrd - word(sub) to match in AB coding
 * @param pIrtrd first matched IRT record
 * @param pFdWrds - collection to add found record
 * @set errno if error.
 **/
void bsdiixfindtst_mtch (BsDiIxTx *pDiIx, BS_CHAR_T *pIwrd,
                            BsDicFindIrtRd *pIrtrd, BsDiFdWds *pFdWrds);

/**
 * <p>Find all matched words in given dictionary and IDX in RAM by given IRT record's index.</p>
 * @param pDiIxRm - dictionary and its whole index in memory
 * @param pIwrd - word(sub) to match in AB coding
 * @param p_irtidx IRT record index
 * @param pFdWrds - collection to add found record
 * @set errno if error.
 **/
void bsdiixrmfindtst_mtch (BsDiIxTxRm *pDiIxRm, BS_CHAR_T *pIwrd,
                            BS_IDX_T p_irtidx, BsDiFdWds *pFdWrds);

//for work:
/**
 * <p>Find all matched words in given dictionary and IDX.</p>
 * @param pDiIxRm - DIC with IDX in RAM
 * @param pFdWrds - collection to add found record
 * @param pSbwrd - sub-word to match
 * @set errno if error.
 **/
void bsdiixtxrmfind_mtch (BsDiIxTxRm *pDiIxRm, BsDiFdWds *pFdWrds, char *pSbwrd);

/**
 * <p>Find all matched words in given dictionary and IDX.</p>
 * @param pDiIx - DIC with IDX
 * @param pFdWrds - collection to add found record
 * @param pSbwrd - sub-word to match
 * @set errno if error.
 **/
void bsdiixtxfind_mtch (BsDiIxTx *pDiIx, BsDiFdWds *pFdWrds, char *pSbwrd);

//This useful only for tests purposes, because finding word's definition by clicking it in the text is also done by find matches,
//although, text words marked with <<any word>> can by seek with methods below:
/**
 * <p>Dictionary string object.</p>
 * @extends BS_STRING
 * @member BS_FOFST_T offset - origin word in dictionary file
 **/
typedef struct {
  BS_STRING
  BS_FOFST_T offset;
} BsDicString;


/**
 * <p>Constructor of initialized with 0 d.string for further filling
 * from file. It validates params that was just read from IDX file.</p>
 * @param pCstr source to clone
 * @param pOfst offset
 * @return string or NULL when error
 * @set errno - ENOMEM, BSE_WRONG_PARAMS 
 **/
BsDicString *bsdicstring_new(char *pCstr, BS_FOFST_T pOfst);

/**
 * <p>Destructor.</p>
 * @param pStr string, maybe NULL
 * @return always NULL
 **/
BsDicString *bsdicstring_free(BsDicString *pStr);

/**
 * <p>Find exactly matched word in given dictionary and IDX.</p>
 * @param pDiIx IDX with head, opened DIC and IDX
 * @param pWrd - word to match
 * @return dic.string or NULL if not found or error
 * @set errno if error.
 **/
BsDicString *bsdicidxfind_exactly(BsDiIxTx *pDiIx, BsString *pWrd);
#endif
