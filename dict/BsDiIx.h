/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ dictionary (text/audio/both...) with index  basic, dictionary file format independent library.
 * This is also supply main interface (data) to high level clients (GUI).
 * The most optimal dictionary format should by like this:
 * <pre>
 * 1. It should have binary format.
 * 2. Record's format:
 *    separated (e.g. by new line) UTF-8 synonyms ended e.g. by two new lines
 *    data descriptors:
 *      count of data descriptors
 *      text data descriptor like: type + length of text zipped or not data
 *      audio data descriptor like: type + length of audio zipped or not data
 *      video data descriptor like: type + length of video zipped or not data
 *      ...
 *    zipped or not text data
 *    zipped or not audio data
 *    zipped or not video data
 *    zipped or not ... data
 * </pre>
 * Find record data interface is presented here as:
 * 1. target word with offset in dictionary - e.g. DSL/STARDICT/.../optimal(see above) formats
 * 2. target word with offset and length in dictionary - e.g. LSA format
 * </p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DIIX
#define BS_DEBUGL_DIIX 30500

#include "stdio.h"
#include "wchar.h"

#include "BsStrings.h"
#include "BsDicFrmt.h"
#include "BsDicIdxAb.h"
#include "BsDicDescr.h"

#define BDI_IDX_FILE_EXT ".idx"

//1. Basic dic's format independent models and services:
  //1.1 IDX file structure:
/**
 * <p>HIRT index file head float-size record structure.
 * To improve searching there are schrsSz second chars with their IRT indexes.
 * </p>
 * @member BS_CHAR_T fchar - first char in word
 * @member BS_IDX_T irtIdx - record's index (start) in IRT, so end will be in the next record or end of IRT
 * @member BS_SMALL_T schrsSz - size of HIRT record's schars array, mayby 0!!!
 * @member BS_CHAR_T *schars - array of all encountered second chars, maybe NULL!!!
 * @member BS_IDX_T *irtIdxs - array of corresponding second chars's indexes in IRT, maybe NULL!
 **/
#define BSDIIXHIRTRD BS_CHAR_T fchar; BS_IDX_T irtIdx;\
  BS_SMALL_T schrsSz; BS_CHAR_T *schars; BS_IDX_T *irtIdxs;

/**
 * <p>HIRT index file head float-size record structure.
 * To improve searching there are schrsSz second chars with their IRT indexes.
 * </p>
 * @extends BSDIIXHIRTRD
 **/
typedef struct {
  BSDIIXHIRTRD
} BsDiIxHirtRd;

/**
 * <p>Constructor from file data with known size.</p>
 * @param schrsSz size of HIRT record's schars array
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHirtRd *bsdicidxhirtrd_new (int pSchrsSz);

/**
 * <p>Destructor.</p>
 * @param pRcd - HIRT index file head fixed-size record
 * @return always NULL
 **/
BsDiIxHirtRd *bsdicidxhirtrd_free (BsDiIxHirtRd *pRcd);

/**
 * <p>Data (statically allocated) to make HIRTRD from set of IRTRAW records.
 * Temporary data to maintain of all encountered second chars indexes in current
 * set of all IRTRAW records started with current fchar.</p>
 * @extends BSDIIXHIRTRD
 * @changes from BSDIIXHIRTRD:
 *   schrsSz is constant and size of all buffers (arrays) - always ab.chrsTot,
 *   so arrays are not-full-filled (contain NULLs)
 **/
typedef struct {
  BSDIIXHIRTRD
} BsDiIxHirtRdMk;

/**
 * <p>Initialize/clear data in HIRTRDMK second chars because of new first letter word encountered.</p>
 * @param pRcdmk HIRT second chars
 **/
void bsdiixhirtrdmk_init (BsDiIxHirtRdMk *pRcdmk);

#define BSDIIXHIRTRDMK_CREATE(p_als, p_bufsz) BS_CHAR_T staschars[p_bufsz];\
  BS_IDX_T stairt_idxs[p_bufsz];\
  BsDiIxHirtRdMk p_als = { .fchar=0, .irtIdx=BS_IDX_NULL, .schrsSz=p_bufsz, .schars=staschars, .irtIdxs=stairt_idxs };\
  bsdiixhirtrdmk_init(&p_als);

/**
 * <p>Make new schar in HIRT second chars</p>
 * @param pRcdmk HIRT second chars
 * @param p_schar second char
 * @param p_irt_idx word with second char IRT index
 * @set errno if error.
 **/
