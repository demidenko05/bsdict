/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ int(32)/enum collection library.</p>
 * @author Yury Demidenko
 **/

#include "stdlib.h"

#include "BsError.h"
#include "BsIntSet.h"

/**
 * <p>Constructor.</p>
 * @param pBufSz size of collection, if BS_IDX_0 or less then error
 * @param pIniVal - initialize collection with this value
 * @return Array or NULL when OOM or wrong size.
 * @set errno - ENOMEM, BSE_ARR_WPSIZE
 **/
BsIntSet *bsintset_new(BS_IDX_T pBufSz, int pIniVal) {
  if (pBufSz < BS_IDX_1) {
    errno = BSE_ARR_WPSIZE;
    BSLOG_LOG(BSLERROR, "SIZE wrong "BS_IDX_FMT"", pBufSz);
    return NULL;
  }
  BsIntSet *obj = malloc(sizeof(BsIntSet));
  if (obj != NULL) {
    obj->vals = malloc(pBufSz * sizeof(int));
    if (obj->vals == NULL) {
     obj = bsintset_free(obj);
   } else {
     obj->bsize = pBufSz;
     obj->size = BS_IDX_0;
     for (BS_IDX_T l = BS_IDX_0; l < obj->bsize; l++) {
       obj->vals[l] = pIniVal;
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
 * <p>Constructor clone.</p>
 * @param p_src source collection, not null
 * @return Array or NULL when OOM or wrong source collection.
 * @set errno - ENOMEM, BSE_WRONG_PARAMS
 **/
BsIntSet *bsintset_clone(BsIntSet *p_src) {
  if (p_src == NULL || p_src->bsize < BS_IDX_1) {
    errno = BSE_WRONG_PARAMS;
    BSLOG_LOG(BSLERROR, "NULL source or SIZE wrong \n");
    return NULL;
  }
  BsIntSet *obj = malloc(sizeof(BsIntSet));
  if (obj != NULL) {
    obj->vals = malloc(p_src->bsize * sizeof(int));
    if (obj->vals == NULL) {
      obj = bsintset_free(obj);
    } else {
      obj->bsize = p_src->bsize;
      obj->size = p_src->size;
      for (BS_IDX_T l = BS_IDX_0; l < obj->bsize; l++) {
        obj->vals[l] = p_src->vals[l];
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
 * @param pSet - collection, maybe NULL
 * @return always NULL
 **/
BsIntSet *bsintset_free(BsIntSet *pSet) {
 if (pSet != NULL) {
    if (pSet->vals != NULL) {
      free(pSet->vals);
    }
    free(pSet);
  }
  return NULL;
}

/**
 * <p>Clear collection.</p>
 * @param pSet - collection
 * @param pIniVal - initialize collection with this value
 **/
void bsintset_clear(BsIntSet *pSet, int pIniVal) {
  for (BS_IDX_T l = BS_IDX_0; l < pSet->bsize; l++) {
    pSet->vals[l] = pIniVal;
  }
  pSet->size = BS_IDX_0;
}

/**
 * <p>Clear last cell in collection,  i.e. sets to initial value and decreases size.</p>
 * @param pSet - collection
 * @param pIniVal - initialize collection with this value
 * @return current size
 * @set errno - BSE_ARR_OUT_OF_BOUNDS if nothing to remove
 **/
BS_IDX_T bsintset_remove_last(BsIntSet *pSet, int pIniVal) {
  if (pSet->size == BS_IDX_NULL) {
    errno = BSE_ARR_OUT_OF_BOUNDS;
    BSLOG_LOG(BSLERROR, "Out of size=%lu\n", pSet->size)
    return pSet->size;
  }
  pSet->size--;
  pSet->vals[pSet->size] = pIniVal;
  return pSet->size;
}

/**
* <p>Add int to the end of collection.
 * It will throw error if it's full-filled.</p>
 * @param pSet - collection
 * @param pVal - int
 * @return current size
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
BS_IDX_T bsintset_add(BsIntSet *pSet, int pVal) {
  if (pSet->size == pSet->bsize) {
    errno = BSE_ARR_OUT_OF_BOUNDS;
    BSLOG_LOG(BSLERROR, "Out of buffer size=%lu\n", pSet->bsize)
    return pSet->size;
  }
  pSet->vals[pSet->size] = pVal;
  pSet->size++;
  return pSet->size;
}

/**
* <p>Add int to the end of collection.
 * It increases collection if it's full-filled.</p>
 * @param pSet - collection
 * @param pVal - int
 * @param pInc - how much realloc when adding into
 *   full-filled collection, if BS_IDX_0 or less then error
 * @param pIniVal - initialize collection with this value
 * @return current size
 * @set errno - BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsintset_add_inc(BsIntSet *pSet, int pVal, BS_IDX_T pInc, int pIniVal) {
  if (pInc < BS_IDX_1) {
    errno = BSE_ARR_WPINCSIZE;
    BSLOG_LOG(BSLERROR, "increase wrong "BS_IDX_FMT"", pInc);
    return pSet->size;
  }
  if (pSet->size == pSet->bsize) {
    BS_IDX_T newsz = pSet->bsize + pInc;
    if (newsz <= pSet->bsize || newsz > BS_IDX_MAX) { //overflow
      errno = BSE_ARR_OUT_MAX_SIZE;
      BSLOG_ERR
      return pSet->size;
    }
    pSet->vals = realloc(pSet->vals, newsz * sizeof(int));
    if (pSet->vals == NULL) {
      if ( errno == 0 ) { errno = ENOMEM; }
      BSLOG_ERR
      pSet->size = BS_IDX_0;
      pSet->bsize = BS_IDX_0;
      return pSet->size;
    }
    pSet->bsize = newsz;
    for (BS_IDX_T l = pSet->size; l < pSet->bsize; l++) {
      pSet->vals[l] = pIniVal;
    }
  }
  pSet->vals[pSet->size] = pVal;
  pSet->size++;
  return pSet->size;
}
