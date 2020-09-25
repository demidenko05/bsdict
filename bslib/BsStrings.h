/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ string, char(int8) arrays library.</p>
 * @author Yury Demidenko
 **/
#ifndef BSSTRINGS_H
#define BSSTRINGS_H 1

#include "BsDataSet.h"

/**
 * <p>Find back-slash in given word.</p>
 * @param pWrd word to find \
 * @return pointer to start back-slash string or NULL
 **/
char *bsstring_find_bslash(char *pWrd);

/**
 * <p>Escape back-slash in given word, e.g. "/X\\\(';..;'\)/X\\"->"/X\(';..;')/X\".</p>
 * @param pWrd word to escape \
 **/
void bsstring_escape_bslash(char *pWrd);

/**
 * <p>Escape leading and trailing spaces and tabs.</p>
 * @param pWrd word to escape leading and trailing spaces and tabs
 * @return modified string length
 **/
int bsstring_escape_bounds_spaces(char *pWrd);

/**
 * <p>String object. This is more optimal than using strlen.
 * This is the best alternative to raw char array.</p>
 * @member int len - length
 * @member char *val - value
 **/
#define BS_STRING int len; char *val;

/**
 * <p>String object. This is more optimal than using strlen.
 * This is the best alternative to raw char array.</p>
 * @extends BS_STRING
 **/
typedef struct {
  BS_STRING
} BsString;

/**
 * <p>Constructor.</p>
 * @param pCstr char array to copy, NOT-NULL
 * @return string or NULL when OOM
 * @set errno - ENOMEM
 **/
BsString *bsstring_new(char *pCstr);

/**
 * <p>Constructor cloner.</p>
 * @param pStr string to clone, NOT-NULL
 * @return string or NULL when OOM
 * @set errno - ENOMEM
 **/
BsString *bsstring_clone(BsString *pStr);

/**
 * <p>Constructor of string-buffer with fixed size initialized with 0.</p>
 * @param pSize include 0 terminator
 * @return string or NULL when OOM
 * @set errno - ENOMEM
 **/
BsString *bsstring_newbuf(int pSize);

/**
 * <p>Destructor.</p>
 * @param pStr string, maybe NULL
 * @return always NULL
 **/
BsString *bsstring_free(BsString *pStr);

/**
 * <p>Comparator.</p>
 * @param pCstr1 string1 NOT NULL
 * @param pCstr2 string2 NOT NULL
 * @return str1 -1 less 0 equal 1 greater than str2
 **/
int bsstring_compare(BsString *pCstr1, BsString *pCstr2);

/**
 * <p>Strings collection. It's used for BS dict's history.</p>
 * @extends BSDATASET(BsString)
 **/
typedef struct {
  BSDATASET(BsString)
} BsStrings;

/**
 * <p>Constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsStrings *bsstrings_new(BS_IDX_T pBufSz);

/**
 * <p>Destructor.</p>
 * @param pSet - collection
 * @return always NULL
 **/
BsStrings *bsstrings_free(BsStrings *pSet);

/**
 * <p>Clear of collection, i.e. destruct all strings.</p>
 * @param pSet - collection
 **/
void bsstrings_clear(BsStrings *pSet);

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
BS_IDX_T bsstrings_add_inc(BsStrings *pSet, BsString *pObj, BS_IDX_T pInc);

/**
 * <p>Clear (set to NULL) array's cell with given index and shrink array and decrease size.</p>
 * @param pSet - data set
 * @param pIdx - member index
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
void bsstrings_remove_shrink(BsStrings *pSet, BS_IDX_T pIdx);

/**
 * <p>Find in collection given object.</p>
 * @param pSet - collection
 * @param pObj - object
 * @return index of object if found or NULL
 **/
BS_IDX_T bsstrings_find(BsStrings *pSet, BsString *pObj);

/**
 * <p>Move object UP (forth) if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pSet - data set
 * @param pIdx - member index
 * @return if done
 **/
bool bsstrings_move_up(BsStrings *pSet, BS_IDX_T pIdx);

/**
 * <p>Move object DOWN (back) if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pSet - data set
 * @param pIdx - member index
 * @return if done
 **/
bool bsstrings_move_down(BsStrings *pSet, BS_IDX_T pIdx);

bool bsstrings_move(BsStrings *pSet, BS_IDX_T pIdx1, BS_IDX_T pIdx2);

/**
 * <p>Make sorted indexes array for given collection.
 * <pre>
 * E.g. array of strings:
 * 0-"week" 1-"beep" 2-"speak",
 * sorted indexes array will be:
 * 1 2 0
 * </pre>
 * This is for N-ary sorted array, e.g. array is sorted historically plus manually,
 * and sorted indexes array with alphabetical order.
 * Or for fast switching between ASC and DESC.
 * </p>
 * @param pSet - data set
 * @return sorted index array or NULL if error
 * @param pBufSz - desired buffer size, it's used if it's more than pSet->bsize
 * @return sorted index array or NULL if error
 * @set errno - BSE_WRONG_PARAMS if any parameter is NULL, if pSet->size < BS_IDX_2
 **/
BsIdxSet *bsstrings_create_idxset(BsStrings *pSet, BS_IDX_T pBufSz);

/**
 * <p>Redo sorted indexes array for given collection.
 * It increases sorted indexes array if need.
 * </p>
 * @param pSet - data set NON-NULL
 * @param pIdxSet sorted index array to redo NON-NULL
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @set errno - ENOMEM, BSE_WRONG_PARAMS if any parameter is NULL or pInc wrong
 **/