void bsdiixhirtrdmk_make (BsDiIxHirtRdMk *pRcdmk, BS_CHAR_T p_schar, BS_IDX_T p_irt_idx);

/**
 * <p>Create and fill HIRTRD from given BsDiIxHirtRdMk.</p>
 * @param pRcdmk HIRT record's schars array
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHirtRd* bsdicidxhirtrd_new_fill(BsDiIxHirtRdMk *pRcdmk);

#define BSDIIXHIRTRD_NEW_FILL_E_RET(p_als,pRcdmk)  BsDiIxHirtRd* p_als=bsdicidxhirtrd_new_fill(pRcdmk);\
  if (errno != 0) { BSLOG_ERR return; }

#define BSDIIXHIRTRD_NEW_FILL_E_OUTE(p_als,pRcdmk)  BsDiIxHirtRd* p_als=bsdicidxhirtrd_new_fill(pRcdmk);\
  if (errno != 0) { BSLOG_ERR goto oute; }

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
                          BS_IDX_T *pIrtStartRt, BS_IDX_T *pIrtEndRt);

/**
 * <p>Dictionary index's head basic IRT with fixed
 * record size (index file-seekable) or not (must be cached into RAM).
 * Huge dictionary should have fixed size IRT (index file-seekable).</p>
 * @member BsString nme - float size dictionary name (description) (first uchar is len)
 * @member EBsDicFrmts frmt - DSL/STARDICT/LSA...

 * @member BsDicIdxAb *ab - alphabet
 * @member BS_IDX_T irtSz IRT size
 * @member int mxIrWdSz - max IRT word size
 * @member bool isIxRm - if IRT in memory (RAM) otherwise in file, this is to satisfy client's prefs 
 * @member int hirtSz - HIRT size, if 0, then it's hieroglyph's dic, and no HIRT
 * @member BsDiIxHirtRd **hirt - head index records table HIRT, NULL for hieroglyph's dic
 **/
#define BSDIIXHEADBS BsString *nme;  EBsDicFrmts frmt; BsDicIdxAb *ab; int mxIrWdSz;\
                     bool isIxRm; BS_IDX_T irtSz; int hirtSz; BsDiIxHirtRd **hirt;

/**
 * <p>Index file's head basic.</p>
 * @extends BSDIIXHEADBS
 **/
typedef struct {
  BSDIIXHEADBS
} BsDiIxHeadBs;

/**
 * <p>Generic consructor.</p>
 * @param pObSz - object size, e.g. sizeof (BsDiIxHeadTx)
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHeadBs *bsdiixheadbs_new (size_t pObSz);

/**
 * <p>Generic destructor.</p>
 * @param pHead - pointer to IFH
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHeadBs *bsdiixheadbs_free (BsDiIxHeadBs *pHead);


/**
 * <p>Save (write/overwrite) IDX RAM's head base (in memory) into file.</p>
 * @param pDiIxRm IDX RAM.
 * @param pPth - dictionary path.
 * @return IDX file for further writing
 * @set errno if error.
 **/
FILE* bsdiixheadbs_save (BsDiIxHeadBs *pHead, char *pPth);

/**
 * <p>Load IDX head base from IDX file.</p>
 * @param pPth - dictionary path.
 * @param pHead - new head to fill
 * @param pDicFlRt - pointer to return opened dictionary
 * @param pIdxFlRt - pointer to return opened IDX file
 * @set errno if error. "IDX file not found" is not error!
 **/
void bsdiixheadbs_load (char *pPth, BsDiIxHeadBs* pHead,
                    FILE ** pDicFlRt, FILE **pIdxFlRt);

/**
 * <p>Add HIRT record.
 * It will allocate HIRT if it's NULL!</p>
 * @param pHead - pointer to IFH
 * @param pRcd HIRT record
 * @set errno if error.
 **/
void bsdiixheadbs_add_hirtrd (BsDiIxHeadBs *pHead, BsDiIxHirtRd *pRcd);

/**
 * <p>Validate HIRT.</p>
 * @param pHirt HIRT
 * @param pHSize HIRT size
 * @param pAb AB
 * @set errno if error.
 **/
void bsdicidxhirt_validate (BsDiIxHirtRd **pHirt, int pSize, BsDicIdxAb *pAb);

