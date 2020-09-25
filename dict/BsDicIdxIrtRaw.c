/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

#include "string.h"
#include "stdlib.h"

#include "BsLog.h"
#include "BsError.h"
#include "BsDicIdxIrtRaw.h"

/**
 * <p>Beigesoft™ dictionary index temporary words lib.</p>
 * @author Yury Demidenko
 **/

/**
 * <p>Only dynamic constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIdxIrtTots *bsdicidxirttotals_new() {
  BsDicIdxIrtTots *obj = malloc(sizeof(BsDicIdxIrtTots));
  if (obj != NULL) {
    obj->ab_chars_total = 0;
    obj->mxIrWdSz = 0;
    obj->alIrWdsSz = 0L;
    obj->irtSz = BS_IDX_0;
    obj->hirtSz = 0;
    obj->i2wptSz = BS_IDX_0;
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor.</p>
 * @param pIrtTots IRT totals
 * @return always NULL
 **/
BsDicIdxIrtTots *bsdicidxirttotals_free(BsDicIdxIrtTots *pIrtTots) {
  if (pIrtTots != NULL) {
    free(pIrtTots);
  }
  return NULL;
}

/**
 * <p>Find matched words offsets in given array of sorted i.words in memory.</p>
 * @param p_iwrdssort - ordered i.words array
 * @param p_matched_data pointer to nulled matched data to return
 * @return 0, BSR_OK_ENOUGH or any error, it can return up to BDI_MAX_MATCHED_WORDS+1 matched word's offsets
 * @set errno if error.
 **/
int bsdicidxirttst_iwrds_match(BsDicIwrds *p_iwrds, BsDicIdxIrtMchd *p_matched_data) {
  BS_DO_E_RETE(BS_IDX_T first_idx = bsdiciwrds_find_first(p_matched_data->idx_subwrd, p_iwrds))
  if (first_idx == BS_IDX_NULL) {
    return 0;
  }
  int rez = 0;
  p_matched_data->matched_total++;
  if (p_iwrds->vals[first_idx]->length_dword == 0) { //TODO is there another matched iword?
    return rez; //this is i2wrd
  }
  for (BS_IDX_T l = first_idx + BS_IDX_1; l < p_iwrds->size; l++) {
    // 2. try to match:
    EBsWrdMatch emch = bsdicidx_istr_match_from_start(p_matched_data->idx_subwrd, p_iwrds->vals[l]->iword);
    if (emch == EWMATCHFROMSTART) {
      p_matched_data->matched_total++;
      if (p_matched_data->matched_total > BDI_MAX_MATCHED_WORDS) {
        rez = BSR_OK_ENOUGH;
        break;
      }
    } else {
      break;
    }
  }
  return rez;
}

/**
 * <p>Find matched words offsets in given array of sorted i.words in memory.</p>
 * @param p_i2wrds - ordered i.2words array
 * @param p_matched_data pointer to nulled matched data to return
 * @return 0, BSR_OK_ENOUGH or any error, it can return up to BDI_MAX_MATCHED_WORDS+1 matched word's offsets
 * @set errno if error.
 * */
