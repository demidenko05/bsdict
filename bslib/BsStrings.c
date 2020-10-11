/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ string, char(int8) arrays library.</p>
 * @author Yury Demidenko
 **/

#include "stdlib.h"
#include "string.h"

#include "BsStrings.h"
#include "BsError.h"

/**
 * <p>Find back-slash in given word.</p>
 * @param pWrd word to find \
 * @return pointer to start back-slash string or NULL
 **/
char *bsstring_find_bslash(char *pWrd) {
  int idx = -1;
  for (int i = 0; ; i++) {
    if (pWrd[i] == 0) {
      break;
    }
    if (pWrd[i] == '\\') {
      idx = i;
      break;
    }
  }
  if (idx >=0) {
    return pWrd + idx;
  }
  return NULL;
}

/**
 * <p>Escape back-slash in given word, e.g. "/X\\\(';..;'\)/X\\"->"/X\(';..;')/X\".</p>
 * @param pWrd word to escape \
 **/
void bsstring_escape_bslash(char *pWrd) {
  char *escp = bsstring_find_bslash(pWrd);//wcschr(pWrd, '\\');
  while (escp != NULL) {
    for (int i = 0; ; i++) {
      escp[i] = escp[i + 1];
      if (escp[i] == 0) {
        break;
      }
    }
    if (escp[0] == '\\') { //keep escaped BSL:
      if (escp[1] == 0) { //the end:
        break;
      } else {
        escp = bsstring_find_bslash(escp + 1);//wcschr(escp + 1, L'\\');
      }
    } else {
      escp = bsstring_find_bslash(escp);//wcschr(escp, L'\\');
    }
  }
}

/**
 * <p>Escape leading and trailing spaces and tabs.</p>
 * @param pWrd word to escape leading and trailing spaces and tabs
 * @return modified string length
 **/
int bsstring_escape_bounds_spaces(char *pWrd) {
  //1.leading
  int i;
  while (pWrd[0] == ' ' || pWrd[0] == '\t') {
    for (i = 0; ; i++) {
      pWrd[i] = pWrd[i + 1];
      if (pWrd[i] == 0) {
        break;
      }
    }
  }
  //1.trailing
  int idxend = strlen(pWrd) - 1;
  while (idxend >= 0 && (pWrd[idxend] == ' ' || pWrd[idxend] == '\t' || pWrd[idxend] == '\r')) {
    pWrd[idxend] = 0;
    idxend--;
  }
  return idxend + 1;
}

/**
 * <p>Constructor.</p>
 * @param pCstr char array to copy, NOT-NULL
 * @return string or NULL when OOM
 * @set errno - ENOMEM
 **/