/**
 * <p>Base text/audio dictionary with cached IDX head.</p>
 * @param pHeadT head type
 * @member FILE *dicFl - opened dictionary
 * @member pHeadT *head
 **/
#define BSDIIXBST(pHeadT) FILE *dicFl; pHeadT *head;

/**
 * <p>Base text/audio/both/... dictionary with cached IDX head.</p>
 * @extends BSDIIXBST(BsDiIxHeadBs)
 **/
typedef struct {
  BSDIIXBST(BsDiIxHeadBs)
} BsDiIxBs;

  /*1.2 Search d.word data models and services
   * Find record data interface is presented here as:
   * 1. target word with offset in dictionary - e.g. DSL/STARDICT/.../optimal(see above) formats
   * 2. target word with offset and length in dictionary - e.g. LSA format */

/**
 * <p>Dic search data record type#1, e.g. DSL/STARDICT record data.</p>
 * @member ofst - data rec. offset
 **/
#define DSDISRDT1 BS_IDX_T ofst;

/**
 * <p>Dictionary data type#1 to search d.word's content.</p>
 * @member diIx - opened dic with head NOT NULL
 * @extends DSDISRDT1
 **/
typedef struct {
  BsDiIxBs *diIx;
  DSDISRDT1
} BsDiSrDt1;

/**
 * <p>Only constructor.</p>
 * @param pDiIx opened dic with index NOT NULL
 * @param pOfst offset of d.word content
 * @return object or NULL when error
 * @set errno - ENOMEM 
 **/
BsDiSrDt1 *bsdisrdt1_new (BsDiIxBs *pDiIx, BS_FOFST_T pOfst);

/**
 * <p>Destructor.</p>
 * @param pDicOfst maybe NULL
 * @return always NULL
 **/
BsDiSrDt1 *bsdisrdt1_free (BsDiSrDt1 *pDicOfst);

/**
 * <p>Collection of dictionaries and offsets to search data.</p>
 * @extends BSDATASET(BsDiSrDt1)
 **/
typedef struct {
  BSDATASET (BsDiSrDt1)
} BsDiSrDt1s;

/**
 * <p>Only constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiSrDt1s *bsdisrdt1s_new (BS_IDX_T pBufSz);

/**
 * <p>Destructor.</p>
 * @param pDicOfsts maybe NULL
 * @return always NULL
 **/
BsDiSrDt1s *bsdisrdt1s_free (BsDiSrDt1s *pDicOfsts);

/**
 * <p>Add object to the first null cell of collection.
 * It increases collection if it's full-filled.</p>
 * @param pDicOfstss - collection
 * @param pDiIx opened dic with index NOT NULL
 * @param pOfst offset of d.word content
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE or ENOMEM
 **/
BS_IDX_T bsdisrdt1s_add_inc(BsDiSrDt1s *pDicOfsts, BsDiIxBs *pDiIx, BS_FOFST_T pOfst);

/**
 * <p>Dic search data record type#2, e.g. LSA audio record data.</p>
 * @member ofst - data rec. offset
 * @member len - data rec. length
 **/
#define DSDISRDT2 unsigned int ofst; unsigned int len;

/**
 * <p>Dictionary data type#2 to search word's content.</p>
 * @member diIx - opened dic with head NOT NULL
 * @extends DSDISRDT2
 **/
typedef struct {
  BsDiIxBs *diIx;
  DSDISRDT2
} BsDiSrDt2;

/**
 * <p>Only constructor.</p>
 * @param pDiIx opened dic with index NOT NULL
 * @param pOfst offset of word's content
 * @param pLen length of word's content
 * @return object or NULL when error
 * @set errno - ENOMEM 
 **/
BsDiSrDt2 *bsdisrdt2_new (BsDiIxBs *pDiIx, unsigned int pOfst, unsigned int pLen);

/**
 * <p>Destructor.</p>
 * @param pDiOfLn maybe NULL
 * @return always NULL
 **/
BsDiSrDt2 *bsdisrdt2_free (BsDiSrDt2 *pDiOfLn);

/**
 * <p>Collection of dictionaries, offsets and length to search word's content.</p>
 * @extends BSDATASET(BsDiSrDt2)
 **/
typedef struct {
  BSDATASET (BsDiSrDt2)
} BsDiSrDt2s;

