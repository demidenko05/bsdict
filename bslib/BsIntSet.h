/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ int(32)/enum collection library.</p>
 * @author Yury Demidenko
 **/
#ifndef BSINTSET_H
#define BSINTSET_H 1

#include "BsLog.h"

//TODO generic array of primitives (in way similar to fread), i.e.:
//#define BSVALSET(pValSize) BS_IDX_T bsize; BS_IDX_T size; int valSz; char *vals;

/**
 * <p>Int(32) base collection type.</p>
 * @param p_int32_t any int32 type including enums
 * @member BS_IDX_T bsize - buffer size more than 0
 * @member BS_IDX_T size - size from 0
 * @member p_int_t *vals - collection of values
 **/
#define BSINTSET(p_int32_t) BS_IDX_T bsize; BS_IDX_T size; p_int32_t *vals;

/**
 * <p>Int collection type.</p>
 * @extends BSINTSET(int)
 **/
typedef struct {
  BSINTSET(int)
} BsIntSet;

/**
 * <p>Constructor.</p>
 * @param pBufSz size of collection, if BS_IDX_0 or less then error
 * @param pIniVal - initialize collection with this value
 * @return Array or NULL when OOM or wrong size.
 * @set errno - ENOMEM, BSE_ARR_WPSIZE
 **/
BsIntSet *bsintset_new(BS_IDX_T pBufSz, int pIniVal);

#define BSINTSET_NEW_E_RETN(p_als,pBufSz,pIniVal) BsIntSet *p_als=bsintset_new(pBufSz,pIniVal);\
  if (errno != 0) { BSLOG_ERR return NULL; }
#define BSINTSET_NEW_E_RET(p_als,pBufSz,pIniVal) BsIntSet *p_als=bsintset_new(pBufSz,pIniVal);\
  if (errno != 0) { BSLOG_ERR return; }
#define BSINTSET_NEW_E_OUT(p_als,pBufSz,pIniVal) BsIntSet *p_als=bsintset_new(pBufSz,pIniVal);\
  if (errno != 0) { BSLOG_ERR goto out; }
#define BSINTSET_NEW_E_OUTE(p_als,pBufSz,pIniVal) BsIntSet *p_als=bsintset_new(pBufSz,pIniVal);\
  if (errno != 0) { BSLOG_ERR goto oute; }

/**
 * <p>Constructor clone.</p>
 * @param p_src source collection, not null
 * @return Array or NULL when OOM or wrong source collection.
 * @set errno - ENOMEM, BSE_WRONG_PARAMS
 **/
BsIntSet *bsintset_clone(BsIntSet *p_src);

/**
 * <p>Destructor.</p>
 * @param pSet - collection, maybe NULL
 * @return always NULL
 **/
BsIntSet *bsintset_free(BsIntSet *pSet);

/**
 * <p>Clear collection.</p>
 * @param pSet - collection
 * @param pIniVal - initialize collection with this value
 **/
void bsintset_clear(BsIntSet *pSet, int pIniVal);

/**
 * <p>Clear last cell in collection,  i.e. sets to initial value and decreases size.</p>
 * @param pSet - collection
 * @param pIniVal - initialize collection with this value
 * @return current size
 * @set errno - BSE_ARR_OUT_OF_BOUNDS if nothing to remove
 **/
BS_IDX_T bsintset_remove_last(BsIntSet *pSet, int pIniVal);

/**
* <p>Add int to the end of collection.
 * It will throw error if it's full-filled.</p>
 * @param pSet - collection
 * @param pVal - int
 * @return current size
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
BS_IDX_T bsintset_add(BsIntSet *pSet, int pVal); //TODO add_inc is best alternative

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
BS_IDX_T bsintset_add_inc(BsIntSet *pSet, int pVal, BS_IDX_T pInc, int pIniVal);
#endif