int bsdicidxirttst_i2wrds_match(BsDicI2wrds *p_i2wrds, BsDicIdxIrtMchd *p_matched_data) {
  BS_DO_E_RETE(BS_IDX_T first_idx = bsdici2wrds_find_first(p_matched_data->idx_subwrd, p_i2wrds))
  if (first_idx == BS_IDX_NULL) {
    return 0;
  }
  int rez = 0;
  p_matched_data->i2wpt_dwolt_idx[p_matched_data->i2wpt_quantity] = p_i2wrds->vals[first_idx]->iphrase_idx;
  p_matched_data->i2wpt_quantity++;
  p_matched_data->matched_total++;
  for (BS_IDX_T l = first_idx + BS_IDX_1; l < p_i2wrds->size; l++) {
    // 2. try to match:
    EBsWrdMatch emch = bsdicidx_istr_match_from_start(p_matched_data->idx_subwrd, p_i2wrds->vals[l]->iword);
    if (emch == EWMATCHFROMSTART) {
      if (p_matched_data->i2wpt_quantity + 1 > p_matched_data->i2wpt_dwolt_idx_size) {
        errno = BSE_ARR_OUT_OF_BOUNDS;
        BSLOG_ERR
        return errno;
      } else {
        p_matched_data->i2wpt_dwolt_idx[p_matched_data->i2wpt_quantity] = p_i2wrds->vals[l]->iphrase_idx;
        p_matched_data->i2wpt_quantity++;
        p_matched_data->matched_total++;
      }
      if (p_matched_data->matched_total > BDI_MAX_MATCHED_WORDS) {
        rez = BSR_OK_ENOUGH;
        break;
      }
    } else {
      break;
    }
  }
  return rez;
}

  //Type-safe wrappers:
/**
 * <p>Constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIdxIrtRaw *bsdicidxirtraw_new(BS_IDX_T pBufSz) {
  return (BsDicIdxIrtRaw*) bsdatasettus_new(sizeof(BsDicIdxIrtRaw), pBufSz);
}

/**
 * <p>Destructor.</p>
 * @param pSet - data set
 * @return always NULL
 **/