/**
 * <p>Only constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiSrDt2s *bsdisrdt2s_new (BS_IDX_T pBufSz);

/**
 * <p>Destructor.</p>
 * @param pDiOfLns maybe NULL
 * @return always NULL
 **/
BsDiSrDt2s *bsdisrdt2s_free (BsDiSrDt2s *pDiOfLns);

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
BS_IDX_T bsdisrdt2s_add_inc(BsDiSrDt2s *pDiOfLns, BsDiIxBs *pDiIx,
                            unsigned int pOfst, unsigned int pLen);

/**
 * <p>Dictionary's found word object with all dics and data to search.
 * This is used as current GUI searching state
 * as well as words history.</p>
 * @member wrd - dic's found word
 * @member dicOfsts - all dictionaries and offsets to search this word's content (type#1)
 * @member dicOfsts - all dictionaries, offsets and lengths to search this word's content (type#2)
 **/
typedef struct {
  BsString *wrd;
  BsDiSrDt1s *dicOfsts;  
  BsDiSrDt2s *dicOfLns;  
} BsDiFdWd;

/**
 * <p>Constructor. Non-public!</p>
 * @param pCstr origin word in dictionary file to own without cloning
 * @return object or NULL when error
 * @set errno - ENOMEM 
 **/
BsDiFdWd *bsdifdwdtst_new (char *pCstr);

/**
 * <p>Constructor cloner.
 * I.e. making history from current search content.</p>
 * @param pDicsWrd not NULL
 * @return object or NULL when error
 * @set errno - ENOMEM 
 **/
BsDiFdWd *bsdifdwd_clone (BsDiFdWd *pDicsWrd);

/**
 * <p>Destructor.</p>
 * @param pDicsWrd maybe NULL
 * @return always NULL
 **/
BsDiFdWd *bsdifdwd_free (BsDiFdWd *pDicsWrd);

/**
 * <p>Collection of dictionary's found words with their
 * dics and offsets to search content.
 * This is used as current GUI searching state
 * as well as words history.</p>
 * @extends BSDATASET(BsDiFdWd)
 **/
typedef struct {
  BSDATASET (BsDiFdWd)
} BsDiFdWds;

/**
 * <p>Only constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiFdWds *bsdifdwds_new (BS_IDX_T pBufSz);

/**
 * <p>Save found words with data to search into given file.</p>
 * @param pFdWrds not NULL
 * @param pFile not NULL
 * @set errno if error
 **/
void bsdifdwds_save (BsDiFdWds *pFdWrds, FILE *pFile);

/**
 * <p>Destructor.</p>
 * @param pFdWrds maybe NULL
 * @return always NULL
 **/
BsDiFdWds *bsdifdwds_free (BsDiFdWds *pFdWrds);

/**
 * <p>Clear all elements.</p>
 * @param pFdWrds NOT NULL
 **/
void bsdifdwds_clear (BsDiFdWds *pFdWrds);

/**
 * <p>Find member by word.</p>
 * @param pFdWrds - collection
 * @return matched member when found, NULL if word NULL or not found
 **/
BsDiFdWd *bsdifdwds_find (BsDiFdWds *pFdWrds, char *pCstr);

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
BS_IDX_T bsdifdwds_add_inc1 (BsDiFdWds *pFdWrds, char *pCstr,
  BsDiIxBs *pDiIx, BS_FOFST_T pOfst);

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
BS_IDX_T bsdifdwds_add_inc2 (BsDiFdWds *pFdWrds, char *pCstr,
  BsDiIxBs *pDiIx, unsigned int pOfst, unsigned int pLen);


//2. Interface for high level client's needs:

  //open DIC-IDX with client feedback:
typedef enum {
  EBSDS_OPENING, EBSDS_INDEXING, EBSDS_OPENED, EBSDS_ERROR, EBSDS_DISABLED, EBSDS_CANCELED
} EBsDicState;

/**
 * <p>Shared with client opening data state.</p>
 * @member EBsDicState stt - current state
 * @member int prgr - progress
 * @member stp - if client want to stop
 **/
typedef struct {
  EBsDicState stt;
  int prgr;
  bool stp;
} BsDiIxOst;

/**
 * <p>Constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxOst* bsdiixost_new ();

/**
 * <p>Destructor.</p>
 * @param pOpSt object or NULL
 * @return always NULL
 **/
BsDiIxOst* bsdiixost_free (BsDiIxOst *pOpSt);

#endif
