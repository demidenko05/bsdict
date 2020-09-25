/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ dictionary index temporary words lib.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICIDXIRTRAW
#define BS_DEBUGL_DICIDXIRTRAW 30300

#include "BsDataSet.h"
#include "BsDicIwrds.h"

//Index word (sub-word) should match to no more than BDI_MAX_MATCHED_WORDS, e.g. "ac" matches to 12657 words:
#define BDI_MAX_MATCHED_WORDS 50

/**
 * <p>Base IRT record type.</p>
 * @member unsigned char idx_subwrd_size - size of index sub-word/phrase
 * @member BS_CHAR_T *idx_subwrd - float size index sub-word/phrase
 * @member BS_IDX_T dwolt_start - index (start) in DWOLT, maybe NULL(for i2word), so end will be in the next NON-NULL record or end of dictionary words offset table
 * @member BS_SMALL_T i2wpt_quantity - total matched phrases from 2-nd.. word, 0 if no matches
 **/
#define BSIRTRCDBASE unsigned char idx_subwrd_size; BS_CHAR_T *idx_subwrd; BS_IDX_T dwolt_start; BS_SMALL_T i2wpt_quantity;

/**
 * <p>IRT raw record type.</p>
 * @member BS_IDX_T *i2wpt_dwolt_idx - array[i2wpt_quantity] of indexes in DWOLT matched phrases from 2-nd.. word:
 *          I2WPT consists of pointers (BS_IDX_T) to DWOLT table
 *          i2wpt_end_previous = i2wpt_start_previous + i2wpt_quantity
 *          i2wpt_start = i2wpt_end_previous
 *          for (i=0; i<i2wpt_quantity;i++) I2WPT[i2wpt_start+i] = i2wpt_dwolt_idx[i];
 **/
#define BSIRTRCDRAW BSIRTRCDBASE BS_IDX_T *i2wpt_dwolt_idx;

/**
 * <p>Raw IRT record in memory to make final IRT record</p>
 * @extends BSIRTRCDRAW
 * @features:
 *   idx_subwrd - burn here, will be transferred into final IRT record,
 *                so it must not be free by RAWIRT destructor
 **/
typedef struct {
  BSIRTRCDRAW
} BsDicIdxIrtRawRd;

/**
 * <p>Constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIdxIrtRawRd *bsdicidxirtrawrd_new();

/**
 * <p>Destructor.</p>
 * @param p_irtrcd IRT record
 * @return always NULL
 **/
BsDicIdxIrtRawRd *bsdicidxirtrawrd_free(BsDicIdxIrtRawRd *p_irtrcd);

#define BSDICIDXIRTRAWRCD_NEW_ERR_OUTE(p_als) BsDicIdxIrtRawRd *p_als=bsdicidxirtrawrd_new();\
  if (errno != 0) { BSLOG_ERR goto oute; }

/**
 * <p>IRT in memory.</p>
 * @extends BSDATASET(BsDicIdxIrtRawRd)
 **/
typedef struct { 
  BSDATASET(BsDicIdxIrtRawRd)
} BsDicIdxIrtRaw;

/**
 * <p>Constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIdxIrtRaw *bsdicidxirtraw_new(BS_IDX_T pBufSz);

#define BSDICIDXIRT_NEW_E_RETN(p_als, pBufSz) BsDicIdxIrtRaw *p_als=bsdicidxirtraw_new(pBufSz);\
  if (errno != 0) { BSLOG_ERR return NULL; }

/**
 * <p>Destructor.</p>
 * @param pSet - data set
 * @return always NULL
 **/
BsDicIdxIrtRaw *bsdicidxirtraw_free(BsDicIdxIrtRaw *pSet);

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
BS_IDX_T bsdicidxirtraw_add_inc(BsDicIdxIrtRaw *pSet, BsDicIdxIrtRawRd *pObj, BS_IDX_T pInc);

