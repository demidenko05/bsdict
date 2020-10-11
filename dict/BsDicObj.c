/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

#include "stdlib.h"
#include "wctype.h"
#include "string.h"

#include "BsError.h"
#include "BsDicObj.h"
#include "BsDicDescrDsl.h"
#include "BsDiIxTx.h"
#include "BsDiIxFind.h"
#include "BsDicLsa.h"

/**
 * <p>Beigesoft™ dictionary object (text/audio/both...)
 * with index and methods assembling library.</p>
 * @author Yury Demidenko
 **/

//Constructors/destructors:

/**
 * <p>Constructor.</p>
 * @param pIsIxRm is IDX RAM
 * @return prefs or NULL when OOM
 * @set errno - ENOMEM
 **/
BsDiPref*
  bsdipref_new (bool pIsIxRm)
{
  BsDiPref *obj = malloc (sizeof (BsDiPref));
  if ( obj != NULL ) {
    obj->isIxRm = pIsIxRm;
  }
  if (obj == NULL) {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}


/**
 * <p>Destructor.</p>
 * @param pPref prefs, maybe NULL
 * @return always NULL
 **/
BsDiPref*
  bsdipref_free (BsDiPref *pPref)
{
  if ( pPref != NULL )
  {
    free (pPref);
  }
  return NULL;
}

//Adapters:

/**
 * <p>Read word's description DIC-IDX text DSL adapter (wrapper).</p>
 * @param pDiIx - DIC with IDX
 * @param pFdWrd - found word with data to search content
 * @return full description as BsHypStrs
 * @set errno if error.
 **/
static BsHypStrs*
  s_bsdicdsl_read (BsDiIxBs *pDiIx, BsDiFdWd *pFdWrd)
{
  for ( int i = 0; i < pFdWrd->dicOfsts->size; i++ )
  {
    if ( pFdWrd->dicOfsts->vals[i]->diIx == pDiIx )
    {
      return bsdicdescrdsl_read(pDiIx->dicFl, pFdWrd->dicOfsts->vals[i]->ofst);
    }
  }
  return NULL;
}

/**
 * <p>Constructor.</p>
 * @param pPth - just chosen path
 * @param pIsIxRm - client prefers IRT (index records table) in memory (RAM) than in file
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicObj*
  bsdicobj_new (char *pPth, bool pIsIxRm)
{
  BsDicObj *obj = malloc (sizeof (BsDicObj));
  if ( obj != NULL )
  {
    obj->diIx = NULL; obj->pth = NULL; obj->nme = NULL; obj->opSt = NULL; obj->pref = NULL;
    obj->diix_destroy = NULL; obj->diixfind_mtch = NULL; obj->diix_read = NULL;
    obj->pth = bsstring_new (pPth);
    if ( obj->pth == NULL )
    {
      obj = bsdicobj_free (obj);
    } else {
      char *nm = strrchr (pPth, '/');
      if ( nm != NULL )
      {
        nm = nm + 1;
      } else {
        nm = pPth;
      }
      obj->nme = bsstring_new (nm);
      if ( obj->nme == NULL )
      {
        obj = bsdicobj_free (obj);
      } else {
        obj->opSt = bsdiixost_new ();
        if ( obj->opSt == NULL )
        {
          obj = bsdicobj_free (obj);
        } else {
          obj->pref = bsdipref_new (pIsIxRm);
          if ( obj->opSt == NULL )
          {
            obj = bsdicobj_free (obj);
          }
        }
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
 * <p>Generic opener (load or create) of DIC IDX with methods object.</p>
 * @param pDiObj - dictionary object to open.
 * @clear errno if error with reporting
 **/
void
  bsdicobj_open (BsDicObj* pDiObj)
{
  bool isLsa = false;
  if ( strncmp (pDiObj->pth->val + ( strlen (pDiObj->pth->val) - 4 ), ".lsa", 4) == 0 ) //TODO 1 more clever method
                  { isLsa = true; }
  if ( isLsa )
  {
    pDiObj->diIx = (BsDiIxBs*) bsdiixlsa_open (pDiObj->pth->val, pDiObj->opSt);
  } else {
    pDiObj->diIx = (BsDiIxBs*) bsdiixtx_open (pDiObj->pth->val, pDiObj->opSt, pDiObj->pref->isIxRm);
  }
  if ( pDiObj->diIx != NULL )
  {
    if ( isLsa )
    {
      pDiObj->diix_destroy = (BsDiIx_Destroy*) &bsdiixt2_destroy;
      if ( pDiObj->pref->isIxRm )
      {
        BSLOG_LOG (BSLERROR, "LSA RAM not yet implemented\n")
        return;
      } else {
        pDiObj->diixfind_mtch = (BsDiIxFind_Mtch*) &bsdiclsafind_mtch;
      }
      pDiObj->diix_read = (BsDiIx_Read*) &bsdiclsa_read;
    } else {
      pDiObj->diix_destroy = (BsDiIx_Destroy*) &bsdiixtx_destroy;
      if ( pDiObj->pref->isIxRm )
      {
        pDiObj->diixfind_mtch = (BsDiIxFind_Mtch*) &bsdiixtxrmfind_mtch;
      } else {
        pDiObj->diixfind_mtch = (BsDiIxFind_Mtch*) &bsdiixtxfind_mtch;
      }
      if ( pDiObj->diIx->head->frmt == DFRM_DSL )
      {
        pDiObj->diix_read = (BsDiIx_Read*) &s_bsdicdsl_read;
      }
      else {
        BSLOG_LOG (BSLERROR, "Read word's content not yet implemented for format=%d\n",  pDiObj->diIx->head->frmt)
        pDiObj->diIx = pDiObj->diix_destroy (pDiObj->diIx);
      }
    }
  }
  errno = 0;
}

/**
 * <p>Generic reopener (load or create) of DIC IDX with methods object.
 * This reopen DIC with IDX according just changed preferences,
 * e.g. user wants to switch IDX FILE into IDX RAM.
 * </p>
 * @param pDiObj - dictionary object to reopen.
 * @clear errno if error with reporting
 **/
void
  bsdicobj_reopen (BsDicObj* pDiObj)
{
  errno = BSE_UNIMPLEMENTED;
  BSLOG_ERR;
  //TODO 1
}

/**
 * <p>Destructor.</p>
 * @param pDiObj - maybe NULL
 * @return always NULL
 **/
BsDicObj *bsdicobj_free(BsDicObj *pDiObj) {
  if ( pDiObj != NULL )
  {
    bsstring_free (pDiObj->nme);
    bsstring_free (pDiObj->pth);
    bsdiixost_free (pDiObj->opSt);
    bsdipref_free (pDiObj->pref);
    if ( pDiObj->diIx != NULL )
          { pDiObj->diix_destroy (pDiObj->diIx); }
    free (pDiObj);
  }
  return NULL;
}

/**
 * <p>Comparator.</p>
 * @param pDiObj1 NOT NULL
 * @param pDiObj2 NOT NULL
 * @return dic1.path -1 less 0 equal 1 greater than dic2.path
 **/
int
  bsdicobj_compare (BsDicObj *pDiObj1, BsDicObj *pDiObj2)
{
  if (pDiObj1->pth == NULL || pDiObj2->pth == NULL) {
    return -10;
  }
  return strcmp(pDiObj1->pth->val, pDiObj2->pth->val);
}

/**
 * <p>Comparator of address.</p>
 * @param pDiObj1 NOT NULL
 * @param pDiObj2 NOT NULL
 * @return dic1 address -1 less 0 equal 1 greater than dic2.path
 **/
int
  bsdicobj_compare_ref (BsDicObj *pDiObj1, BsDicObj *pDiObj2)
{
  if ( pDiObj1 < pDiObj2 ) {
    return -1;
  } else if ( pDiObj1 == pDiObj2 ) {
    return 0;
  } else {
    return 1;
  }
}

/**
 * <p>Constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicObjs *bsdicobjs_new(BS_IDX_T pBufSz) {
  return (BsDicObjs *) bsdatasettus_new(sizeof(BsDicObjs), pBufSz);
}

/**
 * <p>Destructor.</p>
 * @param pDiObjs - collection
 * @return always NULL
 **/
BsDicObjs *bsdicobjs_free(BsDicObjs *pDiObjs) {
  bsdatasettus_free((BsDataSetTus*) pDiObjs, (Bs_Destruct*) &bsdicobj_free);
  return NULL;
}

/**
 * <p>Add object to the first null cell of collection.
 * It increases collection if it's full-filled.</p>
 * @param pDiObjs - collection
 * @param pObj - object
 * @param pInc - how much realloc when adding into
 *   full-filled collection, if BS_IDX_0 or less then error
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsdicobjs_add_inc(BsDicObjs *pDiObjs, BsDicObj *pObj, BS_IDX_T pInc) {
  return bsdatasettus_add_inc((BsDataSetTus*) pDiObjs, pObj, pInc);
}

/**
 * <p>Clear and free array's element with given index and shrink array and decrease size.</p>
 * @param pDiObjs - data set
 * @param pIdx - member index
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
void bsdicobjs_remove_shrink(BsDicObjs *pDiObjs, BS_IDX_T pIdx) {
  bsdatasettus_remove_shrink((BsDataSetTus*) pDiObjs, pIdx, (Bs_Destruct*) &bsdicobj_free);
}

/**
 * <p>Find in collection given dic with same path.</p>
 * @param pDiObjs - data set
 * @param pObj - object NON-NULL
 * @return index of object or BS_IDX_NULL if not found
 * @set errno -BSE_WRONG_PARAMS if any parameter is NULL
 **/
BS_IDX_T
  bsdicobjs_find (BsDicObjs *pDiObjs, BsDicObj *pObj)
{
  return bsdatasettus_find ((BsDataSetTus*) pDiObjs,
                      (void*) pObj, (Bs_Compare*) &bsdicobj_compare);
}

/**
 * <p>Find in collection given dic with same address.</p>
 * @param pDiObjs - data set
 * @param pObj - object NON-NULL
 * @return index of object or BS_IDX_NULL if not found
 * @set errno -BSE_WRONG_PARAMS if any parameter is NULL
 **/
BS_IDX_T
  bsdicobjs_find_ref (BsDicObjs *pDiObjs, BsDicObj *pObj)
{
  return bsdatasettus_find ((BsDataSetTus*) pDiObjs,
                      (void*) pObj, (Bs_Compare*) &bsdicobj_compare_ref);
}


/**
 * <p>Find in collection given dic with IDX.</p>
 * @param pDiObjs - data set
 * @param pDiIx - object NON-NULL
 * @return index of object or BS_IDX_NULL if not found
 * @set errno -BSE_WRONG_PARAMS if any parameter is NULL
 **/
BS_IDX_T
  bsdicobjs_find_diix (BsDicObjs *pDiObjs, BsDiIxBs *pDiIx)
{
  for ( BS_IDX_T l = BS_IDX_0; l < pDiObjs->size;  l++ )
  {
    if ( pDiObjs->vals[l]->diIx == pDiIx )
            { return l; }
  }
  return BS_IDX_NULL;
}

/**
 * <p>Move object UP (forth) if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pDiObjs - data set
 * @param pIdx - member index
 * @return if done
 **/
bool
  bsdicobjs_move_up (BsDicObjs *pDiObjs, BS_IDX_T pIdx)
{
  return bsdatasettus_move_up((BsDataSetTus*) pDiObjs, pIdx);
}

/**
 * <p>Move object DOWN (back) if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pDiObjs - data set
 * @param pIdx - member index
 * @return if done
 **/
bool bsdicobjs_move_down(BsDicObjs *pDiObjs, BS_IDX_T pIdx) {
  return bsdatasettus_move_down((BsDataSetTus*) pDiObjs, pIdx);
}

/**
 * <p>Move object if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pDiObjs - data set
 * @param pIdx1 - index1
 * @param pIdx2 - index2
 * @return if done
 **/
bool bsdicobjs_move(BsDicObjs *pDiObjs, BS_IDX_T pIdx1, BS_IDX_T pIdx2) {
  return bsdatasettus_move((BsDataSetTus*) pDiObjs, pIdx1, pIdx2);
}
