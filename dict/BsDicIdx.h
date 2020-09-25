/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ dictionary index base library.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICIDX
#define BS_DEBUGL_DICIDX 20800

#include "wchar.h"
#include "stdbool.h"

#include "BsBase.h"

/* Matching when create index */
typedef enum {
  EWMATCHNONE, EWMATCHFROMSTART, EWMATCHAFTERSTART
} EBsWrdMatch;

//TODO BsIString type with length member
/**
 * <p>Compute di.string length.</p>
 * @param p_istr string in index alphabet
 * @return di.string length
 **/
int bsdicidx_istr_len(BS_CHAR_T *p_istr);

/**
 * <p>Copy index word from source into destination.</p>
 * @param p_istr_dest destination string in index alphabet
 * @param p_istr_src source string in index alphabet
 **/
void bsdicidx_istr_cpy(BS_CHAR_T *p_istr_dest, BS_CHAR_T *p_istr_src);

/**
 * <p>Compare two given strings.
 * e.g. bsdicidx_istr_cmp("abc", "bbc") will return -1.</p>
 * @param p_istr1 str1
 * @param p_istr2 str2
 * @return str1 -1 less 0 equal 1 greater than str2
 **/
int bsdicidx_istr_cmp(BS_CHAR_T *p_istr1, BS_CHAR_T *p_istr2);

/**
 * <p>Compare two given strings.
 * e.g. bsdicidx_wstr_cmp("abc", "bbc") will return -1.</p>
 * @param pCstr1 str1
 * @param pCstr2 str2
 * @return str1 -1 less 0 equal 1 greater than str2
 **/
int bsdicidx_wstr_cmp(BS_WCHAR_T *pCstr1, BS_WCHAR_T *pCstr2);

/**
 * <p>
 * For creating IDX.
 * Check if isubstr matches to istr, e.g. bsdicidx_istr_match("abc", "bbc abcd") will return EWMATCHAFTERSTART
 * </p>
 * @param p_isubstr i.sub.string
 * @param p_istr i.string
 * @param p_ispace - space in AB-coding
 * @return matched result
 **/
EBsWrdMatch bsdicidx_istr_match(BS_CHAR_T *p_isubstr, BS_CHAR_T *p_istr,
  int p_ispace);


/**
 * <p>
 * For creating IDX.
 * Check if isubstr matches to istr from start.
 * bsdicidx_istr_match_from_start("abc", "abcd bbcd") will return EWMATCHFROMSTART.</p>
 * @param p_isubstr i.sub.string
 * @param p_istr i.string
 * @return matched result
 **/
EBsWrdMatch bsdicidx_istr_match_from_start(BS_CHAR_T *p_isubstr, BS_CHAR_T *p_istr);

/**
 * <p>For finding in IDX. This means that "sen" matches to "send" with result 3!!!
 * But "sen" also matches to "s" with result 1.
 * IRT index word usually length less then indexing source one, e.g.
 * tst_dic1.dsl "ящур" has only index word "я".
 * </p>
 * @param p_iwrd str1 user's word
 * @param p_irtwrd str2 IRT word
 * @return comparing result - > 0 = number of matched chars, 0 str1 > str2, -1 str1 < str2
 **/
int bsdicidx_istr_how_match (BS_CHAR_T *p_iwrd, BS_CHAR_T *p_irtwrd);

/**
 * <p>For making IDX. This means that "sen" equals to "send"!!!
 * But "sen" is greater than "s".</p>
 * @param p_irtwrd str1 IRT word
 * @param p_iwrd str2 dictionary word
 * @return str1 -1 less 0 equal 1 greater than str2
 **/
int bsdicidx_istr_cmp_match(BS_CHAR_T *p_irtwrd, BS_CHAR_T *p_iwrd);

/**
 * <p>For searching words. "Common Sense" contains "sen".</p>
 * @param pIwrd1 found matched word
 * @param pIwrd2 searched word
 * @return true if pIwrd1 contains of pIwrd2
 **/
bool bsdicidx_istr_cont(BS_CHAR_T *pIwrd1, BS_CHAR_T *pIwrd2);
#endif
