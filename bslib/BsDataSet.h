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
#ifndef BS_DEBUGL_DATASET

#include "BsLog.h"

#define BS_DEBUGL_DATASET 1

/**
 * <p>Multi-functional non-NULL objects collection.</p>
 * @param pSetType - array type, e.g. void for generic Object
 * @member BS_IDX_T bsize - buffer size more than 0
 * @member BS_IDX_T size - size from 0
 * @member [OBJECT_TYPE] **vals - array of objects
 **/
#define BSDATASET(pSetType) BS_IDX_T bsize; BS_IDX_T size; pSetType **vals;

/**
 * <p>Type-unsafe multi-functional non-NULL objects collection.</p>
 * @extends BSDATASET(void)
 **/
typedef struct {
  BSDATASET(void)
} BsDataSetTus;

/**
 * <p>Constructor.</p>
 * @param pSize collection structure size, if 1 or less then error
 * @param pBufSz buffer size, if BS_IDX_0 or less then error
 * @return Array or NULL when OOM or wrong size.
 * @set errno - ENOMEM, BSE_ARR_WPSIZE
 **/
BsDataSetTus *bsdatasettus_new(size_t pSize, BS_IDX_T pBufSz);

/**
 * <p>Destructor.</p>
 * @param pSet - data set, maybe NULL
 * @param pObjDestr - record's destructor, maybe NULL
 * @return always NULL
 **/
BsDataSetTus *bsdatasettus_free(BsDataSetTus *pSet, Bs_Destruct *pObjDestr);

/**
 * <p>Clear all elements.</p>
 * @param pSet - data set, NOT NULL
 * @param pObjDestr - record's destructor, NOT NULL
 **/
void bsdatasettus_clear(BsDataSetTus *pSet, Bs_Destruct *pObjDestr);

/**
 * <p>Find in collection given object.</p>
 * @param pSet - data set
 * @param pObj - object NON-NULL
 * @param pCompare comparator NON-NULL
 * @return index of object or BS_IDX_NULL if not found
 * @set errno -BSE_WRONG_PARAMS if any parameter is NULL
 **/
BS_IDX_T bsdatasettus_find(BsDataSetTus *pSet, void *pObj,
  Bs_Compare *pCompare);

/**
 * <p>Find in sorted collection given object.</p>
 * @param pSet - pointer to sorted array
 * @param pObj - object NON-NULL
 * @param pCompare comparator NON-NULL
 * @return index of object or BS_IDX_NULL if not found
 * @set errno -BSE_WRONG_PARAMS if any parameter is NULL
 **/
BS_IDX_T bsdatasettus_sorted_find(BsDataSetTus *pSet, void *pObj,
  Bs_Compare *pCompare);

/**
 * <p>Add object to the end of array.
 * If it's full-filled, then throw error .</p>
 * @param pSet - data set
 * @param pObj - object NON-NULL
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_OF_BOUNDS
 **/
BS_IDX_T bsdatasettus_add(BsDataSetTus *pSet, void *pObj);

/**
 * <p>Add object to the end of array.
 * It increases array if it's full-filled.</p>
 * @param pSet - data set
 * @param pObj - object NON-NULL
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsdatasettus_add_inc(BsDataSetTus *pSet, void *pObj, BS_IDX_T pInc);

/**
 * <p>Add object into array with sorting.
 * It increases array if it's full-filled.</p>
 * @param pSet - data set
 * @param pObj - object NON-NULL
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @param pCompare comparator NON-NULL
 * @param pYesDup - if allow duplicates
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsdatasettus_add_sort_inc(BsDataSetTus *pSet, void *pObj, BS_IDX_T pInc,
  Bs_Compare *pCompare, bool pYesDup);

/**
 * <p>Clear array's cell with given index and shrink array and decrease size.
 * If parameter object's destructor is not NULL, then free object.</p>
 * @param pSet - data set
 * @param pIdx - member index
 * @param pObjDestr - record's destructor, maybe NULL
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
void bsdatasettus_remove_shrink(BsDataSetTus *pSet, BS_IDX_T pIdx, Bs_Destruct *pObjDestr);

/**
 * <p>Move object UP (forth) if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pSet - data set
 * @param pIdx - member index
 * @return if done
 **/
