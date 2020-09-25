/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ dictionary content search type#2 basic,
 * dictionary file strusture independent library.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DIIXT2
#define BS_DEBUGL_DIIXT2 31500

#include "stdio.h"
#include "iconv.h"

#include "BsDiIx.h"

  //TODO 1 collection
#define BSDIAU_FIRSTCHARARRSZ 400

/**
 * <p>Content search type#2 dic's index's head for LSA like format,
 * i.e. all audio records placed together. For PDF like format
 * audio record (usually zipped) placed near to d.word.</p>
 * @extends BSDIIXHEADBS
 * @member BS_FOFST_T cntOfst - gathered records offset
 **/
typedef struct {
  BSDIIXHEADBS
  BS_FOFST_T cntOfst;
} BsDiIxHeadT2;

/**
 * <p>New head with new AB to fill from dictionary and sorted words.
 * HIRT will be created further.</p>
 * @param pTot - total records from LSA file
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHeadT2 * bsdiixheadt2_new_tf (BS_IDX_T pTot);

/**
 * <p>New head to load from IDX file.
 * AB will be created according IDX file's "chars total" property.
 * HIRT will be created further.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHeadT2* bsdiixheadt2_new_tl ();

/**
 * <p>Destructor.</p>
 * @param pHead object or NULL
 * @return always NULL
 **/
BsDiIxHeadT2* bsdiixheadt2_free (BsDiIxHeadT2* pHead);

/**
 * <p>Add HIRT record.
 * It will allocate HIRT if it's NULL!</p>
 * @param pHead - pointer to IFH
 * @param pRcd HIRT record
 * @set errno if error.
 **/
void bsdiixheadt2_add_hirtrd (BsDiIxHeadT2 *pHead, BsDiIxHirtRd *pRcd);

/**
 * <p>LSA dic with index base type.</p>
 * @extends BSDIIXBST(BsDiIxHeadT2)
 **/
typedef struct {
  BSDIIXBST(BsDiIxHeadT2)
} BsDiIxT2Bs;

/**
 * <p>LSA dic with index.</p>
 * @extends BSDIIXBST(BsDiIxHeadT2)
 * @member FILE *idxFl - opened index file
 * @member BS_FOFST_T irtOfst - IRT offset
 **/
typedef struct {
  BSDIIXBST(BsDiIxHeadT2)
  FILE *idxFl;
  BS_FOFST_T irtOfst;
} BsDiIxT2;

/**
 * <p>Constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxT2* bsdiixt2_new (BsDiIxHeadT2 *pHead, FILE* pDicFl, FILE* pIdxFl);

/**
 * <p>Destructor with closing files.</p>
 * @param pDiIx - dictionary with index or NULL
 * @return always NULL
 **/
BsDiIxT2* bsdiixt2_destroy (BsDiIxT2 *pDiIx);

/**
 * <p>Bundle to get word content.</p>
 * @extends DSDISRDT2
 * @member wrd - found d.word
 * @member diIx - DIC with IDX
 **/
typedef struct {
  DSDISRDT2
  BsString *wrd;
  BsDiIxT2Bs *diIx;
} BsDiDtT2;

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
                      unsigned int pLen);

/**
 * <p>Destructor.</p>
 * @param pRcdDt record data, maybe NULL
 * @return always NULL
 **/
BsDiDtT2* bsdidtt2_free (BsDiDtT2 *pRcdDt);

/**
 * <p>Set of bundles to get word content.</p>
 * @extends BSDATASET(BsDiDtT2)
 **/
typedef struct {
  BSDATASET (BsDiDtT2)
} BsDiDtT2s;

/**
 * <p>Constructor.</p>
 * @param pBufSz total records
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiDtT2s *bsdidtt2s_new (BS_IDX_T pBufSz);

/**
 * <p>Destructor.</p>
 * @param pSet dataset or NULL
 * @return always NULL
 **/
BsDiDtT2s *bsdidtt2s_free (BsDiDtT2s *pSet);

/**
 * <p>Clear all elements.</p>
 * @param pFdWrds NOT NULL
 **/
void bsdidtt2s_clear (BsDiDtT2s *pSet);

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
BS_IDX_T bsdidtt2s_add_inc(BsDiDtT2s *pSet, BsDiDtT2 *pObj, BS_IDX_T pInc);