BsString *bsstring_new(char *pCstr) {
  BsString *obj = malloc(sizeof(BsString));
  if (obj != NULL) {
    int ssz = strlen(pCstr) + 1;
    obj->val = malloc(ssz * sizeof(char));
    if (obj->val == NULL) {
     obj = bsstring_free(obj);
    } else {
     obj->len = ssz - 1;
     strcpy(obj->val, pCstr);
   }
  }
  if (obj == NULL) {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Constructor cloner.</p>
 * @param pStr string to clone, NOT-NULL
 * @return string or NULL when OOM
 * @set errno - ENOMEM
 **/
BsString *bsstring_clone(BsString *pStr) { //TODO reflib
  BsString *obj = malloc(sizeof(BsString));
  if (obj != NULL) {
    int ssz = pStr->len + 1;
    obj->val = malloc(ssz * sizeof(char));
    if (obj->val == NULL) {
     obj = bsstring_free(obj);
    } else {
     obj->len = pStr->len;
     strcpy(obj->val, pStr->val);
   }
  }
  if (obj == NULL) {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Constructor of string-buffer with fixed size initialized with 0.</p>
 * @param pSize include 0 terminator
 * @return string or NULL when OOM
 * @set errno - ENOMEM
 **/
BsString*
  bsstring_newbuf (int pSize)
{
  if ( pSize <= 0 )
  {
    errno = BSE_ARR_WPSIZE;
    BSLOG_LOG (BSLERROR, "SIZE wrong "BS_IDX_FMT"", pSize);
    return NULL;
  }
  BsString *obj = malloc (sizeof(BsString));
  if ( obj != NULL )
  {
    obj->val = malloc (pSize * sizeof (char));
    if ( obj->val == NULL )
    {
     obj = bsstring_free (obj);
    } else
    {
      obj->len = pSize - 1;
      for ( int i = 0; i < pSize; i++ )
      {
        obj->val[i] = 0;
      }
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
 * @param pStr string, maybe NULL
 * @return always NULL
 **/
BsString*
  bsstring_free (BsString *pStr)
{
  if ( pStr != NULL )
  {
    if ( pStr->val != NULL )
    {
      free (pStr->val);
    }
    free (pStr);
  }
  return NULL;
}

/**
 * <p>Comparator.</p>
 * @param pCstr1 string1 NOT NULL
 * @param pCstr2 string2 NOT NULL
 * @return str1 -1 less 0 equal 1 greater than str2
 **/
int
  bsstring_compare (BsString *pCstr1, BsString *pCstr2)
{
  return strcmp(pCstr1->val,  pCstr2->val);
}

/**
 * <p>Constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsStrings *bsstrings_new(BS_IDX_T pBufSz) {
  return (BsStrings*) bsdatasettus_new(sizeof(BsStrings), pBufSz);
}

/**
 * <p>Destructor.</p>
 * @param pSet - collection
 * @return always NULL
 **/
BsStrings *bsstrings_free(BsStrings *pSet) {
  if (pSet != NULL) {
    bsdatasettus_free((BsDataSetTus*) pSet, (Bs_Destruct*) &bsstring_free);
  }
  return NULL;
}


/**
 * <p>Clear of collection, i.e. destruct all strings.</p>
 * @param pSet - collection
 **/
void
  bsstrings_clear (BsStrings *pSet)
{
  if ( pSet != NULL )
  {
    if ( pSet->vals != NULL )
    {
      for ( BS_IDX_T l = BS_IDX_0; l < pSet->bsize; l++ )
      {
        if ( pSet->vals[l] != NULL )
              { pSet->vals[l] = bsstring_free (pSet->vals[l]); }
      }
      pSet->size = 0;
    }
  }
}

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
BS_IDX_T
  bsstrings_add_inc (BsStrings *pSet, BsString *pObj, BS_IDX_T pInc)
{
  return bsdatasettus_add_inc ((BsDataSetTus*) pSet, (void*) pObj, pInc);
}

/**
 * <p>Clear and free array's element with given index and shrink array and decrease size.</p>
 * @param pSet - data set
 * @param pIdx - member index
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
void
  bsstrings_remove_shrink (BsStrings *pSet, BS_IDX_T pIdx)
{
  bsdatasettus_remove_shrink ((BsDataSetTus*) pSet, pIdx, (Bs_Destruct*) &bsstring_free);
}

/**
 * <p>Find in collection given object.</p>
 * @param pSet - collection
 * @param pObj - object
 * @return index of object if found or NULL
 **/
BS_IDX_T
  bsstrings_find (BsStrings *pSet, BsString *pObj)
{
  return bsdatasettus_find((BsDataSetTus*) pSet,
                      (void*) pObj, (Bs_Compare*) bsstring_compare);
}

/**
 * <p>Move object UP (forth) if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pSet - data set
 * @param pIdx - member index
 * @return if done
 **/
bool
  bsstrings_move_up (BsStrings *pSet, BS_IDX_T pIdx)
{
  return bsdatasettus_move_up((BsDataSetTus*) pSet, pIdx);
}

/**
 * <p>Move object DOWN (back) if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pSet - data set
 * @param pIdx - member index
 * @return if done
 **/
bool bsstrings_move_down(BsStrings *pSet, BS_IDX_T pIdx) {
  return bsdatasettus_move_down((BsDataSetTus*) pSet, pIdx);
}

/**
 * <p>Move object if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pSet - data set
 * @param pIdx1 - index1
 * @param pIdx2 - index2
 * @return if done
 **/
bool bsstrings_move(BsStrings *pSet, BS_IDX_T pIdx1, BS_IDX_T pIdx2) {
  return bsdatasettus_move((BsDataSetTus*) pSet, pIdx1, pIdx2);
}

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
 * @param pBufSz - desired buffer size, it's used if it's more than pSet->bsize
 * @return sorted index array or NULL if error
 * @set errno - BSE_WRONG_PARAMS if any parameter is NULL, if pSet->size < BS_IDX_2
 **/
BsIdxSet *bsstrings_create_idxset(BsStrings *pSet, BS_IDX_T pBufSz) {
  return bsdatasettus_create_idxset((BsDataSetTus*) pSet,
    (Bs_Compare*) &bsstring_compare, pBufSz);
}

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
void
  bsstrings_redo_idxset (BsStrings *pSet, BsIdxSet *pIdxSet, BS_IDX_T pInc)
{
  bsdatasettus_redo_idxset((BsDataSetTus*) pSet, pIdxSet,
    (Bs_Compare*) &bsstring_compare, pInc);
}

/**
 * <p>Constructor.</p>
 * @param pBufSz size of array, if BS_IDX_0 or less then error
 * @return Array or NULL when OOM or wrong size.
 * @set errno - ENOMEM, BSE_ARR_WPSIZE
 **/
BsStrBuf *bsstrbuf_new(BS_IDX_T pBufSz) {
  if (pBufSz < BS_IDX_1) {
    errno = BSE_ARR_WPSIZE;
    BSLOG_LOG(BSLERROR, "SIZE wrong "BS_IDX_FMT"", pBufSz);
    return NULL;
  }
  BsStrBuf *obj = malloc(sizeof(BsStrBuf));
  if (obj != NULL) {
    obj->vals = malloc(pBufSz * sizeof(char));
    if (obj->vals == NULL) {
     obj = bsstrbuf_free(obj);
    } else {
     obj->bsize = pBufSz;
     obj->size = BS_IDX_0;
     for (BS_IDX_T l = BS_IDX_0; l < obj->bsize; l++) {
       obj->vals[l] = 0;
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
 * @param pStrBuf - data set, maybe NULL
 * @return always NULL
 **/
BsStrBuf *bsstrbuf_free(BsStrBuf *pStrBuf) {
  if (pStrBuf != NULL) {
    if (pStrBuf->vals != NULL) {
      free(pStrBuf->vals);
    }
    free(pStrBuf);
  }
  return NULL;
}

/**
 * <p>Make copy of string with added 0 terminator.</p>
 * @param pStrBuf - pointer to string-buffer
 * @return current string with 0 terminator or NULL if empty string
 * @set errno - ENOMEM
 **/
char *bsstrbuf_tochars(BsStrBuf *pStrBuf) {
  if (pStrBuf->size == 0) {
    return NULL;
  }
  BS_DO_E_RETN(bsstrbuf_add_inc(pStrBuf, 0, BS_IDX_1))
  char *str = malloc(pStrBuf->size * sizeof(char));
  if (str == NULL) {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
    return NULL;
  }
  strcpy(str, pStrBuf->vals);
  return str;
}

/**
 * <p>Clear string-buffer.</p>
 * @param pStrBuf - pointer to string-buffer
 **/
void bsstrbuf_clear(BsStrBuf *pStrBuf) {
  for (BS_IDX_T l = BS_IDX_0; l < pStrBuf->bsize; l++) {
    pStrBuf->vals[l] = 0;
  }
  pStrBuf->size = BS_IDX_0;
}

/**
 * <p>Clear last char in string-buffer, e.g. escaped char.</p>
 * @param pStrBuf - pointer to string-buffer
 **/

void
  bsstrbuf_clear_last (BsStrBuf *pStrBuf)
{
  if ( pStrBuf->bsize > BS_IDX_0 )
  {
    pStrBuf->vals[pStrBuf->bsize - BS_IDX_1] = 0;
    pStrBuf->size--;
  }
}


/**
* <p>Increase string-buffer. This is for cases when it used as buffer, e.g. in fscanf.</p>
 * @param pStrBuf - data set
 * @param pInc - how much realloc, if BS_IDX_0 or less then error
 * @return buffer size
 * @set errno - BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsstrbuf_inc(BsStrBuf *pStrBuf, BS_IDX_T pInc) {
  if (pInc < BS_IDX_1) {
    errno = BSE_ARR_WPINCSIZE;
    BSLOG_LOG(BSLERROR, "increase wrong "BS_IDX_FMT"", pInc);
    return pStrBuf->size;
  }
  BS_IDX_T newsz = pStrBuf->bsize + pInc;
  if (newsz <= pStrBuf->bsize || newsz > BS_IDX_MAX) { //overflow
    errno = BSE_ARR_OUT_MAX_SIZE;
    BSLOG_ERR
    return pStrBuf->size;
  }
  pStrBuf->vals = realloc(pStrBuf->vals, newsz * sizeof(char));
  if (pStrBuf->vals == NULL) {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
    pStrBuf->size = BS_IDX_0;
    pStrBuf->bsize = BS_IDX_0;
    return pStrBuf->bsize;
  }
  pStrBuf->bsize += pInc;
  for (BS_IDX_T l = pStrBuf->size; l < pStrBuf->bsize; l++) {
    pStrBuf->vals[l] = 0;
  }
  return pStrBuf->bsize;
}

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
BS_IDX_T bsstrbuf_add_inc(BsStrBuf *pStrBuf, char pVal, BS_IDX_T pInc) {
  if (pInc < BS_IDX_1) {
    errno = BSE_ARR_WPINCSIZE;
    BSLOG_LOG(BSLERROR, "increase wrong "BS_IDX_FMT"", pInc);
    return pStrBuf->size;
  }
  if (pStrBuf->size == pStrBuf->bsize) {
    BS_IDX_T newsz = pStrBuf->bsize + pInc;
    if (newsz <= pStrBuf->bsize || newsz > BS_IDX_MAX) { //overflow
      errno = BSE_ARR_OUT_MAX_SIZE;
      BSLOG_ERR
      return pStrBuf->size;
    }
    pStrBuf->vals = realloc(pStrBuf->vals, newsz * sizeof(char));
    if (pStrBuf->vals == NULL) {
      if ( errno == 0 ) { errno = ENOMEM; }
      BSLOG_ERR
      pStrBuf->size = BS_IDX_0;
      pStrBuf->bsize = BS_IDX_0;
      return pStrBuf->size;
    }
    pStrBuf->bsize += pInc;
    for (BS_IDX_T l = pStrBuf->size; l < pStrBuf->bsize; l++) {
      pStrBuf->vals[l] = 0;
    }
  }
  pStrBuf->vals[pStrBuf->size] = pVal;
  pStrBuf->size++;
  return pStrBuf->size;
}

/**
 * <p>Get char from string-buffer.</p>
 * @param pStrBuf - data set
 * @param pIdx - member index
 * @return char
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
char
  bsstrbuf_get (BsStrBuf *pStrBuf, BS_IDX_T pIdx)
{
  if ( pIdx < BS_IDX_0 || pIdx >= pStrBuf->size )
  {
    errno = BSE_ARR_OUT_OF_BOUNDS;
    BSLOG_ERR
    return 0;
  }
  return pStrBuf->vals[pIdx];
}

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
BS_IDX_T bsstrbuf_add_str_inc(BsStrBuf *pStrBuf, char* pCstr, BS_IDX_T pInc) {
  if (pInc < BS_IDX_1) {
    errno = BSE_ARR_WPINCSIZE;
    BSLOG_LOG(BSLERROR, "increase wrong "BS_IDX_FMT"", pInc);
    return pStrBuf->size;
  }
  BS_IDX_T slen = strlen(pCstr);
  if (pStrBuf->size + slen >= pStrBuf->bsize) {
    BS_IDX_T newsz;
    if (slen > pInc) {
      newsz = pStrBuf->bsize + slen;
    } else {
      newsz = pStrBuf->bsize + pInc;
    }
    if (newsz <= pStrBuf->bsize || newsz > BS_IDX_MAX) { //overflow
      errno = BSE_ARR_OUT_MAX_SIZE;
      BSLOG_ERR
      return pStrBuf->size;
    }
    pStrBuf->bsize = newsz;
    pStrBuf->vals = realloc(pStrBuf->vals, pStrBuf->bsize * sizeof(char));
    if (pStrBuf->vals == NULL) {
      if ( errno == 0 ) { errno = ENOMEM; }
      BSLOG_ERR
      pStrBuf->bsize = BS_IDX_0;
      pStrBuf->size = BS_IDX_0;
      return pStrBuf->size;
    }
    for (BS_IDX_T l = pStrBuf->size; l < pStrBuf->bsize; l++) {
      pStrBuf->vals[l] = 0;
    }
  }
  for (int i = 0; ; i++) {
    if (pCstr[i] == 0) {
      break;
    }
    pStrBuf->vals[pStrBuf->size] = pCstr[i];
    pStrBuf->size++;
  }
  return pStrBuf->size;
}


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
BS_IDX_T
  bsstrbuf_ins_str_inc (BsStrBuf *pStrBuf, char* pCstr, BS_IDX_T pIdx, BS_IDX_T pInc)
{
  if (pInc < BS_IDX_1) {
    errno = BSE_ARR_WPINCSIZE;
    BSLOG_LOG(BSLERROR, "increase wrong "BS_IDX_FMT"", pInc);
    return pStrBuf->size;
  }
  BS_IDX_T slen = strlen(pCstr);
  if (pStrBuf->size + slen >= pStrBuf->bsize) {
    BS_IDX_T newsz;
    if (slen > pInc) {
      newsz = pStrBuf->bsize + slen;
    } else {
      newsz = pStrBuf->bsize + pInc;
    }
    if (newsz <= pStrBuf->bsize || newsz > BS_IDX_MAX) { //overflow
      errno = BSE_ARR_OUT_MAX_SIZE;
      BSLOG_ERR
      return pStrBuf->size;
    }
    pStrBuf->bsize = newsz;
    pStrBuf->vals = realloc(pStrBuf->vals, pStrBuf->bsize * sizeof(char));
    if (pStrBuf->vals == NULL) {
      if ( errno == 0 ) { errno = ENOMEM; }
      BSLOG_ERR
      pStrBuf->bsize = BS_IDX_0;
      pStrBuf->size = BS_IDX_0;
      return pStrBuf->size;
    }
    for (BS_IDX_T l = pStrBuf->size; l < pStrBuf->bsize; l++) {
      pStrBuf->vals[l] = 0;
    }
  }
  int i;
  for (i = pStrBuf->size - 1; i >= pIdx; i--) {
    pStrBuf->vals[i + slen] = pStrBuf->vals[i];
  }
  for (i = 0; ; i++) {
    if (pCstr[i] == 0) {
      break;
    }
    pStrBuf->vals[pIdx + i] = pCstr[i];
    pStrBuf->size++;
  }
  return pStrBuf->size;
}

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
BS_IDX_T
  bsstrbuf_add_inc_tus (BsStrBuf *pStrBuf, void* pSrc, BS_IDX_T pCnt, BS_IDX_T pInc)
{
  if (pInc < BS_IDX_1) {
    errno = BSE_ARR_WPINCSIZE;
    BSLOG_LOG(BSLERROR, "increase wrong "BS_IDX_FMT"", pInc);
    return pStrBuf->size;
  }
  if (pStrBuf->size + pCnt >= pStrBuf->bsize) {
    BS_IDX_T newsz;
    if (pCnt > pInc) {
      newsz = pStrBuf->bsize + pCnt;
    } else {
      newsz = pStrBuf->bsize + pInc;
    }
    if (newsz <= pStrBuf->bsize || newsz > BS_IDX_MAX) { //overflow
      errno = BSE_ARR_OUT_MAX_SIZE;
      BSLOG_ERR
      return pStrBuf->size;
    }
    pStrBuf->bsize = newsz;
    pStrBuf->vals = realloc(pStrBuf->vals, pStrBuf->bsize * sizeof(char));
    if (pStrBuf->vals == NULL) {
      if ( errno == 0 ) { errno = ENOMEM; }
      BSLOG_ERR
      pStrBuf->bsize = BS_IDX_0;
      pStrBuf->size = BS_IDX_0;
      return pStrBuf->size;
    }
    for (BS_IDX_T l = pStrBuf->size; l < pStrBuf->bsize; l++) {
      pStrBuf->vals[l] = 0;
    }
  }
  memcpy (pStrBuf->vals + pStrBuf->size, pSrc, pCnt);
  pStrBuf->size += pCnt;
  return pStrBuf->size;
}

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
BS_IDX_T
  bsstrbuf_add_uchars_inc (BsStrBuf *pStrBuf, unsigned char* pUchars, int pCnt, BS_IDX_T pInc)
{
  if (pInc < BS_IDX_1) {
    errno = BSE_ARR_WPINCSIZE;
    BSLOG_LOG(BSLERROR, "increase wrong "BS_IDX_FMT"", pInc);
    return pStrBuf->size;
  }
  if (pStrBuf->size + pCnt >= pStrBuf->bsize) {
    BS_IDX_T newsz;
    if (pCnt > pInc) {
      newsz = pStrBuf->bsize + pCnt;
    } else {
      newsz = pStrBuf->bsize + pInc;
    }
    if (newsz <= pStrBuf->bsize || newsz > BS_IDX_MAX) { //overflow
      errno = BSE_ARR_OUT_MAX_SIZE;
      BSLOG_ERR
      return pStrBuf->size;
    }
    pStrBuf->bsize = newsz;
    pStrBuf->vals = realloc(pStrBuf->vals, pStrBuf->bsize * sizeof(char));
    if (pStrBuf->vals == NULL) {
      if ( errno == 0 ) { errno = ENOMEM; }
      BSLOG_ERR
      pStrBuf->bsize = BS_IDX_0;
      pStrBuf->size = BS_IDX_0;
      return pStrBuf->size;
    }
    for (BS_IDX_T l = pStrBuf->size; l < pStrBuf->bsize; l++) {
      pStrBuf->vals[l] = 0;
    }
  }
  for (int i = 0; i < pCnt; i++) {
    pStrBuf->vals[pStrBuf->size] = (char) pUchars[i];
    pStrBuf->size++;
  }
  return pStrBuf->size;
}


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
BS_IDX_T
  bsstrbuf_add_chars_inc (BsStrBuf *pStrBuf, char* pChars, int pCnt, BS_IDX_T pInc)
{
  if (pInc < BS_IDX_1) {
    errno = BSE_ARR_WPINCSIZE;
    BSLOG_LOG(BSLERROR, "increase wrong "BS_IDX_FMT"", pInc);
    return pStrBuf->size;
  }
  if (pStrBuf->size + pCnt >= pStrBuf->bsize) {
    BS_IDX_T newsz;
    if (pCnt > pInc) {
      newsz = pStrBuf->bsize + pCnt;
    } else {
      newsz = pStrBuf->bsize + pInc;
    }
    if (newsz <= pStrBuf->bsize || newsz > BS_IDX_MAX) { //overflow
      errno = BSE_ARR_OUT_MAX_SIZE;
      BSLOG_ERR
      return pStrBuf->size;
    }
    pStrBuf->bsize = newsz;
    pStrBuf->vals = realloc(pStrBuf->vals, pStrBuf->bsize * sizeof(char));
    if (pStrBuf->vals == NULL) {
      if ( errno == 0 ) { errno = ENOMEM; }
      BSLOG_ERR
      pStrBuf->bsize = BS_IDX_0;
      pStrBuf->size = BS_IDX_0;
      return pStrBuf->size;
    }
    for (BS_IDX_T l = pStrBuf->size; l < pStrBuf->bsize; l++) {
      pStrBuf->vals[l] = 0;
    }
  }
  for (int i = 0; i < pCnt; i++) {
    pStrBuf->vals[pStrBuf->size] = pChars[i];
    pStrBuf->size++;
  }
  return pStrBuf->size;
}
