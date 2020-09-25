/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ dictionary object (text/audio/both...)
 * with index and methods assembling library.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICOBJ
#define BS_DEBUGL_DICOBJ 33000

#include "BsDiIx.h"

/**
 * <p>Client's preferences.</p>
 * @member bool isIxRm - client prefers IRT (index records table) in memory (RAM) than in file
 **/
typedef struct {
  bool isIxRm;
} BsDiPref;

/**
 * <p>Constructor.</p>
 * @param pIsIxRm is IDX RAM
 * @return prefs or NULL when OOM
 * @set errno - ENOMEM
 **/
BsDiPref* bsdipref_new (bool pIsIxRm);

/**
 * <p>Destructor.</p>
 * @param pPref prefs, maybe NULL
 * @return always NULL
 **/
BsDiPref* bsdipref_free (BsDiPref *pPref);

  //To make OOP like generic object with methods:

/**
 * <p>Generic destructor.</p>
 * @param pDiIx - object or NULL
 * @return always NULL
 **/
typedef BsDiIxBs* BsDiIx_Destroy (BsDiIxBs *pDiIx);

/**
 * <p>Find all matched words in given dictionary and IDX.</p>
 * @param pDiIx - DIC with IDX
 * @param pFdWrds - collection to add found record
 * @param pSbwrd - sub-word to match
 * @set errno if error.
 **/
typedef void BsDiIxFind_Mtch (BsDiIxBs *pDiIx, BsDiFdWds *pFdWrds, char *pSbwrd);

/**
 * <p>Read word's description with substituted DIC's tags by HTML ones
 * from dictionary with search content any type.</p>
 * @param pDiIx - DIC with IDX
 * @param pFdWrd - found word with data to search content
 * @return full description as BsHypStrs
 * @set errno if error.
 **/
typedef BsHypStrs *BsDiIx_Read (BsDiIxBs *pDiIx, BsDiFdWd *pFdWrd);

/**
 * <p>Generic, type-safe assembly of text/audio/both/... dictionary
 * with cached IDX head and methods (OOP like object).
 * This is interface for high level GUI.
 * This exposes abstractions (data and methods) that GUI needs.</p>
 * @member nme - file name plus state - e.g. indexing..., it will be hided in GUI with opened diIx-head->nme
 * @member pth - file path either from bsdict.conf or that user chose
 * @member opSt - opening shared data
 * @member pref - user preferences
 * @member diIx - text/audio/both/... dictionary with cached IDX head
 * @method diix_destroy - destroyer
 * @method diixfind_mtch - finder of matched words
 * @method diix_read - reader of content of found word
 **/
typedef struct {
  BsString *nme;
  BsString *pth;
  BsDiIxOst *opSt;
  BsDiPref *pref;
  BsDiIxBs *diIx;
  BsDiIx_Destroy *diix_destroy;
  BsDiIxFind_Mtch *diixfind_mtch;
  BsDiIx_Read *diix_read;
} BsDicObj;

/**
 * <p>Constructor.</p>
 * @param pPth - just chosen path
 * @param pIsIxRm - client prefers IRT (index records table) in memory (RAM) than in file
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicObj* bsdicobj_new (char *pPth, bool pIsIxRm);

/**
 * <p>Generic opener (load or create) of DIC IDX with methods object.</p>
 * @param pDiObj - dictionary object to open.
 * @clear errno if error with reporting
 **/
void bsdicobj_open (BsDicObj* pDiObj);

/**
 * <p>Generic reopener (load or create) of DIC IDX with methods object.
 * This reopen DIC with IDX according just changed preferences,
 * e.g. user wants to switch IDX FILE into IDX RAM.
 * </p>
 * @param pDiObj - dictionary object to reopen.
 * @clear errno if error with reporting
 **/
void bsdicobj_reopen (BsDicObj* pDiObj);

/**
 * <p>Destructor.</p>
 * @param pDiObj - maybe NULL
 * @return always NULL
 **/
BsDicObj *bsdicobj_free(BsDicObj *pDiObj);

/**
 * <p>Comparator.</p>
 * @param pDiObj1 NOT NULL
 * @param pDiObj2 NOT NULL
 * @return dic1.path -1 less 0 equal 1 greater than dic2.path
 **/
int bsdicobj_compare (BsDicObj *pDiObj1, BsDicObj *pDiObj2);

/**
 * <p>Comparator of address.</p>
 * @param pDiObj1 NOT NULL
 * @param pDiObj2 NOT NULL
 * @return dic1 address -1 less 0 equal 1 greater than dic2.path
 **/
int bsdicobj_compare_ref (BsDicObj *pDiObj1, BsDicObj *pDiObj2);

/**
 * <p>Working dictionaries.</p>
 * @extends BSDATASET(BsDicObj)
 **/
typedef struct {
  BSDATASET(BsDicObj)
} BsDicObjs;

/**
 * <p>Constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicObjs *bsdicobjs_new (BS_IDX_T pBufSz);

/**
 * <p>Destructor.</p>
 * @param pDiObjs - collection
 * @return always NULL
 **/
BsDicObjs *bsdicobjs_free (BsDicObjs *pDiObjs);

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
BS_IDX_T bsdicobjs_add_inc (BsDicObjs *pDiObjs, BsDicObj *pObj, BS_IDX_T pInc);

/**
 * <p>Clear (set to NULL) array's cell with given index and shrink array and decrease size.</p>
 * @param pDiObjs - data set
 * @param pIdx - member index
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
void bsdicobjs_remove_shrink (BsDicObjs *pDiObjs, BS_IDX_T pIdx);

/**
 * <p>Find in collection given dic with same path.</p>
 * @param pDiObjs - data set
 * @param pObj - object NON-NULL
 * @return index of object or BS_IDX_NULL if not found
 * @set errno -BSE_WRONG_PARAMS if any parameter is NULL
 **/
BS_IDX_T bsdicobjs_find (BsDicObjs *pDiObjs, BsDicObj *pObj);

/**
 * <p>Find in collection given dic with same address.</p>
 * @param pDiObjs - data set
 * @param pObj - object NON-NULL
 * @return index of object or BS_IDX_NULL if not found
 * @set errno -BSE_WRONG_PARAMS if any parameter is NULL
 **/
BS_IDX_T bsdicobjs_find_ref (BsDicObjs *pDiObjs, BsDicObj *pObj);

/**
 * <p>Find in collection given dic with IDX.</p>
 * @param pDiObjs - data set
 * @param pDiIx - object NON-NULL
 * @return index of object or BS_IDX_NULL if not found
 * @set errno -BSE_WRONG_PARAMS if any parameter is NULL
 **/
BS_IDX_T bsdicobjs_find_diix (BsDicObjs *pDiObjs, BsDiIxBs *pDiIx);

/**
 * <p>Move object UP (forth) if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pDiObjs - data set
 * @param pIdx - member index
 * @return if done
 **/
bool bsdicobjs_move_up (BsDicObjs *pDiObjs, BS_IDX_T pIdx);

/**
 * <p>Move object DOWN (back) if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pDiObjs - data set
 * @param pIdx - member index
 * @return if done
 **/
bool bsdicobjs_move_down (BsDicObjs *pDiObjs, BS_IDX_T pIdx);

/**
 * <p>Move object if parameters OK. This is for manually sorting.
 * This is fault tolerated method.</p>
 * @param pDiObjs - data set
 * @param pIdx1 - index1
 * @param pIdx2 - index2
 * @return if done
 **/
bool bsdicobjs_move (BsDicObjs *pDiObjs, BS_IDX_T pIdx1, BS_IDX_T pIdx2);

#endif
