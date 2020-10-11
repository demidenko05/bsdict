/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ collections (array of objects) library.
 * It consists of type-unsafe generic methods and data models
 * and type safe models and method's wrappers.</p>
 * @author Yury Demidenko
 **/

#include "stdlib.h"

#include "BsDataSet.h"
#include "BsError.h"

/**
 * <p>Constructor initializing with BS_IDX_NULL.</p>
 * @param pBufSz buffer size, if BS_IDX_0 or less then error
 * @return Array or NULL when OOM or wrong size.
 * @set errno - ENOMEM, BSE_ARR_WPSIZE
 **/
BsIdxSet *bsidxset_new(BS_IDX_T pBufSz) {
  if (pBufSz < BS_IDX_1) {
    errno = BSE_ARR_WPSIZE;
    BSLOG_LOG(BSLERROR, "SIZE wrong "BS_IDX_FMT"", pBufSz);
    return NULL;
  }
  BsIdxSet *obj = malloc(sizeof(BsIdxSet));
  if (obj != NULL) {
    obj->vals = malloc(pBufSz * sizeof(BS_IDX_T));
    if (obj->vals == NULL) {
     obj = bsidxset_free(obj);
   } else {
     obj->bsize = pBufSz;
     obj->size = BS_IDX_0;
     for (BS_IDX_T l = BS_IDX_0; l < obj->bsize; l++) {
       obj->vals[l] = BS_IDX_NULL;
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
 * @param pSet - data set, maybe NULL
 * @return always NULL
 **/
BsIdxSet *bsidxset_free(BsIdxSet *pSet) {
 if (pSet != NULL) {
    if (pSet->vals != NULL) {
      free(pSet->vals);
    }
    free(pSet);
  }
  return NULL;
}

/**
 * <p>Just set size to 0 and items to BS_IDX_NULL.
 * If set is null, then just report.</p>
 * @param pSet - data set
 **/
void
  bsidxset_clear (BsIdxSet *pSet)
{
  if ( pSet == NULL )
    { BSLOG_LOG (BSLWARN, "NULL IDXSET to clear\n") }
  for (BS_IDX_T l = BS_IDX_0; l < pSet->bsize; l++) {
    pSet->vals[l] = BS_IDX_NULL;
  }
  pSet->size = BS_IDX_0;
}

/**
 * <p>Constructor.</p>
 * @param pSize collection structure size, if 1 or less then error
 * @param pBufSz buffer size, if BS_IDX_0 or less then error
 * @return Array or NULL when OOM or wrong size.
 * @set errno - ENOMEM, BSE_ARR_WPSIZE
 **/
BsDataSetTus *bsdatasettus_new(size_t pSize, BS_IDX_T pBufSz) {
  if (pSize <= 1) {
    errno = BSE_ARR_WPSIZE;
    BSLOG_LOG(BSLERROR, "SIZE wrong %d", pSize);
    return NULL;
  }
  if (pBufSz < BS_IDX_1) {
    errno = BSE_ARR_WPSIZE;
    BSLOG_LOG(BSLERROR, "Buffer size wrong="BS_IDX_FMT"\n", pBufSz);
    return NULL;
  }
  BsDataSetTus *obj = malloc (pSize);
  if (obj != NULL) {
    obj->vals = malloc(pBufSz * sizeof(void*));
    if (obj->vals == NULL) {
      obj = bsdatasettus_free(obj, NULL);
    } else {
      obj->bsize = pBufSz;
      obj->size = BS_IDX_0;
      for (BS_IDX_T l = BS_IDX_0; l < obj->bsize; l++) {
       obj->vals[l] = NULL;
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
 * @param pSet - data set, maybe NULL
 * @param pObjDestr - record's destructor, maybe NULL
 * @return always NULL
 **/
BsDataSetTus *bsdatasettus_free(BsDataSetTus *pSet, Bs_Destruct *pObjDestr) {
  if (pSet != NULL) {
    if (pSet->vals != NULL) {
      if (pObjDestr != NULL) {
        for (BS_IDX_T l = BS_IDX_0; l < pSet->bsize; l++) {
          if (pSet->vals[l] != NULL) {
            pObjDestr(pSet->vals[l]);
          }
        }
      }
      free(pSet->vals);
    }
    free(pSet);
  }
  return NULL;
}

/**
 * <p>Clear all elements.</p>
 * @param pSet - data set, NOT NULL
 * @param pObjDestr - record's destructor, NOT NULL
 **/
void
  bsdatasettus_clear (BsDataSetTus *pSet, Bs_Destruct *pObjDestr)
{
  if ( pSet->vals != NULL )
  {
    for ( BS_IDX_T l = BS_IDX_0; l < pSet->bsize; l++ )
    {
      if ( pSet->vals[l] != NULL )
      {
        pSet->vals[l] = pObjDestr (pSet->vals[l]);
      }
    }
    pSet->size = BS_IDX_0;
  }
}

/**
 * <p>Increase array if it's full-filled.</p>
 * @param pSet - data set
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
void bsdatasettustst_inc(BsDataSetTus *pSet, BS_IDX_T pInc) {
  if (pInc < BS_IDX_1) {
    errno = BSE_ARR_WPINCSIZE;
    BSLOG_LOG(BSLERROR, "increase wrong "BS_IDX_FMT"\n", pInc);
    return;
  }
  if (pSet->size == pSet->bsize) {
    BS_IDX_T newsz = pSet->bsize + pInc;
    if (newsz <= pSet->bsize || newsz > BS_IDX_MAX) { //overflow
      errno = BSE_ARR_OUT_MAX_SIZE;
      BSLOG_ERR
      return;
    }
    pSet->vals = realloc(pSet->vals, newsz * sizeof(void*));
    if (pSet->vals == NULL) {
      if ( errno == 0 ) { errno = ENOMEM; }
      BSLOG_ERR
      pSet->bsize = BS_IDX_0;
      pSet->size = BS_IDX_0;
      return;
    }
    pSet->bsize = newsz;
    for (BS_IDX_T l = pSet->size; l < pSet->bsize; l++) {
      pSet->vals[l] = NULL;
    }
  }
}

/**
 * <p>Check arguments and increase array if it's full-filled.</p>
 * @param pSet - data set
 * @param pObj - object
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
void bsdatasettustst_check_inc(BsDataSetTus *pSet, void *pObj, BS_IDX_T pInc) {
  if (pObj == NULL) {
    errno = BSE_ARR_SET_NULL;
    BSLOG_ERR
    return;
  }
  bsdatasettustst_inc(pSet, pInc);
}

/**
 * <p>Add object to the end of array.
 * If it's full-filled, then throw error .</p>
 * @param pSet - data set
 * @param pObj - object NON-NULL
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_OF_BOUNDS
 **/
BS_IDX_T bsdatasettus_add(BsDataSetTus *pSet, void *pObj) {
  if (pObj == NULL) {
    errno = BSE_ARR_SET_NULL;
    BSLOG_ERR
    return BS_IDX_NULL;
  }
  if (pSet->size == pSet->bsize) {
    errno = BSE_ARR_OUT_OF_BOUNDS;
    BSLOG_LOG(BSLERROR, "Array full! size="BS_IDX_FMT"\n", pSet->size);
    return BS_IDX_NULL;
  }
  pSet->vals[pSet->size] = pObj;
  pSet->size++;
  return pSet->size - BS_IDX_1;
}

/**
 * <p>Add object to the end of array.
 * It increases array if it's full-filled.</p>
 * @param pSet - data set
 * @param pObj - object
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsdatasettus_add_inc(BsDataSetTus *pSet, void *pObj, BS_IDX_T pInc) {
  BS_DO_E_OUTE(bsdatasettustst_check_inc(pSet, pObj,  pInc))
  pSet->vals[pSet->size] = pObj;
  pSet->size++;
  return pSet->size - BS_IDX_1;
oute:
  return BS_IDX_NULL;
}

/**
 * <p>Check if collection contains given object.</p>
 * @param pSet - data set
 * @param pObj - object NON-NULL
 * @return index of object or BS_IDX_NULL if not found
 * @set errno -BSE_WRONG_PARAMS if any parameter is NULL
 **/
BS_IDX_T bsdatasettus_find(BsDataSetTus *pSet, void *pObj,
  Bs_Compare *pCompare) {
  if (pObj == NULL || pSet == NULL || pCompare == NULL) {
    errno = BSE_WRONG_PARAMS;
    BSLOG_ERR
    return BS_IDX_NULL;
  }
  if (pSet->size == BS_IDX_0) {
    return BS_IDX_NULL;
  } else {
    for (BS_IDX_T l = BS_IDX_0; l < pSet->size; l++) {
      if (pCompare(pObj, pSet->vals[l]) == 0) {
        return l;
      }
    }
    return BS_IDX_NULL;
  }
}

/**
 * <p>Find in sorted collection given object.</p>
 * @param pSet - pointer to sorted array
 * @param pObj - object NON-NULL
 * @return index of object or BS_IDX_NULL if not found
 * @set errno -BSE_WRONG_PARAMS if any parameter is NULL
 **/
BS_IDX_T bsdatasettus_sorted_find(BsDataSetTus *pSet, void *pObj,
  Bs_Compare *pCompare) {
  if (pObj == NULL || pSet == NULL || pCompare == NULL) {
    errno = BSE_WRONG_PARAMS;
    BSLOG_ERR
    return BS_IDX_NULL;
  }
  if (pSet->size == BS_IDX_0) {
    return BS_IDX_NULL;
  } else {
    BS_IDX_T idx_start = BS_IDX_0;
    BS_IDX_T idx_end = pSet->size - BS_IDX_1;
    BS_IDX_T idx_middle;
    if (idx_start == idx_end) {
      idx_middle = idx_start;
    } else {
      idx_middle = idx_start + (idx_end - idx_start) / BS_IDX_2;
    }
    BS_IDX_T idx_equal = BS_IDX_NULL;
    while (true) {
      int idx_start_was_eq_idx_end = (idx_start == idx_end);
      int cmpr = pCompare(pObj, pSet->vals[idx_middle]);
      if (cmpr == 0) {
        idx_equal = idx_middle;
        break;
      } else if (cmpr <= 0) {
        if (idx_middle == BS_IDX_0) {
          break;
        }
        if (!idx_start_was_eq_idx_end) {
          idx_end = idx_middle - BS_IDX_1;
        }
      } else {
        if (idx_middle + BS_IDX_1 == pSet->size) {
          break;
        }
        if (!idx_start_was_eq_idx_end) {
          idx_start = idx_middle + BS_IDX_1;
        }
      }
      if (idx_start_was_eq_idx_end || idx_start > idx_end) {
        break;
      }
      if (idx_start == idx_end) {
        idx_middle = idx_start;
      } else {
        idx_middle = idx_start + (idx_end - idx_start) / BS_IDX_2;
      }
    }
    return idx_equal;
  }
}

/**
 * <p>Add object into array with sorting.
 * It increases array if it's full-filled.</p>
 * @param pSet - data set
 * @param pObj - object
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @param pCompare comparator
 * @param pYesDup - if allow duplicates
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsdatasettus_add_sort_inc(BsDataSetTus *pSet, void *pObj, BS_IDX_T pInc,
  Bs_Compare *pCompare, bool pYesDup) {
  BS_DO_E_OUTE(bsdatasettustst_check_inc(pSet, pObj,  pInc))
  bool is_debug = bslog_is_debug(BS_DEBUGL_DATASET + 70);
  if (is_debug) {
    bslog_log(BSLONLYMSG, "Try to add obj#%p into pSet->size="BS_IDX_FMT", pSet->bsize="BS_IDX_FMT"... ", pObj, pSet->size, pSet->bsize);
  }
  //the first greater object's index (or equal if allow duplicates), if it's BS_IDX_NULL, then pObj is greatest/equals to last
  BS_IDX_T idx_greater = BS_IDX_NULL;
  if (pSet->size == BS_IDX_0) {
    pSet->vals[pSet->size] = pObj;
    idx_greater = pSet->size;
  } else {
    BS_IDX_T idx_start = BS_IDX_0;
    BS_IDX_T idx_end = pSet->size - BS_IDX_1;
    BS_IDX_T idx_middle;
    if (idx_start == idx_end) {
      idx_middle = idx_start;
    } else {
      idx_middle = idx_start + (idx_end - idx_start) / BS_IDX_2;
    }
    while (true) {
      int idx_start_was_eq_idx_end = (idx_start == idx_end);
      int cmpr = pCompare(pObj, pSet->vals[idx_middle]);
      if (cmpr < 0) {
        idx_greater = idx_middle;
        if (idx_middle == BS_IDX_0) {
          break;
        }
        if (!idx_start_was_eq_idx_end) {
          idx_end = idx_middle - BS_IDX_1;
        }
      } else { // equal/less
        if (cmpr == 0 && !pYesDup) {
          if (is_debug) {
            bslog_log(BSLONLYMSG, " rejecting duplicate on idx="BS_IDX_FMT"\n", idx_middle);
          }
          goto out; //reject duplicate
        }
        if (idx_middle + BS_IDX_1 == pSet->size) {
          break;
        }
        if (!idx_start_was_eq_idx_end) {
          idx_start = idx_middle + BS_IDX_1;
        }
      }
      if (idx_start_was_eq_idx_end || idx_start > idx_end) {
        break;
      }
      if (idx_start == idx_end) {
        idx_middle = idx_start;
      } else {
        idx_middle = idx_start + (idx_end - idx_start) / BS_IDX_2;
      }
    }
    if (is_debug) {
      bslog_log(BSLONLYMSG, " adding, idx_greater="BS_IDX_FMT"\n", idx_greater);
    }
    if (idx_greater == BS_IDX_NULL) { //pObj is greatest/equals to last
      pSet->vals[pSet->size] = pObj;
    } else {
      for (BS_IDX_T l = pSet->size; l > idx_greater; l--) {
        pSet->vals[l] = pSet->vals[l - BS_IDX_1];
      }
      pSet->vals[idx_greater] = pObj;
    }
  }
  pSet->size++;
out:
  return idx_greater;
oute:
  return BS_IDX_NULL;
}

/**
 * <p>Add index of given objects collection into sorted indexes.</p>
 * @param pIdxset - indexes set to add
 * @param pSet - collection of objects
 * @param pIdx index to add
 * @param pCompare comparator
 **/
void bsdatasettst_add_sortedidx(BsIdxSet *pIdxset,
  BsDataSetTus *pSet, BS_IDX_T pIdx, Bs_Compare *pCompare) {
  if (pIdxset->size == BS_IDX_0) {
    pIdxset->vals[pIdxset->size] = pIdx;
  } else {
    BS_IDX_T idx_start = BS_IDX_0;
    BS_IDX_T idx_end = pIdxset->size - BS_IDX_1;
    BS_IDX_T idx_middle;
    if (idx_start == idx_end) {
      idx_middle = idx_start;
    } else {
      idx_middle = idx_start + (idx_end - idx_start) / BS_IDX_2;
    }
    BS_IDX_T idx_gteq = BS_IDX_NULL;
    while (true) {
      int idx_start_was_eq_idx_end = (idx_start == idx_end);
      int cmpr = pCompare(pSet->vals[pIdx], pSet->vals[pIdxset->vals[idx_middle]]);
      if (cmpr <= 0) {
        idx_gteq = idx_middle;
        if (cmpr == 0 || idx_middle == BS_IDX_0) {
          break;
        }
        if (!idx_start_was_eq_idx_end) {
          idx_end = idx_middle - BS_IDX_1;
        }
      } else { // equal/less
        if (idx_middle + BS_IDX_1 == pSet->size) {
          break;
        }
        if (!idx_start_was_eq_idx_end) {
          idx_start = idx_middle + BS_IDX_1;
        }
      }
      if (idx_start_was_eq_idx_end || idx_start > idx_end) {
        break;
      }
      if (idx_start == idx_end) {
        idx_middle = idx_start;
      } else {
        idx_middle = idx_start + (idx_end - idx_start) / BS_IDX_2;
      }
    }
    if (idx_gteq == BS_IDX_NULL) { //idx is greatest
      pIdxset->vals[pIdxset->size] = pIdx;
    } else {
      for (BS_IDX_T l = pIdxset->size; l > idx_gteq; l--) {
        pIdxset->vals[l] = pIdxset->vals[l - BS_IDX_1];
      }
      pIdxset->vals[idx_gteq] = pIdx;
    }
  }
  pIdxset->size++;
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
 * @param pCompare comparator NON-NULL
 * @param pBufSz - desired buffer size, it's used if it's more than pSet->bsize
 * @return sorted index array or NULL if error
 * @set errno - BSE_WRONG_PARAMS if any parameter is NULL, if pSet->size < BS_IDX_2
 **/
BsIdxSet*
  bsdatasettus_create_idxset (BsDataSetTus *pSet, Bs_Compare *pCompare, BS_IDX_T pBufSz)
{
  if (pSet == NULL || pSet->size < BS_IDX_2 || pCompare == NULL) {
    errno = BSE_WRONG_PARAMS;
    BSLOG_ERR
    return NULL;
  }
  if ( pBufSz < pSet->size )
                    { pBufSz = pSet->size; }
  BS_DO_E_RETN (BsIdxSet *obj = bsidxset_new(pBufSz))
  for (BS_IDX_T l = BS_IDX_0; l < pSet->size; l++)
  {
    bsdatasettst_add_sortedidx (obj, pSet, l, pCompare);
  }
  return obj;
}

/**
 * <p>Redo sorted indexes array for given collection.
 * It increases sorted indexes array if need.
 * </p>
 * @param pSet - data set NON-NULL
 * @param pIdxSet sorted index array to redo NON-NULL
 * @param pCompare comparator NON-NULL
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @set errno - ENOMEM, BSE_WRONG_PARAMS if any parameter is NULL or pInc wrong
 **/
void
  bsdatasettus_redo_idxset (BsDataSetTus *pSet, BsIdxSet *pIdxSet,
                            Bs_Compare *pCompare, BS_IDX_T pInc)
{
  if ( pSet == NULL || pIdxSet == NULL || pCompare == NULL
                || pInc < BS_IDX_1 ) {
    errno = BSE_WRONG_PARAMS;
    BSLOG_ERR
    return;
  }
  if ( pSet->bsize > pIdxSet->bsize )
  {
    BS_IDX_T newsz = pIdxSet->bsize + pInc;
    if (newsz <= pIdxSet->bsize || newsz > BS_IDX_MAX) { //overflow
      errno = BSE_ARR_OUT_MAX_SIZE;
      BSLOG_ERR
      return;
    }
    pIdxSet->vals = realloc(pIdxSet->vals, newsz * sizeof(void*));
    if (pIdxSet->vals == NULL) {
      if ( errno == 0 ) { errno = ENOMEM; }
      BSLOG_ERR
      pIdxSet->bsize = BS_IDX_0;
      pIdxSet->size = BS_IDX_0;
      return;
    }
    pIdxSet->bsize = newsz;
  }
  pIdxSet->size = BS_IDX_0;
  BS_IDX_T l;
  for (l = BS_IDX_0; l < pIdxSet->bsize; l++) {
    pIdxSet->vals[l] = BS_IDX_NULL;
  }
  for (l = BS_IDX_0; l < pSet->size; l++) {
    bsdatasettst_add_sortedidx(pIdxSet, pSet, l, pCompare);
  }
}

/**
 * <p>Clear array's cell with given index and shrink array and decrease size.
 * If parameter object's destructor is not NULL, then free object.</p>
 * @param pSet - data set
 * @param pObjDestr - record's destructor, maybe NULL
 * @param pIdx - member index
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
void
  bsdatasettus_remove_shrink (BsDataSetTus *pSet, BS_IDX_T pIdx, Bs_Destruct *pObjDestr)
{
  if ( pIdx < BS_IDX_0 || pIdx >= pSet->size )
  {
    errno = BSE_ARR_OUT_OF_BOUNDS;
    BSLOG_ERR
    return;
  }
  if ( pObjDestr != NULL )
  {
    pObjDestr (pSet->vals[pIdx]);
  }
  for (BS_IDX_T l = pIdx + BS_IDX_1; l < pSet->size; l++ )
  {
    pSet->vals[l - BS_IDX_1] = pSet->vals[l];
  }
  pSet->vals[pSet->size - BS_IDX_1] = NULL;
  pSet->size--;
}

/**
 * <p>Move object UP (forth) if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pSet - data set
 * @param pIdx - member index
 * @return if done
 **/
bool bsdatasettus_move_up(BsDataSetTus *pSet, BS_IDX_T pIdx) {
  if (pIdx < BS_IDX_0 || pIdx >= pSet->size - 1) {
    return false;
  }
  void *obj = pSet->vals[pIdx];
  pSet->vals[pIdx] = pSet->vals[pIdx + 1];
  pSet->vals[pIdx + 1] = obj;
  return true;
}

/**
 * <p>Move object DOWN (back) if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pSet - data set
 * @param pIdx - member index
 * @return if done
 **/
bool bsdatasettus_move_down(BsDataSetTus *pSet, BS_IDX_T pIdx) {
  if (pIdx < BS_IDX_1 || pIdx >= pSet->size) {
    return false;
  }
  void *obj = pSet->vals[pIdx];
  pSet->vals[pIdx] = pSet->vals[pIdx - 1];
  pSet->vals[pIdx - 1] = obj;
  return true;
}

/**
 * <p>Move object if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pSet - data set
 * @param pIdx1 - index1
 * @param pIdx2 - index2
 * @return if done
 **/
bool bsdatasettus_move(BsDataSetTus *pSet, BS_IDX_T pIdx1, BS_IDX_T pIdx2) {
  if (pIdx1 == pIdx2 || pIdx1 < BS_IDX_0 || pIdx1 >= pSet->size
    || pIdx2 < BS_IDX_0 || pIdx2 >= pSet->size) {
    return false;
  }
  void *obj = pSet->vals[pIdx1];
  pSet->vals[pIdx1] = pSet->vals[pIdx2];
  pSet->vals[pIdx2] = obj;
  return true;
}

  //Type-safe wrappers:
/**
 * <p>Constructor.</p>
 * @param pBufSz buffer size, if BS_IDX_0 or less then error
 * @return Array or NULL when error.
 * @set errno - ENOMEM, BSE_ARR_WPSIZE
 **/
BsVoidMeths *bsvoidmeths_new(BS_IDX_T pBufSz) {
  return (BsVoidMeths*) bsdatasettus_new(sizeof(BsVoidMeths), pBufSz);
}

/**
 * <p>Destructor.</p>
 * @param pSet - data set
 * @return always NULL
 **/
BsVoidMeths *bsvoidmeths_free(BsVoidMeths *pSet) {
  if (pSet != NULL) {
    bsdatasettus_free((BsDataSetTus*) pSet, NULL);
  }
  return NULL;
}

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
BS_IDX_T bsvoidmeths_add_inc(BsVoidMeths *pSet, BsVoid_Method *pObj, BS_IDX_T pInc) {
  return bsdatasettus_add_inc((BsDataSetTus*) pSet, (void*) pObj, pInc);
}

/**
 * <p>Clear array's method with given index and shrink array and decrease size.</p>
 * @param pSet - data set
 * @param pIdx - member index
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
void
  bsvoidmeths_remove_shrink (BsVoidMeths *pSet, BS_IDX_T pIdx)
{
  bsdatasettus_remove_shrink ((BsDataSetTus*) pSet, pIdx, NULL);
}

/**
 * <p>Invoke all void methods in collection.
 * When client works with array via dedicated methods,
 * then there is no possible error like NULL array.
 * </p>
 * @param pSet - data set
 * @return quantity of invoked methods
 **/
int bsvoidmeths_invoke_all(BsVoidMeths *pSet) {
  BS_IDX_T i = 0;
  for (BS_IDX_T l = BS_IDX_0; l < pSet->bsize; l++) {
    if (pSet->vals[l] != NULL) {
      pSet->vals[l]();
      i++;
    }
  }
  return i;
}