/**
 * <p>IWORD/IRT record.</p>
 * @extends BSLSARDBS
 * @member iwrd - i.word float size with terminator 0
 **/
typedef struct {
  DSDISRDT2
  BS_CHAR_T *iwrd;
} BsDiIxT2Ird;

#define BSDIIXT2IRTRD_FIX_SZ(pMaxWrdSz) 2 * sizeof (unsigned int) + pMaxWrdSz * BS_CHAR_LEN

/**
 * <p>IRT table in memory, cause it holds whole words (and maybe phrase)
 * with sound data.</p>
 * @extends BSDATASET(BsDiIxT2Ird)
 **/
typedef struct {
  BSDATASET (BsDiIxT2Ird)
} BsDiIxT2Irt;

/**
 * <p>IWORD/IRT record with transient IDX.</p>
 * @extends BSLSARDBS
 * @member iwrd - i.word float size with terminator 0
 * @member idxTrns - transient index
 **/
typedef struct {
  DSDISRDT2
  BS_CHAR_T *iwrd;
  BS_IDX_T idxTrnz;
} BsDiIxT2IdxIrd;

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
                      unsigned int pLen, BS_IDX_T pIdxTrnz, int pMxIrWdSz);

/**
 * <p>Destructor.</p>
 * @param pRcd IWORD record
 * @return always NULL
 **/
BsDiIxT2IdxIrd* bsdiixt2idxird_free (BsDiIxT2IdxIrd *pRcd);

/**
 * <p>Constructor.</p>
 * @param pIwrdc i.word to copy
 * @param pOfst - content rec. offset
 * @param pLen - content rec. length
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxT2Ird *bsdiixt2ird_new (BS_CHAR_T *pIwrdc, unsigned int pOfst, unsigned int pLen);

/**
 * <p>Destructor.</p>
 * @param pRcd IWORD record
 * @return always NULL
 **/
BsDiIxT2Ird *bsdiixt2ird_free (BsDiIxT2Ird *pRcd);

/**
 * <p>Comparator.</p>
 * @param pRcd1 IWORD record1
 * @param pRcd2 IWORD record2
 * @return pRcd1 -1 less 0 equal 1 greater than pRcd2
 **/
int bsdiixt2ird_compare (BsDiIxT2Ird *pRcd1, BsDiIxT2Ird *pRcd2);

/**
 * <p>Constructor.</p>
 * @param pBufSz total records
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxT2Irt *bsdiixt2irt_new (BS_IDX_T pBufSz);

/**
 * <p>Destructor.</p>
 * @param p_iwrd_sort allocated array of records i-words to sort or NULL
 * @return always NULL
 **/
BsDiIxT2Irt *bsdiixt2irt_free (BsDiIxT2Irt *p_iwrd_sort);

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
BS_IDX_T bsdiixt2irt_add_sort_inc (BsDiIxT2Irt *pSet, BsDiIxT2Ird *pObj, BS_IDX_T pInc);

/**
 * <p>LSA dic with index in memory.</p>
 * @extends BSDIIXBST(BsDiIxHeadT2)
 * @member irt - sorted iwords with sound data in memory
 **/
typedef struct {
  BSDIIXBST(BsDiIxHeadT2)
  BsDiIxT2Irt *irt;
} BsDiIxT2Rm;

/**
 * <p>Constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxT2Rm* bsdiixt2rm_new (BsDiIxHeadT2 *pHead, FILE* pDicFl);

/**
 * <p>Destructor with closing files.</p>
 * @param pDiIxRm - dictionary with index or NULL
 * @return always NULL
 **/
BsDiIxT2Rm* bsdiixt2rm_destroy (BsDiIxT2Rm *pDiIxRm);

//Making DIC with IDX main lib:

/**
 * <p>Consumer of just read d.word sound offset and length.</p>
 * @param pDiIxRm opened rewind dictionary bundle
 * @param pDwrd - d.word
 * @param pOfst - content rec. offset
 * @param pLen - content rec. length
 * @param pInstr consumer's additional instrument
 * @return 0 to continue iteration, otherwise stop iteration
 * @set errno if error.
 **/
