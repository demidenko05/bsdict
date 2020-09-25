/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

#include "stdlib.h"

#include "BsLog.h"
#include "BsError.h"
#include "BsDicIwrds.h"

/**
 * <p>Beigesoft™ dictionary I-Words (in AB-coding) orderer(sorter) to make IDX file.</p>
 * @author Yury Demidenko
 **/

/**
 * <p>Constructor.</p>
 * @param p_phrase phrase to copy from
 * @param p_i2wstart I2WORD index start in p_phrase
 * @param p_i2wsize I2WORD size
 * @param p_iphrase_idx index of parent phrase in IWORDS
 * @set errno if error.
 **/
BsDicI2wrdRd *bsdici2wrdrd_new(BS_CHAR_T *p_iphrase, int p_i2wstart, int p_i2wsize,
  BS_IDX_T p_iphrase_idx) {
  BsDicI2wrdRd *obj = malloc(sizeof(BsDicI2wrdRd));
  if (obj != NULL) {
    obj->iword = malloc(p_i2wsize * sizeof(BS_CHAR_T));
    if (obj->iword == NULL) {
      obj = bsdici2wrdrd_free(obj);
    } else {
      int i;
      for (i = 0; i < p_i2wsize - 1; i++) {
        obj->iword[i] = p_iphrase[p_i2wstart + i];
      }
      obj->iword[i] = 0;
      obj->iphrase_idx = p_iphrase_idx;
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
 * @param pRcd I2WORD record
 * @return always NULL
 **/
BsDicI2wrdRd *bsdici2wrdrd_free(BsDicI2wrdRd *pRcd) {
  if (pRcd != NULL) {
    if (pRcd->iword != NULL) {
      free(pRcd->iword);
    }
    free(pRcd);
  }
  return NULL;
}

/**
 * <p>Comparator.</p>
 * @param p_rcd1 I2WORD record1
 * @param p_rcd2 I2WORD record2
 * @return p_rcd1 -1 less 0 equal 1 greater than p_rcd2
 **/
int bsdici2wrdrd_compare(BsDicI2wrdRd *p_rcd1, BsDicI2wrdRd *p_rcd2) {
  return bsdicidx_istr_cmp(p_rcd1->iword, p_rcd2->iword);
}


/**
 * <p>Constructor and filler.</p>
 * @param pBufSz total records
 * @param p_iwrds - whole phrases
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicI2wrds *bsdici2wrds_new_fill(BS_IDX_T pBufSz, BsDicIwrds *p_iwrds) {
  BS_DO_E_RETN(BsDicI2wrds *obj = (BsDicI2wrds *)
    bsdatasettus_new(sizeof(BsDicI2wrds), pBufSz))
  obj->idx_ab = p_iwrds->idx_ab;
  // 1. make 2nd, 3d sub-words:
  BS_CHAR_T *iwords_done[10]; //10 pointers for done i2words from phrase
  BS_IDX_T l;
  for (l = BS_IDX_0; l < p_iwrds->size; l++) {
    for (int iw = 0; iw < 10; iw++) {
      iwords_done[iw] = NULL;
    }
    int sw_start = -1;
    for (int i = 1; ; i++) {
      int is_end_swrd = FALSE;
      if (p_iwrds->vals[l]->iword[i] == 0) {
        if (sw_start > 0) {
          is_end_swrd = TRUE;
        } else {
          break;
        }
      } else if (p_iwrds->vals[l]->iword[i] == p_iwrds->idx_ab->ispace) {
        if (sw_start > 0) {
          is_end_swrd = TRUE;
        } else {
          sw_start = i + 1;
        }
      }
      if (is_end_swrd) {
        //make i2word decord:
        int i2wsz = i - sw_start + 1;
        if (i2wsz - 1 < BSDICI2WORDMINLEN) { //TODO hieroglyphs
          goto out;
        }
        BS_DO_E_OUTE(BsDicI2wrdRd *i2wrdrd = bsdici2wrdrd_new(p_iwrds->vals[l]->iword,
          sw_start, i2wsz, l))
        //rejecting due to duplicating IN CURRENT PHRASE:
        for (int iw = 0; iw < 10; iw++) {
          if (iwords_done[iw] == NULL) {
            break;
          }
          if (bsdicidx_istr_cmp(iwords_done[iw], i2wrdrd->iword) == 0) {
            bsdici2wrdrd_free(i2wrdrd); //reject duplicate
            goto out;
          }
        }
        for (int iw = 1; iw < 10; iw++) {
          if (iwords_done[iw] == NULL) {
            iwords_done[iw] = i2wrdrd->iword;
            break;
          }
        }
        BS_DO_E_OUTE(bsdici2wrds_add_sort_inc(obj, i2wrdrd, BS_IDX_100))
        //reset:
        if (p_iwrds->vals[l]->iword[i] == 0) {
          break;
        }
      out:
        sw_start = i + 1;
      }
    }
  }
  BSLOG_LOG(BSLINFO, "Created i2words: buf.size="BS_IDX_FMT", size="BS_IDX_FMT"\n", obj->bsize, obj->size);
  if (p_iwrds->size < BDI_MAX_IWORDS_TO_AVOIDI2W) {
    for (l = BS_IDX_0; l < obj->size; l++) {
      BS_DO_E_OUTE(BsDicIwrdRd *iwrdrd = bsdiciwrdrd_new(obj->vals[l]->iword, l, 0))
      int iwsz = bsdicidx_istr_len(obj->vals[l]->iword) + 1;
      if (p_iwrds->max_iword_size < iwsz) {
        p_iwrds->max_iword_size = iwsz;
      }
      p_iwrds->all_iwords_size += iwsz;
      BS_DO_E_OUTE(bsdiciwrds_addi2w_sortm_inc(p_iwrds, iwrdrd, BS_IDX_100))
    }
    BSLOG_LOG(BSLINFO, "Added i2words into i.words sorted: bsize="BS_IDX_FMT", size="BS_IDX_FMT", max i.word size=%d, all_iwords_size="BS_IDX_FMT"\n", p_iwrds->bsize, p_iwrds->size, p_iwrds->max_iword_size, p_iwrds->all_iwords_size);
  }
  return obj;
oute:
  bsdici2wrds_free(obj);
  return NULL;
}

/**
 * <p>Destructor WITHOUT transient AB.</p>
 * @param p_srr allocated array of records i-words or NULL
 * @return always NULL
 **/
BsDicI2wrds *bsdici2wrds_free(BsDicI2wrds *pSet) {
  if (pSet != NULL) {
    bsdatasettus_free((BsDataSetTus*) pSet, (Bs_Destruct*) &bsdici2wrdrd_free);
  }
  return NULL;
}

/**
 * <p>Add i.word into array with sorting with duplicates.
 * It increases array if it's full-filled.</p>
 * @param pSet - data set
 * @param pObj - object NON-NULL
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsdici2wrds_add_sort_inc(BsDicI2wrds *pSet, BsDicI2wrdRd *pObj, BS_IDX_T pInc) {
  return bsdatasettus_add_sort_inc((BsDataSetTus*) pSet, (void*)pObj,
    pInc, (Bs_Compare*) &bsdici2wrdrd_compare, true);
}

/**
 * <p>Constructor.</p>
 * @param pIwrdc i.word to  copy
 * @param p_dwofst d.word offset
 * @param p_dwlen d.word length
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIwrdRd*
  bsdiciwrdrd_new (BS_CHAR_T *pIwrdc, BS_FOFST_T p_dwofst,
                BS_SMALL_T p_dwlen)
{
  BsDicIwrdRd *obj = malloc (sizeof (BsDicIwrdRd));

  if ( obj != NULL )
  {
    int iwsz = bsdicidx_istr_len (pIwrdc) + 1;
    obj->iword = malloc (iwsz * sizeof (BS_CHAR_T));

    if ( obj->iword == NULL )
    {
      obj = bsdiciwrdrd_free (obj);
    } else {
      bsdicidx_istr_cpy (obj->iword, pIwrdc);
      obj->offset_dword = p_dwofst;
      obj->length_dword = p_dwlen;
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
 * @param pRcd IWORD record
 * @return always NULL
 **/
BsDicIwrdRd*
  bsdiciwrdrd_free (BsDicIwrdRd *pRcd)
{
  if ( pRcd != NULL )
  {
    if ( pRcd->iword != NULL )
    {
      free (pRcd->iword);
    }
    free (pRcd);
  }
  return NULL;
}

/**
 * <p>Comparator.</p>
 * @param p_rcd1 IWORD record1
 * @param p_rcd2 IWORD record2
 * @return p_rcd1 -1 less 0 equal 1 greater than p_rcd2
 **/
int
  bsdiciwrdrd_compare (BsDicIwrdRd *p_rcd1, BsDicIwrdRd *p_rcd2)
{
  return bsdicidx_istr_cmp (p_rcd1->iword, p_rcd2->iword);
}

/**
 * <p>Comparator match, e.g. sen" equals to "send"!!!
 * But "sen" is greater than "s".</p>
 * @param p_rcd1 IWORD record1
 * @param p_rcd2 IWORD record2
 * @return p_rcd1 -1 less 0 equal 1 greater than p_rcd2
 **/
int
  bsdiciwrdrd_comparem (BsDicIwrdRd *p_rcd1, BsDicIwrdRd *p_rcd2)
{
  return bsdicidx_istr_cmp_match (p_rcd1->iword, p_rcd2->iword);
}

/**
 * <p>Constructor.</p>
 * @param pBufSz total records
 * @param pIdx_ab alphabet
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIwrds*
  bsdiciwrds_new (BS_IDX_T pBufSz, BsDicIdxAb *pIdx_ab)
{
  BS_DO_E_RETN (BsDicIwrds *obj = (BsDicIwrds *)
                bsdatasettus_new (sizeof(BsDicIwrds), pBufSz))
  obj->idx_ab = pIdx_ab;
  obj->max_iword_size = 0;
  obj->all_iwords_size = BS_IDX_0;
  obj->dwoltSz = BS_IDX_0;
  return obj;
}

/**
 * <p>Destructor WITHOUT transient AB.</p>
 * @param p_srr allocated array of records i-words or NULL
 * @return always NULL
 **/
BsDicIwrds*
  bsdiciwrds_free (BsDicIwrds *pSet)
{
  if ( pSet != NULL )
  {
    bsdatasettus_free ((BsDataSetTus*) pSet, (Bs_Destruct*) &bsdiciwrdrd_free);
  }
  return NULL;
}

/**
 * <p>Add i.word into array with sorting without duplicates.
 * It increases array if it's full-filled.</p>
 * @param pSet - data set
 * @param pObj - object NON-NULL
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T
  bsdiciwrds_add_sort_inc (BsDicIwrds *pSet, BsDicIwrdRd *pObj, BS_IDX_T pInc)
{
  return bsdatasettus_add_sort_inc((BsDataSetTus*) pSet, (void*)pObj,
                          pInc, (Bs_Compare*) &bsdiciwrdrd_compare, false);
}

/**
 * <p>Add i2word record into array with sorting without matched duplicates, e.g.
 * "sense" is duplicate for "sense of humor".
 * It increases array if it's full-filled.</p>
 * @param pSet - data set
 * @param pObj - object NON-NULL
 * @param pInc - how much realloc when adding into
 *   full-filled array, if BS_IDX_0 or less then error
 * @return index of added object when OK
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
BS_IDX_T bsdiciwrds_addi2w_sortm_inc(BsDicIwrds *pSet, BsDicIwrdRd *pObj, BS_IDX_T pInc) {
  return bsdatasettus_add_sort_inc((BsDataSetTus*) pSet, (void*)pObj,
    pInc, (Bs_Compare*) &bsdiciwrdrd_comparem, false);
}

/**
 * <p>Type-safe consumer of just read dic.word. It makes sorted i.words array.</p>
 * @param p_dword - pointer to return next word
 * @param p_iwrds - pointer to make sorted i.words array
 * @return 0 to continue iteration, otherwise stop iteration
 * @set errno if error.
 **/
int bsdiciwrds_dwrd_csm(BsDicWord *p_dword, BsDicIwrds *p_iwrds) {
  int dwsz = wcslen(p_dword->word) + 1;
  BS_CHAR_T iwrd[dwsz];
  bsdicidxab_wstr_to_istr(p_dword->word, iwrd, p_iwrds->idx_ab);
  int iwsz = bsdicidx_istr_len(iwrd) + 1;
  if (p_iwrds->max_iword_size < iwsz) {
    p_iwrds->max_iword_size = iwsz;
  }
  p_iwrds->all_iwords_size += iwsz;
  bool is_debug = bslog_is_debug(BS_DEBUGL_DICIWORDS + 80);
  if (is_debug) {
    bslog_log(BSLONLYMSG, "Consuming d.word=%ls, dwofst=%ld, dwlen=%d, dwsz=%d, iwsz=%d", p_dword->word, p_dword->offset, p_dword->lenChrs, dwsz, iwsz);
  }
  BS_DO_E_RETE(BsDicIwrdRd *iwrdrd = bsdiciwrdrd_new(iwrd, p_dword->offset, p_dword->lenChrs))
  if (is_debug) {
    bslog_log(BSLONLYMSG, ", ... created iwrdrd#%p, .iword#%p\n", iwrdrd, iwrdrd->iword);
  }
  BS_DO_E_OUTE(bsdiciwrds_add_sort_inc(p_iwrds, iwrdrd, BS_IDX_100))
  return 0;
oute:
  BSLOG_LOG(BSLERROR, "Error on d.word=%ls\n", p_dword->word)
  bsdiciwrdrd_free(iwrdrd);
  return errno;
}

/**
 * <p>Iterates trough whole dictionary, just read d.word will be consumed
 * by bsdiciwrds_dwrd_csm.</p>
 * @param pDicFl - opened unwound dictionary file
 * @param p_dic_entry_buffer_size - client expected/predicted this max entry size in dictionary
 * @param p_abtots AB totals
 * @param pIdx_ab alphabet
 * @param p_dw_iter_all - pointer to type-unsafe iterator BsDicWord_Iter_Tus
 * @param p_consumer - pointer to type-safe consumer BsDicIwrds_Dwrd_Csm
 * @param p_iwrds_ret - pointer to return sorted i.words array
 * @return BSR_OK_END or consumer's last return value, e.g. BSR_OK_ENOUGH or errno
 * @set errno if error.
 **/
int bsdiciwrds_iter(FILE *pDicFl, int p_dic_entry_buffer_size, BsDicIdxAbTotals *p_abtots, BsDicIdxAb *pIdx_ab,
  BsDicWord_Iter_Tus *p_dw_iter_all, BsDicIwrds_Dwrd_Csm p_consumer, BsDicIwrds **p_iwrds_ret) {
  BS_DO_E_RETE(BsDicIwrds *iwrds = bsdiciwrds_new(p_abtots->dwoltSz, pIdx_ab))
  *p_iwrds_ret = iwrds;
  BS_DO_E_RETE(int ret = p_dw_iter_all(pDicFl, p_dic_entry_buffer_size,
    (BsDicWord_Consume_Tus*) p_consumer, (void*) iwrds))
  BSLOG_LOG(BSLINFO, "Created i.words: buf.size="BS_IDX_FMT", size="BS_IDX_FMT", max i.word size=%d, all_iwords_size="BS_IDX_FMT", chars=%d\n", iwrds->bsize, iwrds->size, iwrds->max_iword_size, iwrds->all_iwords_size, iwrds->idx_ab->chrsTot);
  iwrds->dwoltSz = iwrds->size;
  return ret;
}

/**
 * <p>Basic method. Find index of the first sorted i.word matched to given one.</p>
 * @param p_iword_- i.word to match
 * @param p_iwrds_sort - sorted i.words array
 * @return index if found, BS_IDX_NULL if not found.
 * @set errno if error.
 **/
BS_IDX_T bsdiciwrdsbs_find_first(BS_CHAR_T *p_iword, BsDicIwrdsBs *p_iwrds) {
  bool isdbg = bslog_is_debug(BS_DEBUGL_DICIWORDS);
  if (p_iwrds->size == BS_IDX_0) {
    return BS_IDX_NULL;
  }
  BS_IDX_T idx_start = BS_IDX_0;
  BS_IDX_T idx_end = p_iwrds->size - BS_IDX_1;
  BS_IDX_T idx_middle;
  if (idx_start == idx_end) {
    idx_middle = idx_start;
  } else {
    idx_middle = idx_start + (idx_end - idx_start) / BS_IDX_2;
  }
  BS_IDX_T idx_equal = BS_IDX_NULL;
  if (isdbg) {
    BS_WCHAR_T wwrd[bsdicidx_istr_len(p_iword) + 1];
    bsdicidxab_istr_to_wstr(p_iword, wwrd, p_iwrds->idx_ab);
    BSLOG_LOG(BSLDEBUG, "wrd=%ls, iwrdssort=%p size="BS_IDX_FMT" \n", wwrd, p_iwrds, p_iwrds->size);
  }
  while (true) {
    int idx_start_was_eq_idx_end = (idx_start == idx_end);
    int cmpr = bsdicidx_istr_cmp_match(p_iword, p_iwrds->vals[idx_middle]->iword);
    if (cmpr <= 0) { 
      if (cmpr == 0) {
        idx_equal = idx_middle;
      }
      if (idx_middle == BS_IDX_0) {
        break;
      }
      if (!idx_start_was_eq_idx_end) { idx_end = idx_middle - BS_IDX_1; }
    } else {
      if (idx_middle + BS_IDX_1 == p_iwrds->size) {
        break;
      }
      if (!idx_start_was_eq_idx_end) { idx_start = idx_middle + BS_IDX_1; }
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
  if (isdbg) {
    BSLOG_LOG(BSLONLYMSG, "i[2]wrds=%p idx equal="BS_IDX_FMT"\n", p_iwrds, idx_equal);
  }
  return idx_equal;
}

/**
 * <p>Type-safe wrapper. Find index of the first sorted i.word matched to given one.</p>
 * @param p_iword_- i.word to match
 * @param p_iwrds - sorted i.words array
 * @return index if found, BS_IDX_NULL if not found.
 * @set errno if error.
 **/
BS_IDX_T bsdiciwrds_find_first(BS_CHAR_T *p_iword, BsDicIwrds *p_iwrds) {
  return bsdiciwrdsbs_find_first(p_iword, (BsDicIwrdsBs*) p_iwrds);
}

/**
 * <p>Type-safe wrapper. Find index of the first sorted i.2word matched to given one.</p>
 * @param p_iword_- i.word to match
 * @param p_i2wrds - sorted i.2words array
 * @return index if found, BS_IDX_NULL if not found.
 * @set errno if error.
 **/
BS_IDX_T bsdici2wrds_find_first(BS_CHAR_T *p_iword, BsDicI2wrds *p_i2wrds) {
  return bsdiciwrdsbs_find_first(p_iword, (BsDicIwrdsBs*) p_i2wrds);
}