BsDicIdxIrtRaw *bsdicidxirtraw_free(BsDicIdxIrtRaw *pSet) {
  if (pSet != NULL) {
    bsdatasettus_free((BsDataSetTus*) pSet, (Bs_Destruct*) &bsdicidxirtrawrd_free);
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
BS_IDX_T bsdicidxirtraw_add_inc(BsDicIdxIrtRaw *pSet, BsDicIdxIrtRawRd *pObj, BS_IDX_T pInc) {
  return bsdatasettus_add_inc((BsDataSetTus*) pSet, (void*) pObj, pInc);
}

/**
 * <p>Constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIdxIrtRawRd *bsdicidxirtrawrd_new() {
  BsDicIdxIrtRawRd *obj = malloc(sizeof(BsDicIdxIrtRawRd));
  if (obj != NULL) {
    obj->idx_subwrd = NULL;
    obj->idx_subwrd_size = 0;
    obj->i2wpt_dwolt_idx = NULL;
    obj->dwolt_start = BS_IDX_NULL;
    obj->i2wpt_quantity = 0;
  }
  if (obj == NULL) {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor.</p>
 * @param p_irtrcd IRT record
 * @return always NULL
 **/
BsDicIdxIrtRawRd *bsdicidxirtrawrd_free(BsDicIdxIrtRawRd *p_irtrcd) {
  if (p_irtrcd != NULL) {
    //p_irtrcd->idx_subwrd will be transferred into IRT, then nulled
    if (p_irtrcd->idx_subwrd != NULL) { //this is in case of tests, i.e. without transferring into IRT
      free(p_irtrcd->idx_subwrd);
    }
    if (p_irtrcd->i2wpt_dwolt_idx != NULL) {
      free(p_irtrcd->i2wpt_dwolt_idx);
    }
    free(p_irtrcd);
  }
  return NULL;
}

/**
 * <p>Bulk setter.</p>
 * @param p_irtrcd IRT record
 * @param p_matched_data data
 * @set errno if error.
 **/
void bsdicidxirtrcd_set_bulk(BsDicIdxIrtRawRd *p_irtrcd, BsDicIdxIrtMchd *p_matched_data) {
  if (p_matched_data->idx_subwrd_size > UCHAR_MAX) {
    errno = BSE_INTEGER_OVERFLOW;
    BSLOG_ERR
    return;
  }
  p_irtrcd->idx_subwrd_size = p_matched_data->idx_subwrd_size;
  p_irtrcd->idx_subwrd = malloc(p_matched_data->idx_subwrd_size * BS_CHAR_LEN);
  if (p_irtrcd->idx_subwrd == NULL) {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
    return;
  }
  bsdicidx_istr_cpy(p_irtrcd->idx_subwrd, p_matched_data->idx_subwrd);
  if (p_matched_data->i2wpt_quantity > 0) {
    p_irtrcd->i2wpt_dwolt_idx = malloc(p_matched_data->i2wpt_quantity * BS_IDX_LEN);
    if (p_irtrcd->i2wpt_dwolt_idx == NULL) {
      free(p_irtrcd->idx_subwrd);
      if ( errno == 0 ) { errno = ENOMEM; }
      BSLOG_ERR
      return;
    }
    for (int i = 0; i < p_matched_data->i2wpt_quantity; i++) {
      p_irtrcd->i2wpt_dwolt_idx[i] = p_matched_data->i2wpt_dwolt_idx[i];
    }
  }
  p_irtrcd->dwolt_start = p_matched_data->dwolt_start;
  p_irtrcd->i2wpt_quantity = p_matched_data->i2wpt_quantity;
}

/**
 * <p>Create IRT in memory from array of sorted i.words in memory.</p>
 * @param p_iwrds - ordered iwords array
 * @param p_i2wrds - ordered i2words array
 * @param pIrtTots IRT totals to make
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIdxIrtRaw *bsdicidxirtraw_create(BsDicIwrds *p_iwrds, BsDicI2wrds *p_i2wrds,
  BsDicIdxIrtTots *pIrtTots) {
  BS_IDX_T irtsz0 = p_iwrds->size / BS_IDX_10; //usually less than 10%
  if (irtsz0 == BS_IDX_0) { irtsz0 = BS_IDX_1; }
  BS_IDX_T irtinc = p_iwrds->size / BS_IDX_100;
  if (irtinc == BS_IDX_0) { irtinc = BS_IDX_1; }
  BSDICIDXIRT_NEW_E_RETN(irt, irtsz0)
  int rez = 0;
  BS_CHAR_T first_chars[p_iwrds->idx_ab->chrsTot];
  int i;
  for (i = 0; i < p_iwrds->idx_ab->chrsTot; i++) {
    first_chars[i] = 0;
  }
#ifdef BS_USE_LOG_DETAIL1
#define BSULD1_CHR_CNT 100
  int dbgFlr = bslog_get_debug_floor();
  int dbgClg = bslog_get_debug_ceiling();
  BS_WCHAR_T wrd[BSULD1_CHR_CNT];
#endif
  int idx_wrd_size = p_iwrds->max_iword_size + 1;
  BS_SMALL_T i2wpt_dwolt_idx_sz = BDI_MAX_MATCHED_WORDS; //this temporary data isn't too big
  BSDICIDXIRTMCHD_CREATE(matched_data, idx_wrd_size, i2wpt_dwolt_idx_sz, p_iwrds->idx_ab->ispace)
  BS_IDX_T l, irtIdx, prev_i2wrds_count = BS_IDX_0;
  for (l = BS_IDX_0; l < p_iwrds->size; l++) { //without duplicates
    //1.1 process word's all or up to triggered rule letters:
    int idxw_lenm1 = bsdicidx_istr_len(p_iwrds->vals[l]->iword) - 1;
    for (int j = 0; j <= idxw_lenm1; j++) {
      //a)nulling m.data:
      int k;
      for (k = 0; k <= j; k++) {
        matched_data.idx_subwrd[k] = p_iwrds->vals[l]->iword[k];
      }
      if (matched_data.idx_subwrd[k-1] == p_iwrds->idx_ab->ispace) {
        continue;
      }
      matched_data.idx_subwrd[k] = 0;
      matched_data.idx_subwrd_size = k + 1;
      for (k = 0; k < i2wpt_dwolt_idx_sz; k++) {
        matched_data.i2wpt_dwolt_idx[k] = BS_IDX_NULL;
      }
      matched_data.dwolt_start = BS_IDX_NULL;
      matched_data.i2wpt_quantity = 0;
      matched_data.matched_total = 0;
      //b)try to match:
      BS_DO_E_OUTE(rez = bsdicidxirttst_iwrds_match(p_iwrds, &matched_data))
#ifdef BS_USE_LOG_DETAIL1
      int chrdbgl = BS_DEBUGL_DICIDXIRTRAW + p_iwrds->vals[l]->iword[0];
      if (chrdbgl >=dbgFlr && chrdbgl<= dbgClg) {
        if (j == 0) {
          bsdicidxab_istrn_to_wstr(p_iwrds->vals[l]->iword, wrd, BSULD1_CHR_CNT, p_iwrds->idx_ab); 
          bslog_log(BSLONLYMSG, "#"BS_IDX_FMT" %ls:\n", l, wrd);
        }
        bsdicidxab_istrn_to_wstr(matched_data.idx_subwrd, wrd, BSULD1_CHR_CNT, p_iwrds->idx_ab); 
        bslog_log(BSLONLYMSG, "  #%d %ls mt=%d", j, wrd, matched_data.matched_total);
        bsdicidxab_istrn_to_wstr(matched_data.last_idx_subwrd, wrd, BSULD1_CHR_CNT, p_iwrds->idx_ab); 
        bslog_log(BSLONLYMSG, " / last %ls lmt=%d\n", wrd, matched_data.last_idx_matched);
      }
#endif
      if (p_i2wrds != NULL && rez == 0) {
        BS_DO_E_OUTE(bsdicidxirttst_i2wrds_match(p_i2wrds, &matched_data))
      }
      if (matched_data.matched_total > 0) {
        int cmpr = bsdicidx_istr_cmp_match(matched_data.last_idx_subwrd, matched_data.idx_subwrd);
        if (matched_data.matched_total > BDI_MAX_MATCHED_WORDS && j < idxw_lenm1) {
          continue;
        } else if (matched_data.last_idx_matched > 0 && (cmpr >= 0)
          && matched_data.last_idx_matched <= BDI_MAX_MATCHED_WORDS) {
          //matched_data.last_idx_matched > BDI_MAX_MATCHED_WORDS - previous was a short word e.g "a" "b"
          //previous word with same start index already done 
          break;
        }
        BSDICIDXIRTRAWRCD_NEW_ERR_OUTE(irtrcd)
        if (p_iwrds->vals[l]->length_dword == 0) {
          prev_i2wrds_count++;
        } else {
          matched_data.dwolt_start = l - prev_i2wrds_count;
        }
        BS_DO_E_OUTE(irtIdx = bsdicidxirtraw_add_inc(irt, irtrcd, irtinc))
        BS_DO_E_OUTE(bsdicidxirtrcd_set_bulk(irtrcd, &matched_data))
#ifdef BS_USE_LOG_DETAIL1
        if (chrdbgl >=dbgFlr && chrdbgl<= dbgClg) {
          bslog_log(BSLONLYMSG, "    created IRT IDX="BS_IDX_FMT" dwolt_start="BS_IDX_FMT" i2wpt_quantity=%d\n", irtIdx, matched_data.dwolt_start, matched_data.i2wpt_quantity);
        }
#endif
        //save last index data:
        matched_data.last_idx_matched = matched_data.matched_total;
        bsdicidx_istr_cpy(matched_data.last_idx_subwrd, matched_data.idx_subwrd);
        if (matched_data.idx_subwrd_size > pIrtTots->mxIrWdSz) {
          pIrtTots->mxIrWdSz = matched_data.idx_subwrd_size;
        }
        pIrtTots->alIrWdsSz += matched_data.idx_subwrd_size;
        pIrtTots->irtSz++;
        pIrtTots->i2wptSz += matched_data.i2wpt_quantity;
        int is_fch_added = FALSE;
        for (i = 0; i < pIrtTots->ab_chars_total; i++) {
          if (first_chars[i] == matched_data.idx_subwrd[0]) {
            is_fch_added = TRUE;
            break;
          }
        }
        if (!is_fch_added) {
          for (i = 0; i < pIrtTots->ab_chars_total; i++) {
            if (first_chars[i] == 0) {
              first_chars[i] = matched_data.idx_subwrd[0];
              break;
            }
          }
          pIrtTots->hirtSz++;
        }
      }
    }
  }
  BSLOG_LOG(BSLINFO, "Created IRTRAW: hirtSz=%d, mxIrWdSz=%d, alIrWdsSz="BS_IDX_FMT", irtSz="BS_IDX_FMT", i2wptSz="BS_IDX_FMT"\n", pIrtTots->hirtSz, pIrtTots->mxIrWdSz, pIrtTots->alIrWdsSz, pIrtTots->irtSz, pIrtTots->i2wptSz);
  return irt;
oute:
  bsdicidxirtraw_free(irt);
  return NULL;
}

/**
 * <p>Validate raw IRT in memory.</p>
 * @param p_irt IRT
 * @param p_irtsize IRT size from IRT-TOTALS
 * @set errno if error.
 **/
void bsdicidxirtraw_validate(BsDicIdxIrtRaw *p_irt, BS_IDX_T p_irtsize) {
  if (p_irt == NULL) {
    errno = BSE_VALIDATE_ERR;
    BSLOG_LOG(BSLERROR, "IRT == NULL\n")
    return;
  }
  if (p_irt->bsize < p_irtsize) {
    errno = BSE_VALIDATE_ERR;
    BSLOG_LOG(BSLERROR, "IRT-size="BS_IDX_FMT" < IRT-TOTALS size="BS_IDX_FMT"\n", p_irt->bsize, p_irtsize)
    return;
  }
  for (BS_IDX_T l = BS_IDX_0; l < p_irtsize; l++) {
    if (p_irt->vals[l] == NULL) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "IRT has NULL on l="BS_IDX_FMT", size="BS_IDX_FMT"\n", l, p_irtsize)
      return;
    }
    if (p_irt->vals[l] != NULL && p_irt->vals[l]->idx_subwrd == NULL) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "IRT subword NULL, l="BS_IDX_FMT", size="BS_IDX_FMT"\n", l, p_irtsize)
      return;
    }
    if (p_irt->vals[l]->dwolt_start == BS_IDX_NULL && p_irt->vals[l]->i2wpt_quantity <= 0) {
      errno = BSE_VALIDATE_ERR;
      BSLOG_LOG(BSLERROR, "irtraw["BS_IDX_FMT"]-dwolt_start == NULL && i2wpt_quantity <= 0\n", l)
      return;
    }
    if (p_irt->vals[l]->i2wpt_quantity > 0) {
      if (p_irt->vals[l]->i2wpt_dwolt_idx == NULL) {
        errno = BSE_VALIDATE_ERR;
        BSLOG_LOG(BSLERROR, "IRT i2wpt_dwolt_idx NULL, i2wpt_quantity=%d, l="BS_IDX_FMT", size="BS_IDX_FMT"\n", p_irt->vals[l]->i2wpt_quantity, l, p_irt->bsize)
        return;
      }
      for (int i = 0; i < p_irt->vals[l]->i2wpt_quantity; i++) {
        if(p_irt->vals[l]->i2wpt_dwolt_idx[i] == BS_IDX_NULL) {
          errno = BSE_VALIDATE_ERR;
          BSLOG_LOG(BSLERROR, "IRT i2wpt_dwolt_idx[i] NULL!, i2wpt_quantity=%d, i=%d, l="BS_IDX_FMT", size="BS_IDX_FMT"\n", p_irt->vals[l]->i2wpt_quantity, i, l, p_irt->bsize)
          return;
        }
      }
    }
  }
  BSLOG_LOG(BSLINFO, "IRTRAW#%p has been successfully validated!\n", p_irt);
}