bool bsdatasettus_move_up(BsDataSetTus *pSet, BS_IDX_T pIdx);

/**
 * <p>Move object DOWN (back) if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pSet - data set
 * @param pIdx - member index
 * @return if done
 **/
bool bsdatasettus_move_down(BsDataSetTus *pSet, BS_IDX_T pIdx);

/**
 * <p>Move object if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pSet - data set
 * @param pIdx1 - index1
 * @param pIdx2 - index2
 * @return if done
 **/
bool bsdatasettus_move(BsDataSetTus *pSet, BS_IDX_T pIdx1, BS_IDX_T pIdx2);

/**
 * <p>BS_IDX_T collection.</p>
 * @member BS_IDX_T bsize - buffer size more than 0
 * @member BS_IDX_T size - size from 0
 * @member BS_IDX_T *vals - array of indexes
 **/
typedef struct {
  BS_IDX_T bsize;
  BS_IDX_T size;
  BS_IDX_T *vals;
} BsIdxSet;

/**
 * <p>Constructor initializing with BS_IDX_NULL.</p>
 * @param pBufSz buffer size, if BS_IDX_0 or less then error
 * @return Array or NULL when OOM or wrong size.
 * @set errno - ENOMEM, BSE_ARR_WPSIZE
 **/
BsIdxSet *bsidxset_new(BS_IDX_T pBufSz);

/**
 * <p>Destructor.</p>
 * @param pSet - data set, maybe NULL
 * @return always NULL
 **/
BsIdxSet *bsidxset_free(BsIdxSet *pSet);

/**
 * <p>Just set size to 0 and items to BS_IDX_NULL.
 * If set is null, then just report.</p>
 * @param pSet - data set
 **/
void bsidxset_clear(BsIdxSet *pSet);


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
BsIdxSet *bsdatasettus_create_idxset(BsDataSetTus *pSet, Bs_Compare *pCompare, BS_IDX_T pBufSz);

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
void bsdatasettus_redo_idxset(BsDataSetTus *pSet, BsIdxSet *pIdxSet,
  Bs_Compare *pCompare, BS_IDX_T pInc);

/**
 * <p>Type-safe multi-functional pointers to (void method(void)) collection.</p>
 * @extends BSDATASET(BsVoid_Method)
 **/
typedef struct {
  BSDATASET(BsVoid_Method)
} BsVoidMeths;

  //Type-safe wrappers:
/**
 * <p>Constructor.</p>
 * @param pBufSz buffer size, if BS_IDX_0 or less then error
 * @return Array or NULL when error.
 * @set errno - ENOMEM, BSE_ARR_WPSIZE
 **/
BsVoidMeths *bsvoidmeths_new(BS_IDX_T pBufSz);

#define BSVOIDMETHS_NEW_E_RET(p_als, pBufSz) BsVoidMeths *p_als=bsvoidmeths_new(pBufSz);\
  if (errno != 0) { return; }
#define BSVOIDMETHS_NEW_E_OUT(p_als, pBufSz) BsVoidMeths *p_als=bsvoidmeths_new(pBufSz);\
  if (errno != 0) { goto out; }

/**
 * <p>Destructor.</p>
 * @param pSet - data set
 * @return always NULL
 **/
BsVoidMeths *bsvoidmeths_free(BsVoidMeths *pSet);

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
BS_IDX_T bsvoidmeths_add_inc(BsVoidMeths *pSet, BsVoid_Method *pObj, BS_IDX_T pInc);

/**
 * <p>Clear array's method with given index and shrink array and decrease size.</p>
 * @param pSet - data set
 * @param pIdx - member index
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
void bsvoidmeths_remove_shrink(BsVoidMeths *pSet, BS_IDX_T pIdx);

/**
 * <p>Invoke all void methods in collection.
 * When client works with array via dedicated methods,
 * then there is no possible error like NULL array.
 * </p>
 * @param pSet - data set
 * @return quantity of invoked methods.
 **/
int bsvoidmeths_invoke_all(BsVoidMeths *pSet);
#endif