typedef int BsDiAuWd_Csm (BsDiIxT2Rm*, BS_WCHAR_T*, unsigned int, unsigned int, void*);

/**
 * <p>Read all words from dictionary file.</p>
 * @param pDiIxRm opened rewind dictionary bundle
 * @param pIcDs - iconv descriptor
 * @param pDiWd_Csm consumer of just read d.word, content offset and length
 * @param pInstr consumer's additional instrument
 * @return BSR_OK_END or consumer's last return value, e.g. BSR_OK_ENOUGH or errno
 * @set errno if error.
 **/
typedef int BsDiAu_Iter (BsDiIxT2Rm *pDiIxRm, iconv_t pIcDs, BsDiAuWd_Csm *pDiWd_Csm, void *pInstr);

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
int bsdiixt2_csm_ab (BsDiIxT2Rm* pDiIxRm, BS_WCHAR_T *pDwrd, unsigned int pOfst, unsigned int pLen, BS_WCHAR_T *pFstChrs);

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
int bsdiixt2_csm_irt (BsDiIxT2Rm* pDiIxRm, BS_WCHAR_T *pDwrd, unsigned int pOfst, unsigned int pLen, BsDiIxT2Irt *pIrt);

/**
 * <p>Create LSA with index bundle in memory from dictionary file.</p>
 * @param pPth - dictionary path.
 * @param pOpSt - opening state data shared with client
 * @return opened dictionary with index bundle in memory
 * @set errno if error.
 **/
typedef BsDiIxT2Rm* BsDiIxT2Rm_Create (char *pPth, BsDiIxOst* pOpSt);

/**
 * <p>Load IDX Base (head) from IDX file.</p>
 * @param pPth - dictionary path.
 * @param pHeadRt - pointer to return filled head
 * @param pDicFlRt - pointer to return opened dictionary
 * @param pIdxFlRt - pointer to return opened IDX file
 * @set errno if error. "IDX file not found" is not error!
 **/
void bsdiixheadt2_load (char *pPth, BsDiIxHeadT2** pHeadRt, FILE ** pDicFlRt, FILE **pIdxFlRt);

/**
 * <p>Save (write/overwrite) IDX RAM (in memory) into file.</p>
 * @param pDiIxRm IDX RAM.
 * @param pPth - dictionary path.
 * @set errno if error.
 **/
void bsdiixt2rm_save (BsDiIxT2Rm *pDiIxRm, char *pPth);

/**
 * <p>Load LSA with index from dictionary file.</p>
 * @param pPth - dictionary path.
 * @return opened dictionary with index bundle or NULL if index file doesn't exist
 * @set errno if error.
 **/
BsDiIxT2* bsdiixt2_load (char *pPth);

/**
 * <p>Open DIC IDX, i.e. load or create then load.</p>
 * @param pPth - dictionary path.
 * @param pOpSt - opening state data shared with client
 * @return object or NULL if error
 * @set errno if error.
 **/
typedef BsDiIxT2 *BsDiIxT2_Open (char *pPth, BsDiIxOst* pOpSt);

//Find lib:
/**
 * <p>Find IRT record matched given word inside given range in given dictionary.</p>
 * @param pDiIxRm - dictionary and its index in file
 * @param pIwrd - word(sub) to match in AB coding
 * @param pIrtStart index IRT start
 * @param pIrtEnd index IRT end
 * @return IRT record matched to word or NULL if not found.
 * @set errno if error.
 **/
BsDiIxT2IdxIrd*
  bsdiixt2tst_find_irtrd (BsDiIxT2 *pDiIx, BS_CHAR_T *pIwrd,
                         BS_IDX_T pIrtStart, BS_IDX_T pIrtEnd);

/**
 * <p>Fill mathched words with search data.</p>
 * @param pDiIxRm - dictionary and its index in file
 * @param pIwrd - word(sub) to match in AB coding
 * @param pIrtRd record with maximum matched i.word
 * @param pFdWrds - collection to add found record
 * @set errno if error.
 **/

void bsdiixt2tst_find_mtch (BsDiIxT2 *pDiIx, BS_CHAR_T *pIwrd,
                            BsDiIxT2IdxIrd *pIrtRd, BsDiFdWds *pFdWrds);

#endif