/**
 * <p>Data to make matched word's offsets.
 * Passed data offsets must be nulled.
 * </p>
 **/
typedef struct {
  BSIRTRCDRAW
  const BS_SMALL_T i2wpt_dwolt_idx_size; //size of i2wpt_dwolt_idx array 
  int matched_total; //must be nulled to 0, it's for performance
  BS_CHAR_T *last_idx_subwrd; //float size last index sub-word with index created
  int last_idx_matched; //0 at very start, matched_total of last index (even of another word), e.g. current "sent" and previous "sen" for "sense" with mathed_total=3
  BS_CHAR_T ispace; //space in AB-coding, initial 0 TODO duplicate AB
} BsDicIdxIrtMchd;

#define BSDICIDXIRTMCHD_CREATE(p_als, p_iwrdsize, p_i2wptdiwoidxsz, p_ispace)  BS_CHAR_T idx_swrd[p_iwrdsize];\
  BS_CHAR_T last_idx_swrd[p_iwrdsize]; last_idx_swrd[0] = 0;\
  BS_IDX_T i2wpt_diwo_ofss[p_i2wptdiwoidxsz];\
  BsDicIdxIrtMchd p_als = { .idx_subwrd_size=0, .idx_subwrd=idx_swrd, .dwolt_start=BS_IDX_NULL,\
    .i2wpt_dwolt_idx_size=p_i2wptdiwoidxsz,\
      .i2wpt_dwolt_idx=i2wpt_diwo_ofss, .i2wpt_quantity=0, .matched_total=0,\
        .last_idx_subwrd=last_idx_swrd, .last_idx_matched=0, .ispace=p_ispace };


/**
 * <p>Bulk setter.</p>
 * @param p_irtrcd IRT record
 * @param p_matched_data data
 * @set errno if error.
 **/
void bsdicidxirtrcd_set_bulk(BsDicIdxIrtRawRd *p_irtrcd, BsDicIdxIrtMchd *p_matched_data);

/**
 * <p>Totals calculated during filling temporary IRT file.
 * Part of it is saved into IDX file, so it's a dynamic thing.
 * </p>
 **/
typedef struct {
  //non-persistent:
  int ab_chars_total; //total chars in alphabet TODO duplicate ab-chrsTot
  //statistic:
    //non-persistent:
  unsigned long alIrWdsSz; //all sizes of all index sub-words in IRT
  //persistent into IDX:
  int mxIrWdSz; //for IRTDR FIXED SIZE maximum length of index sub-word/phrase in IRT
  int hirtSz; //total records in HIRT (distinct first index sub-word's chars)
  BS_IDX_T irtSz; //total records in IRT
  BS_IDX_T i2wptSz; //total records in I2WPT
} BsDicIdxIrtTots;

#define BSDICIDXIRTTOTALS_SIZE 2*sizeof(int) + 2*sizeof(BS_IDX_T)

/**
 * <p>Dynamic constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIdxIrtTots *bsdicidxirttotals_new();

/**
 * <p>Destructor.</p>
 * @param pIrtTots IRT totals or NULL
 * @return always NULL
 **/
BsDicIdxIrtTots *bsdicidxirttotals_free(BsDicIdxIrtTots *pIrtTots);

/**
 * <p>Create IRT in memory from array of sorted i.words in memory.</p>
 * @param p_iwrds - ordered iwords array
 * @param p_i2wrds - ordered i2words array
 * @param pIrtTots IRT totals to make
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIdxIrtRaw *bsdicidxirtraw_create(BsDicIwrds *p_iwrds, BsDicI2wrds *p_i2wrds,
  BsDicIdxIrtTots *pIrtTots);

/**
 * <p>Validate raw IRT in memory.</p>
 * @param p_irtsize IRT size from IRT-TOTALS
 * @set errno if error.
 **/
void bsdicidxirtraw_validate(BsDicIdxIrtRaw *p_irt, BS_IDX_T p_irtsize);
#endif
