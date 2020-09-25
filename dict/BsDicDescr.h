/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ dictionary word's description reader lib.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICDESCR
#define BS_DEBUGL_DICDESCR 31000

#include "stdio.h"

#include "BsDataSet.h"
#include "BsIntSet.h"
#include "BsStrings.h"

#define BSDICDESCR_BUF_SZ 300
#define BSDICDESCR_BUF_INC 1000

#define BSDICDESCR_TAGS_CNT 18

/* hyper-tags EBSHT_EMPTY - NULL value */
typedef enum {
  EBSHT_EMPTY, EBSHT_BOLD, EBSHT_ITALIC, EBSHT_RED, EBSHT_GRAY, EBSHT_GREEN,
  EBSHT_TAB1, EBSHT_TAB2, EBSHT_TAB3, EBSHT_TAB4, EBSHT_TAB5, EBSHT_TAB6, EBSHT_TAB7, EBSHT_TAB8, EBSHT_TAB9, EBSHT_TAB10,
  EBSHT_TOOLTIP, EBSHT_AUDIO
} EBsHypTag;

/**
 * <p>Get hype-tag name.</p>
 * @param pTag tag number
 * @return tag name or NULL without reporting
 **/
char *bshyptag_name(EBsHypTag pTag);

/**
 * <p>Hyper-tags collection type, initialized with EBSHT_EMPTY.</p>
 * @extends BSINTSET(EBsHypTag)
 **/
typedef struct {
  BSINTSET(EBsHypTag)
} BsHypTags;

/**
 * <p>Constructor.</p>
 * @param pBufSz size of collection, if BS_IDX_0 or less then error
 * @return Array or NULL when OOM or wrong size.
 * @set errno - ENOMEM, BSE_ARR_WPSIZE
 **/
BsHypTags *bshyptags_new(BS_IDX_T pBufSz);

#define BSHYPERTAGS_NEW_E_RETN(p_als,pBufSz) BsHypTags *p_als=bshyptags_new(pBufSz);\
  if (errno != 0) { BSLOG_ERR return NULL; }
#define BSHYPERTAGS_NEW_E_RET(p_als,pBufSz) BsHypTags *p_als=bshyptags_new(pBufSz);\
  if (errno != 0) { BSLOG_ERR return; }
#define BSHYPERTAGS_NEW_E_OUT(p_als,pBufSz) BsHypTags *p_als=bshyptags_new(pBufSz);\
  if (errno != 0) { BSLOG_ERR goto out; }
#define BSHYPERTAGS_NEW_E_OUTE(p_als,pBufSz) BsHypTags *p_als=bshyptags_new(pBufSz);\
  if (errno != 0) { BSLOG_ERR goto oute; }

/**
 * <p>Constructor clone.</p>
 * @param p_src source collection, not null
 * @return Array or NULL when OOM or wrong source collection.
 * @set errno - ENOMEM, BSE_WRONG_PARAMS
 **/
BsHypTags *bshyptags_clone(BsHypTags *p_src);

/**
 * <p>Destructor.</p>
 * @param pSet - collection, maybe NULL
 * @return always NULL
 **/
BsHypTags *bshyptags_free(BsHypTags *pSet);

/**
 * <p>Clear collection.</p>
 * @param pSet - collection
 **/
void bshyptags_clear(BsHypTags *pSet);

/**
* <p>Add int to the end of collection.
 * It will throw error if it's full-filled.</p>
 * @param pSet - collection
 * @param pVal - EBsHypTag
 * @return current size
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
BS_IDX_T bshyptags_add(BsHypTags *pSet, EBsHypTag pVal);

/**
 * <p>Clear last cell in collection, i.e. sets to initial value and decreases size.
 * It's tolerated if collection is empty.</p>
 * @param pSet - collection
 * @return current size
 **/
BS_IDX_T bshyptags_remove_last_tlrn(BsHypTags *pSet);

/**
 * <p>Hyper string with optional tags.</p>
 * @member str - non-NULL
 * @member tags - maybe NULL
 * @member ofst - content's offset, e.g. audio record
 * @member len - content's len, e.g. audio record
 **/
typedef struct {
  BsString *str;
  BsHypTags *tags;
  unsigned int ofst;
  unsigned int len;
} BsHypStr;

#define BSDICDESCR_TAGS_MAX_SIZE 30L

/**
 * <p>Constructor.</p>
 * @param pCstr string not NULL
 * @param p_tags_to_clone - tags to clone, maybe NULL or empty
 * @param pOfst - optional content rec. offset
 * @param pLen - optional content rec. length
 * @return object or NULL when error
 * @set errno if error.
 **/
BsHypStr*
  bshypstr_new (char *pCstr, BsHypTags *p_tags_to_clone,
                unsigned int pOfst, unsigned int pLen);

/**
 * <p>Destructor.</p>
 * @param p_hstr - pointer to hyper-string
 * @return always NULL
 **/
BsHypStr *bshypstr_free (BsHypStr *p_hstr);

/**
 * <p>Hyper-strings.</p>
 * @extends BSDATASET(BsHypStr)
 **/
typedef struct {
  BSDATASET(BsHypStr)
} BsHypStrs;

/**
 * <p>Constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsHypStrs *bshypstrs_new(BS_IDX_T pBufSz);

#define BSHYPERSTRINGS_NEW_E_OUTE(p_als, pBufSz) BsHypStrs *p_als=bshypstrs_new(pBufSz);\
  if (errno != 0) { BSLOG_ERR goto oute; }

/**
 * <p>Destructor.</p>
 * @param pSet - collection
 * @return always NULL
 **/
BsHypStrs *bshypstrs_free(BsHypStrs *pSet);

/**
 * <p>Add object to the first null cell of collection.
 * It increases collection if it's full-filled.</p>
 * @param pSet - collection
 * @param pObj - object
 * @param pInc - how much realloc when adding into
 *   full-filled collection, if BS_IDX_0 or less then error
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bshypstrs_add_inc(BsHypStrs *pSet, BsHypStr *pObj, BS_IDX_T pInc);

/**
 * <p>Read word's description with substituted DIC's tags by HTML ones
 * from dictionary with search content type#1.</p>
 * @param pDicFl - dictionary
 * @param pOfst offset of content
 * @return full description as BsHypStrs
 * @set errno if error.
 **/
typedef BsHypStrs *BsDiDscrT1_Read (FILE *pDicFl, BS_FOFST_T pOfst);

/**
 * <p>Read word's description with substituted DIC's tags by HTML ones
 * from dictionary with search content type#2.</p>
 * @param pDicFl - dictionary
 * @param pOfst offset of content
 * @param pLen length of content
 * @return full description as BsHypStrs
 * @set errno if error.
 **/
typedef BsHypStrs *BsDiDscrT2_Read (FILE *pFile, unsigned int pOfst, unsigned int pLen);
#endif
