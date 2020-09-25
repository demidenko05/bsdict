/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

#include "stdlib.h"
#include "wctype.h"

#include "BsError.h"
#include "BsLog.h"
#include "BsDicIdxAb.h"
#include "BsDicWordDsl.h"

/**
 * <p>Beigesoft™ dictionary index wchar based alphabet library.</p>
 * @author Yury Demidenko
 **/

//Public methods:
/**
 * <p>
 * Create new alphabet, allocate wchars, add space.
 * </p>
 * @param p_buffer_sz - real or predicted AB size
 * @return pointer to index alphabet or NULL
 * @set errno if error.
 **/
BsDicIdxAb *bsdicidxab_new(int p_buffer_sz) {
  if (p_buffer_sz < 2) {
    errno = BSE_ARR_WPSIZE;
    BSLOG_LOG(BSLERROR, "SIZE wrong %d", p_buffer_sz);
    return NULL;
  }
  BsDicIdxAb *obj = malloc(sizeof(BsDicIdxAb));
  if (obj != NULL) {
    obj->transient_buffer_len = p_buffer_sz;
    obj->wchars = malloc(obj->transient_buffer_len * BS_WCHAR_LEN);
    if (obj->wchars == NULL) {
      free(obj);
      obj = NULL;
    } else {
      for (int i = 0; i < obj->transient_buffer_len; i++) {
        obj->wchars[i] = 0;
      }
      obj->wchars[0] = L' ';
      obj->chrsTot = 1;
      obj->ispace = 1;
    }
  }
  if (obj == NULL) {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>
 * Destroy alphabet.
 * </p>
 * @param pIdx_ab pointer to index alphabet
 * @return always NULL
 **/
BsDicIdxAb *bsdicidxab_free(BsDicIdxAb *pIdx_ab) {
  if (pIdx_ab != NULL) {
    if (pIdx_ab->wchars != NULL)  {
      free(pIdx_ab->wchars);
    }
    free(pIdx_ab);
  }
  return NULL;
}

/**
 * <p>Type-safe consumer. Fill AB by all words from dictionary.
 * @param p_dword just read d.word
 * @param p_csm_inst instrument to consume
 * @return 0 to continue iteration, otherwise stop iteration
 * @set errno if error.
 * </p>
 **/
int bsdicidxab_dwrd_consume_fill(BsDicWord *p_dword, BsDicIdxAbFill *p_csm_inst) {
  BS_DO_E_RETE(bsdicidxab_add_wstr(p_dword->word, p_csm_inst->idx_ab))
  bsdicidxab_wstr_to_istr(p_dword->word, p_csm_inst->idxstr, p_csm_inst->idx_ab);
  int isl = bsdicidx_istr_len(p_csm_inst->idxstr);
  if (isl > p_csm_inst->totals->max_iword_len) {
    p_csm_inst->totals->max_iword_len = isl;
  }
  int sw_start = -1;
  for (int i = 1; ; i++) {
    int is_end_swrd = FALSE;
    if (p_csm_inst->idxstr[i] == 0) {
      if (sw_start > 0) {
        is_end_swrd = TRUE;
      } else {
        break;
      }
    } else if (p_csm_inst->idxstr[i] == p_csm_inst->idx_ab->ispace) {
      if (sw_start > 0) {
        is_end_swrd = TRUE;
      } else {
        sw_start = i + 1;
      }
    }
    if (is_end_swrd) {
      p_csm_inst->totals->i2wptSz++;
      if (p_csm_inst->idxstr[i] == 0) {
        break;
      }
      sw_start = i + 1;
    }
  }
  p_csm_inst->totals->dwoltSz++;
  return 0;
}

/**
 * <p>
 * Type-safe iterator trough whole DSL dictionary, just read dic.word will be consumed
 * by given consumer, if consumer returns an error, then iteration will stop.
 * </p>
 * @param pDicFl - opened unwound dictionary file
 * @param p_dic_entry_buffer_size - client expected/predicted this max entry size in dictionary
 * @param pIdx_ab pointer to index alphabet
 * @param p_totals pointer to return totals
 * @return BSR_OK_END or consumer's last return value, e.g. BSR_OK_ENOUGH
 * @set errno if error.
 **/
int bsdicidxab_iter_dsl_fill(FILE *pDicFl, int p_dic_entry_buffer_size, BsDicIdxAb *pIdx_ab, BsDicIdxAbTotals *p_totals) {
  BS_CHAR_T istr[p_dic_entry_buffer_size]; //max index usually less than original maximum word/phrase, but this wasting memory is OK
  BsDicIdxAbFill instr = { .idx_ab=pIdx_ab, .totals=p_totals, .idxstr=istr };
  int rez = bsdicworddsl_iter_tus(pDicFl, p_dic_entry_buffer_size, (BsDicWord_Consume_Tus*) bsdicidxab_dwrd_consume_fill, (void*) &instr);
  BSLOG_LOG(BSLINFO, "Created AB chars total=%d, ispace=%d, max_iword_len=%d, dwoltSz=%lu, i2wptSz=%lu, wchars:\n", pIdx_ab->chrsTot, pIdx_ab->ispace, p_totals->max_iword_len, p_totals->dwoltSz, p_totals->i2wptSz);
  for (int k = 0; k < pIdx_ab->chrsTot; k++) {
    bslog_log(BSLONLYMSG, " [%d / %d]", k+1, pIdx_ab->wchars[k]);
  }
  bslog_log(BSLONLYMSG, "\n");
  return rez;
}

/**
 * <p>
 * Fills alphabet with wide char string.
 * Wide chars will be converted to lower case, to spaces=hyphen=space.
 * Index's alphabet consists of lower case chars plus space and "'".
 * All other chars, e.g. digits, comma... will be rejected.
 * "its" and "it's" are two different indexes.
 * Index word never start with "'" or " ", e.g. "'twere".
 * Adding wchar will be in ordered way.
 * @param p_wstr string
 * @param pIdx_ab index alphabet to make, at start must be initialized with two first wchars - space and "'"
 * @set errno if error.
 * </p>
 **/
void bsdicidxab_add_wstr(BS_WCHAR_T *p_wstr, BsDicIdxAb *pIdx_ab) {
  for (int i = 0; ; i++) {
    BS_WCHAR_T nwch;
    if (p_wstr[i] == 0) {
      break;
    } else if (iswalpha(p_wstr[i])) {
      nwch = towlower(p_wstr[i]);
    } else { // any char:
      nwch = p_wstr[i];
    }
    int is_fnd = FALSE;
    int lt_idx = 0; //last less idx
    for (int i = 0; i < pIdx_ab->chrsTot; i++) {
      if (nwch == pIdx_ab->wchars[i]) {
        is_fnd = TRUE;
        break;
      } else if (pIdx_ab->wchars[i] < nwch) {
        lt_idx = i;
      }
    }
    if (!is_fnd) {
      if (pIdx_ab->chrsTot >= pIdx_ab->transient_buffer_len) {
        pIdx_ab->transient_buffer_len += BDI_AB_BUF_INCREASE;
        pIdx_ab->wchars = realloc(pIdx_ab->wchars, pIdx_ab->transient_buffer_len * BS_WCHAR_LEN);
        if (pIdx_ab->wchars == NULL) {
          errno = ENOMEM;
          BSLOG_ERR
          return;
        }
        for (int i = pIdx_ab->chrsTot; i < pIdx_ab->transient_buffer_len; i++) {
          pIdx_ab->wchars[i] = 0;
        }
      }
      int nwch_idx = -1;
      if (lt_idx == pIdx_ab->chrsTot - 1) { //into last
        nwch_idx = pIdx_ab->chrsTot;
      } else { //insert after lt_idx:
        for (int i = pIdx_ab->chrsTot; i > lt_idx + 1; i--) {
          pIdx_ab->wchars[i] = pIdx_ab->wchars[i - 1];
        }
        nwch_idx = lt_idx + 1;
      }
      pIdx_ab->wchars[nwch_idx] = nwch;
      pIdx_ab->chrsTot++;
    }
  }
  for (int i = 0; i < pIdx_ab->chrsTot; i++) {
    if (pIdx_ab->wchars[i] == L' ') {
      pIdx_ab->ispace = i + 1;
    }
  }
}

/**
 * <p>
 * Converts wide chars string into index's chars.
 * Wide chars will be converted to lower case, 3spaces=2spaces=1space.
 * @param p_wstr w.string
 * @param p_istr result full string in index alphabet for further indexing
 * @param pIdx_ab index alphabet
 * </p>
 **/
void bsdicidxab_wstr_to_istr(BS_WCHAR_T *p_wstr, BS_CHAR_T *p_istr, BsDicIdxAb *pIdx_ab) {
  int idx_str_len = 0;
  int was_space = FALSE;
  for (int i = 0; ; i++) {
    BS_WCHAR_T nwch;
    if (p_wstr[i] == 0) {
      break;
    } else if (iswalpha(p_wstr[i])) {
      nwch = towlower(p_wstr[i]);
    } else {
      nwch = p_wstr[i];
    }
    for (int j = 0; j < pIdx_ab->chrsTot; j++) {
      if (nwch == pIdx_ab->wchars[j]) {
        if (nwch == pIdx_ab->ispace) { //TODO hyphen between words (iswalpha), e.g. word (---) is actually a sign
          if (was_space) {
            break;
          }
          was_space = TRUE;
        } else {
          was_space = FALSE;
        }
        p_istr[idx_str_len] = j + 1;
        idx_str_len++;
        break;
      }
    }
  }
  p_istr[idx_str_len] = 0;
}

/**
 * <p>
 * Converts index's chars string into wide chars.
 * It's for testing bsdicidxab_wstr_to_istr
 * @param p_istr string in index alphabet
 * @param p_wstr w.string result
 * @param pIdx_ab index alphabet
 * </p>
 **/
void bsdicidxab_istr_to_wstr(BS_CHAR_T *p_istr, BS_WCHAR_T *p_wstr, BsDicIdxAb *pIdx_ab) {
  int wstr_len = 0;
  for (int i = 0; ; i++) {
    if (p_istr[i] == 0) {
      break;
    } else {
        p_wstr[wstr_len] = pIdx_ab->wchars[p_istr[i] - 1];
        wstr_len++;
    }
  }
  p_wstr[wstr_len] = 0;
}

/**
 * <p>
 * Converts index's chars string up to given chars count into wide chars.
 * It's for testing bsdicidxab_wstr_to_istr
 * @param p_istr string in index alphabet
 * @param p_wstr w.string result
 * @param pCnt maximum chars to convert
 * @param pIdx_ab index alphabet
 * </p>
 **/
void bsdicidxab_istrn_to_wstr(BS_CHAR_T *p_istr, BS_WCHAR_T *p_wstr, int pCnt, BsDicIdxAb *pIdx_ab) {
  int wstr_len = 0;
  for (int i = 0; i < pCnt; i++) {
    if (p_istr[i] == 0) {
      break;
    } else {
        p_wstr[wstr_len] = pIdx_ab->wchars[p_istr[i] - 1];
        wstr_len++;
    }
  }
  p_wstr[wstr_len] = 0;
}


/**
 * <p>Сonvert user's word to AB coding, it maybe empty string.</p>
 * @param pWrd - word from
 * @param pIwrd - i.word to
 * @param pAb - alphabet
 * @set errno if error.
 **/
void
  bsdicidxab_str_to_istr (char *pWrd, BS_CHAR_T *pIwrd, BsDicIdxAb *pAb)
{
  int len = strlen (pWrd);
  BS_WCHAR_T dsubwrd[len + 5]; //small buffer cause SEGF!
  int i = mbstowcs (dsubwrd, pWrd, len + 1);
  if ( i <= 0 || errno != 0 )
  {
    if ( errno == 0 ) { errno = BSE_ERR; }
    BSLOG_LOG (BSLERROR, "mbstowcs fail on %s, rz=%d\n", pWrd, i)
    return;
  }
  bsdicidxab_wstr_to_istr (dsubwrd, pIwrd, pAb);
}
