/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

#include "stdlib.h"

#include "BsLog.h"
#include "BsError.h"
#include "BsDicDescr.h"

static char *sBsHyTagNms[BSDICDESCR_TAGS_CNT] = { "EBSHT_EMPTY", "EBSHT_BOLD", "EBSHT_ITALIC", "EBSHT_RED", "EBSHT_GRAY", "EBSHT_GREEN",
  "EBSHT_TAB1", "EBSHT_TAB2", "EBSHT_TAB3", "EBSHT_TAB4", "EBSHT_TAB5", "EBSHT_TAB6", "EBSHT_TAB7", "EBSHT_TAB8", "EBSHT_TAB9", "EBSHT_TAB10",
  "EBSHT_TOOLTIP", "EBSHT_AUDIO" };

/**
 * <p>Get hype-tag name.</p>
 * @param pTag tag number
 * @return tag name or NULL without reporting
 **/
char*
  bshyptag_name (EBsHypTag pTag)
{
  if ( pTag < 1 || pTag >= BSDICDESCR_TAGS_CNT )
                    { return NULL; }

  return sBsHyTagNms[pTag];
}

/**
 * <p>Constructor.</p>
 * @param pBufSz size of collection, if BS_IDX_0 or less then error
 * @return Array or NULL when OOM or wrong size.
 * @set errno - ENOMEM, BSE_ARR_WPSIZE
 **/
BsHypTags *bshyptags_new(BS_IDX_T pBufSz) {
  return (BsHypTags*) bsintset_new(pBufSz, EBSHT_EMPTY);
}

/**
 * <p>Constructor clone.</p>
 * @param p_src source collection, not null
 * @return Array or NULL when OOM or wrong source collection.
 * @set errno - ENOMEM, BSE_WRONG_PARAMS
 **/
BsHypTags *bshyptags_clone(BsHypTags *p_src) {
  return (BsHypTags*) bsintset_clone((BsIntSet*) p_src);
}

/**
 * <p>Destructor.</p>
 * @param pSet - collection, maybe NULL
 * @return always NULL
 **/
BsHypTags *bshyptags_free(BsHypTags *pSet) {
  bsintset_free((BsIntSet*) pSet);
  return NULL;
}

/**
 * <p>Clear collection.</p>
 * @param pSet - collection
 **/
void bshyptags_clear(BsHypTags *pSet) {
  bsintset_clear((BsIntSet*) pSet, EBSHT_EMPTY);
}

/**
* <p>Add int to the end of collection.
 * It will throw error if it's full-filled.</p>
 * @param pSet - collection
 * @param pVal - EBsHypTag
 * @return current size
 * @set errno - BSE_ARR_OUT_OF_BOUNDS
 **/
BS_IDX_T bshyptags_add(BsHypTags *pSet, EBsHypTag pVal) {
  return bsintset_add((BsIntSet*) pSet, pVal);
}

/**
 * <p>Clear last cell in collection, i.e. sets to initial value and decreases size.
 * It's tolerated if collection is empty.</p>
 * @param pSet - collection
 * @return current size
 **/
BS_IDX_T bshyptags_remove_last_tlrn(BsHypTags *pSet) {
  if (pSet->size > BS_IDX_0) {
    bsintset_remove_last((BsIntSet*) pSet, EBSHT_EMPTY);
  //} else {
    //BSLOG_LOG(BSLWARN, "Attempt to remove last cell of empty collection!\n")
  }
  return pSet->size;
}

/**
 * <p>Constructor.</p>
 * @param pCstr string not NULL
 * @param p_tags_to_clone - tags to clone, maybe NULL or empty
 * @param pOfst - optional content rec. offset
 * @param pLen - optional content rec. length
 * @return object or NULL when error
 * @set errno if error.
 **/
BsHypStr*
  bshypstr_new (char *pCstr, BsHypTags *p_tags_to_clone,
                unsigned int pOfst, unsigned int pLen)
{
  BsHypStr *obj = malloc (sizeof(BsHypStr));
  if ( obj != NULL )
  {
    obj->tags = NULL;
    obj->ofst = pOfst;
    obj->len = pLen;
    obj->str = bsstring_new (pCstr);
    if ( obj->str == NULL )
    {
      obj = bshypstr_free (obj);
    } else {
      if ( p_tags_to_clone != NULL && p_tags_to_clone->size > BS_IDX_0 )
      {
        obj->tags = bshyptags_clone (p_tags_to_clone);
        if ( obj->tags == NULL )
                { obj = bshypstr_free (obj); }
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
 * @param p_hstr - pointer to hyper-string
 * @return always NULL
 **/
BsHypStr*
  bshypstr_free (BsHypStr *p_hstr)
{
  if ( p_hstr != NULL )
  {
    if ( p_hstr->tags != NULL)
          { bshyptags_free (p_hstr->tags); }

    if ( p_hstr->str != NULL)
          { bsstring_free (p_hstr->str); }

    free(p_hstr);
  }
  return NULL;
}

/**
 * <p>Constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsHypStrs *bshypstrs_new(BS_IDX_T pBufSz) {
  return (BsHypStrs*) bsdatasettus_new(sizeof(BsHypStrs), pBufSz);
}

/**
 * <p>Destructor.</p>
 * @param pSet - collection
 * @return always NULL
 **/
BsHypStrs *bshypstrs_free(BsHypStrs *pSet) {
  if (pSet != NULL) {
    bsdatasettus_free((BsDataSetTus*) pSet, (Bs_Destruct*) &bshypstr_free);
  }
  return NULL;
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
BS_IDX_T bshypstrs_add_inc(BsHypStrs *pSet, BsHypStr *pObj, BS_IDX_T pInc) {
  return bsdatasettus_add_inc((BsDataSetTus*) pSet, (void*) pObj, pInc);
}