void bsstrings_redo_idxset(BsStrings *pSet, BsIdxSet *pIdxSet, BS_IDX_T pInc);

/**
 * <p>String-buffer with float size.</p>
 * @member BS_IDX_T bsize - buffer size more than 0
 * @member BS_IDX_T size - size from 0
 * @member char *vals - array of chars
 **/
typedef struct {
  BS_IDX_T bsize;
  BS_IDX_T size;
  char *vals;
} BsStrBuf;

/**
 * <p>Constructor.</p>
 * @param pBufSz size of array, if BS_IDX_0 or less then error
 * @return Array or NULL when OOM or wrong size.
 * @set errno - ENOMEM, BSE_ARR_WPSIZE
 **/
BsStrBuf *bsstrbuf_new(BS_IDX_T pBufSz);

#define BSSTRINGBUFFER_NEW_E_RETN(p_als, pBufSz) BsStrBuf *p_als=bsstrbuf_new(pBufSz);\
  if (errno != 0) { BSLOG_ERR return NULL; }
#define BSSTRINGBUFFER_NEW_E_RET(p_als, pBufSz) BsStrBuf *p_als=bsstrbuf_new(pBufSz);\
  if (errno != 0) { BSLOG_ERR return; }
#define BSSTRINGBUFFER_NEW_E_OUT(p_als, pBufSz) BsStrBuf *p_als=bsstrbuf_new(pBufSz);\
  if (errno != 0) { BSLOG_ERR goto out; }
#define BSSTRINGBUFFER_NEW_E_OUTE(p_als, pBufSz) BsStrBuf *p_als=bsstrbuf_new(pBufSz);\
  if (errno != 0) { BSLOG_ERR goto oute; }

/**
 * <p>Destructor.</p>
 * @param pStrBuf - data set, maybe NULL
 * @return always NULL
 **/
BsStrBuf *bsstrbuf_free(BsStrBuf *pStrBuf);

/**
 * <p>Make copy of string with added 0 terminator.</p>
 * @param pStrBuf - pointer to string-buffer
 * @return current string with 0 terminator or NULL if empty string
 * @set errno - ENOMEM
 **/
char *bsstrbuf_tochars(BsStrBuf *pStrBuf);

/**
 * <p>Clear string-buffer.</p>
 * @param pStrBuf - pointer to string-buffer
 **/
void bsstrbuf_clear(BsStrBuf *pStrBuf);

/**
 * <p>Clear last char in string-buffer, e.g. escaped char.</p>
 * @param pStrBuf - pointer to string-buffer
 **/
void bsstrbuf_clear_last(BsStrBuf *pStrBuf);

/**
* <p>Increase string-buffer. This is for cases when it used as buffer, e.g. in fscanf.</p>
 * @param pStrBuf - data set
 * @param pInc - how much realloc, if BS_IDX_0 or less then error
 * @return buffer size
 * @set errno - BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsstrbuf_inc(BsStrBuf *pStrBuf, BS_IDX_T pInc);

/**
 * <p>Add char to the end of string-buffer.
 * It increases array if it's full-filled.</p>
 * @param pStrBuf - data set
 * @param pVal - char
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @return current size
 * @set errno - BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsstrbuf_add_inc(BsStrBuf *pStrBuf, char pVal, BS_IDX_T pInc);

/**
 * <p>Get char from string-buffer.</p>
 * @param pStrBuf - data set
 * @param pIdx - member index
 * @return char
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
char bsstrbuf_get(BsStrBuf *pStrBuf, BS_IDX_T pIdx);

/**
 * <p>Add string without 0 terminator to the end of string-buffer.
 * It increases array if it's full-filled.</p>
 * @param pStrBuf - data set
 * @param pCstr - string
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @return current size
 * @set errno - BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsstrbuf_add_str_inc(BsStrBuf *pStrBuf, char* pCstr, BS_IDX_T pInc);

/**
 * <p>Insert string into given position of string-buffer.
 * It increases array if it's full-filled.</p>
 * @param pStrBuf - data set
 * @param pCstr - string
 * @param pIdx - position to insert
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @return current size
 * @set errno - BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsstrbuf_ins_str_inc(BsStrBuf *pStrBuf, char* pCstr, BS_IDX_T pIdx, BS_IDX_T pInc);

/**
 * <p>Add raw bytes from generic source without 0 terminator to the end of string-buffer.
 * It increases array if it's full-filled.</p>
 * @param pStrBuf - data set
 * @param pSrc - generic source
 * @param pCnt - count of bytes
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @return current size
 * @set errno - BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsstrbuf_add_inc_tus(BsStrBuf *pStrBuf, void* pSrc, BS_IDX_T pCnt, BS_IDX_T pInc);

/**
 * <p>Add uchars without 0 terminator to the end of string-buffer.
 * It increases array if it's full-filled.
 * This is used for OGG file in memory.</p>
 * @param pStrBuf - data set
 * @param pUchars - uchars array
 * @param pCnt - count to add
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @return current size
 * @set errno - BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsstrbuf_add_uchars_inc(BsStrBuf *pStrBuf, unsigned char* pUchars, int pCnt, BS_IDX_T pInc);

/**
 * <p>Add chars without 0 terminator to the end of string-buffer.
 * It increases array if it's full-filled.
 * This is used for OGG file in memory.</p>
 * @param pStrBuf - data set
 * @param pChars - chars array
 * @param pCnt - count to add
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @return current size
 * @set errno - BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsstrbuf_add_chars_inc(BsStrBuf *pStrBuf, char* pChars, int pCnt, BS_IDX_T pInc);
#endif
