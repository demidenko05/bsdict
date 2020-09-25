/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ dictionary index wchar based alphabet library.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICIDXAB
#define BS_DEBUGL_DICIDXAB 20900

#include "stdio.h"
#include "string.h"

#include "BsDicIdx.h"
#include "BsDicWord.h"

#define BDI_AB_BUF_INITSIZE 60 //40 low letters + BDI_AB_FIRST_CHARS_COUNT is enough for standard dictionary
#define BDI_AB_BUF_INCREASE 300 //it's usually a multi-lang dic

#define BDI_AB_SIZE(p_pidx_ab) sizeof(int) + BS_WCHAR_LEN*p_pidx_ab->chrsTot

/**
 * <p>Index file head alphabet structure.
 * Index's alphabet consists of lower case letters and other chars.
 * Because of a dictionary can use any char, e.g. word "a^_^".
 * </p>
 * */
typedef struct {
  int transient_buffer_len; //buffers wchars size, non-persistent var for memory allocation
  int chrsTot; //total used lower case chars.
  //chrsTot ordered wchars array
  //corresponding to wchar char will be its index plus 1:
  BS_WCHAR_T *wchars;
  int ispace; //space in AB-coding, initial 0
} BsDicIdxAb;

//Public methods:

/**
 * <p>Create new alphabet, allocate wchars, add space.</p>
 * @param p_buffer_sz - real or predicted AB size
 * @return index alphabet or NULL
 * @set errno if error.
 **/
BsDicIdxAb *bsdicidxab_new(int p_buffer_sz);

/**
 * <p>
 * Destroy alphabet.
 * </p>
 * @param pIdx_ab pointer to index alphabet
 * @return always NULL
 **/
BsDicIdxAb *bsdicidxab_free(BsDicIdxAb *pIdx_ab);

/* Temporary result holder, no need to dynamic allocate */
typedef struct {
  int max_iword_len; //pointer to return max i.word (in idx.AB) length
  BS_IDX_T dwoltSz; //total words in dictionary including duplicates
  BS_IDX_T i2wptSz; //total 2-nd, 3d ... sub-words in dictionary including duplicates
} BsDicIdxAbTotals;

#define BSDICIDXABTOTALS_CREATE(p_alias) BsDicIdxAbTotals p_alias = { .max_iword_len=0, .dwoltSz=BS_IDX_0, .i2wptSz=BS_IDX_0 };

/* Initialize AB from just read d.word instrument */
typedef struct {
  BsDicIdxAb *idx_ab; //pointer to index alphabet
  BsDicIdxAbTotals *totals;
  BS_CHAR_T *idxstr; //buffer for i.word
} BsDicIdxAbFill;

/**
 * <p>Type-safe consumer. Fill AB by all words from dictionary.
 * @param p_dword just read d.word
 * @param p_csm_inst instrument to consume
 * @return 0 to continue iteration, otherwise stop iteration
 * @set errno if error.
 * </p>
 **/
int bsdicidxab_dwrd_consume_fill(BsDicWord *p_dword, BsDicIdxAbFill *p_csm_inst);

/**
 * <p>
 * Type-safe iterator trough whole DSL/STARDIC... dictionary, just read dic.word will be consumed
 * by given consumer, if consumer returns an error, then iteration will stop.
 * </p>
 * @param pDicFl - opened unwound dictionary file
 * @param p_dic_entry_buffer_size - client expected/predicted this max entry size in dictionary
 * @param pIdx_ab pointer to index alphabet
 * @param p_totals pointer to return totals
 * @return BSR_OK_END or consumer's last return value, e.g. BSR_OK_ENOUGH or errno
 * @set errno if error.
 **/
typedef int BsDicIdxAb_Iter_Dwrd_Fill(FILE*, int, BsDicIdxAb*, BsDicIdxAbTotals*);

/**
 * <p>
 * Type-safe iterator trough whole DSL dictionary, just read dic.word will be consumed
 * by given consumer, if consumer returns an error, then iteration will stop.
 * </p>
 * @param pDicFl - opened unwound dictionary file
 * @param p_dic_entry_buffer_size - client expected/predicted this max entry size in dictionary
 * @param pIdx_ab pointer to index alphabet
 * @param p_totals pointer to return totals
 * @return BSR_OK_END or consumer's last return value, e.g. BSR_OK_ENOUGH or errno
 * @set errno if error.
 **/
int bsdicidxab_iter_dsl_fill(FILE *pDicFl, int p_dic_entry_buffer_size, BsDicIdxAb *pIdx_ab,
  BsDicIdxAbTotals *p_totals); //TODO 1 extract into AB-DSL lib

/**
 * <p>
 * Fills alphabet with wide char string.
 * Wide chars will be converted to lower case, to spaces=hyphen=space.
 * Index's alphabet consists of lower case chars plus space and "'".
 * All other chars, e.g. digits, comma... will be rejected.
 * "its" and "it's" are two different indexes.
 * Index word never start with "'" or " ", e.g. "'twere".
 * Adding wchar will be in ordered way.
 * @param p_wstr w.string
 * @param pIdx_ab index alphabet to make
 * @set errno if error.
 * </p>
 **/
void bsdicidxab_add_wstr(BS_WCHAR_T *p_wstr, BsDicIdxAb *pIdx_ab);

/**
 * <p>
 * Converts wide chars string into index's chars.
 * Wide chars will be converted to lower case, 3spaces=2spaces=1space.
 * @param p_wstr w.string
 * @param p_istr result full string in index alphabet for further indexing
 * @param pIdx_ab index alphabet
 * </p>
 **/
void bsdicidxab_wstr_to_istr(BS_WCHAR_T *p_wstr, BS_CHAR_T *p_istr, BsDicIdxAb *pIdx_ab);

/**
 * <p>
 * Converts index's chars string into wide chars.
 * It's for testing bsdicidxab_wstr_to_istr
 * @param p_istr string in index alphabet
 * @param p_wstr w.string result
 * @param pIdx_ab index alphabet
 * </p>
 **/
void bsdicidxab_istr_to_wstr(BS_CHAR_T *p_istr, BS_WCHAR_T *p_wstr, BsDicIdxAb *pIdx_ab);

/**
 * <p>
 * Converts index's chars string up to given chars count into wide chars.
 * It's for testing bsdicidxab_wstr_to_istr
 * @param p_istr string in index alphabet
 * @param p_wstr w.string result
 * @param pCnt maximum chars to convert
 * @param pIdx_ab index alphabet
 * </p>
 **/
void bsdicidxab_istrn_to_wstr(BS_CHAR_T *p_istr, BS_WCHAR_T *p_wstr, int pCnt, BsDicIdxAb *pIdx_ab);

/**
 * <p>Сonvert user's word to AB coding, it maybe empty string.</p>
 * @param pWrd - word from
 * @param pIwrd - i.word to
 * @param pAb - alphabet
 * @set errno if error.
 **/
void bsdicidxab_str_to_istr (char *pWrd, BS_CHAR_T *pIwrd, BsDicIdxAb *pAb);
#endif
