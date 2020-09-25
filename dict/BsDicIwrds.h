/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
/**
 * <p>Beigesoft™ dictionary I-Words (in AB-coding) orderer(sorter) to make IDX files.
 * DIWO or i[2]wordssorted are synonyms.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICIWORDS
#define BS_DEBUGL_DICIWORDS 30100

#include "BsDataSet.h"
#include "BsDicIdxAb.h"

//max iwords to avoid adding i2words into iwords, small means switch off adding i2wordssorted into iwordssorted
#define BDI_MAX_IWORDS_TO_AVOIDI2W 9
//Data types:

/**
 * <p>DWOLT record type.</p>
 * @member BS_FOFST_T offset_dword - origin word in dictionary file
 * @member BS_SMALL_T length_dword -length of origin word in dictionary file without terminator
 **/
#define BSDICDWOLTRCDBASE BS_FOFST_T offset_dword; BS_SMALL_T length_dword;

/**
 * <p>Base record with I-(2)WORD in AB-coding basic type.</p>
 * @member BS_CHAR_T *iword - i2/3/...word or whole phrase float size with terminator 0
 **/
#define BSDICIWRDRDBASE BS_CHAR_T *iword;

/**
 * <p>Basic I[2]WORD record for basic methods.</p>
 * @extends BSDICIWRDRDBASE
 **/
typedef struct {
  BSDICIWRDRDBASE
} BsDicIwrdRdBs;

/**
 * <p>Basic I[2]WORDS table type.</p>
 * @param p_iwrdrdtype I[2}WORD record type extends BSDICIWRDRDBASE
 * @extends BSDATASET(p_iwrdrdtype)
 * @member BsDicIdxAb *idx_ab - transient AB to convert i.word into d.word (for working and debugging)
 **/
#define BSDICIWORDSBASE(p_iwrdrdtype) BSDATASET(p_iwrdrdtype) BsDicIdxAb *idx_ab;

/**
 * <p>Basic I[2]WORDS table for basic methods.</p>
 * @extends BSDICIWORDSBASE(BsDicIwrdRdBs)
 **/
typedef struct {
  BSDICIWORDSBASE(BsDicIwrdRdBs)
} BsDicIwrdsBs;

/**
 * <p>Basic method. Find index of the first sorted i.word matched to given one.</p>
 * @param p_iword_- i.word to match
 * @param p_iwrds_sort - sorted i.words array
 * @return index if found, BS_IDX_NULL if not found.
 * @set errno if error.
 **/
BS_IDX_T bsdiciwrdsbs_find_first(BS_CHAR_T *p_iword, BsDicIwrdsBs *p_iwrds);

/**
 * <p>Record in main DIWS table (DWOLT) - i-word (from whole d-word), origin offset and length in chars (from a dictionary).
 * In case when any i2word is not present from start in this table, e.g. small dic has "sense of humor", but
 * it hasn't any word started from "sense", then this record is actually record from i2words table,
 * and length_dword=0 and offset_dword=index in i2words table.</p>
 * @extends BSDICIWRDRDBASE
 * @extends BSDICDWOLTRCDBASE
 * @changes BSDICDWOLTRCDBASE: 
 *    !if length_dword == 0 then it's iword from i2wrdsort table and offset_dword is index in i2wrdsort
 **/
typedef struct {
  BSDICIWRDRDBASE
  BSDICDWOLTRCDBASE
} BsDicIwrdRd;

/**
 * <p>Constructor.</p>
 * @param pIwrdc i.word to  copy
 * @param p_dwofst d.word offset
 * @param p_dwlen d.word length
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIwrdRd *bsdiciwrdrd_new(BS_CHAR_T *pIwrdc, BS_FOFST_T p_dwofst, BS_SMALL_T p_dwlen);

/**
 * <p>Destructor.</p>
 * @param pRcd IWORD record
 * @return always NULL
 **/
BsDicIwrdRd *bsdiciwrdrd_free(BsDicIwrdRd *pRcd);

/**
 * <p>Comparator.</p>
 * @param p_rcd1 IWORD record1
 * @param p_rcd2 IWORD record2
 * @return p_rcd1 -1 less 0 equal 1 greater than p_rcd2
 **/
int bsdiciwrdrd_compare(BsDicIwrdRd *p_rcd1, BsDicIwrdRd *p_rcd2);

/**
 * <p>Comparator match, e.g. sen" equals to "send"!!!
 * But "sen" is greater than "s".</p>
 * @param p_rcd1 IWORD record1
 * @param p_rcd2 IWORD record2
 * @return p_rcd1 -1 less 0 equal 1 greater than p_rcd2
 **/
int bsdiciwrdrd_comparem(BsDicIwrdRd *p_rcd1, BsDicIwrdRd *p_rcd2);

/**
 * <p>Main sorted dic.i-words table. It consists of all ordered i-words from whole d-words without duplicates,
 * their origin offsets and lengths in chars (from a dictionary).
 * It also contains at the end of all i2words from BsDicI2wrdSort that not exists from start of origin phrase.
 * This is actually DWOLT plus i2words marked with length_dword=0.</p>
 * @extends BSDICIWORDSBASE(BsDicIwrdRd)
 * @member BS_IDX_T dwoltSz = size - i2words
 * //transient statistics:
 * @member int max_iword_size - maximum i-word size
 * @member unsigned long all_iwords_size - all i-words size
 **/
typedef struct {
  BSDICIWORDSBASE(BsDicIwrdRd)
  BS_IDX_T dwoltSz;
  int max_iword_size;
  unsigned long all_iwords_size;
} BsDicIwrds;

/**
 * <p>Constructor.</p>
 * @param pBufSz total records
 * @param pIdx_ab alphabet
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIwrds *bsdiciwrds_new(BS_IDX_T pBufSz, BsDicIdxAb *pIdx_ab);

/**
 * <p>Destructor.</p>
 * @param p_iwrd_sort allocated array of records i-words to sort or NULL
 * @return always NULL
 **/
BsDicIwrds *bsdiciwrds_free(BsDicIwrds *p_iwrd_sort);

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
BS_IDX_T bsdiciwrds_add_sort_inc(BsDicIwrds *pSet, BsDicIwrdRd *pObj, BS_IDX_T pInc);

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
BS_IDX_T bsdiciwrds_addi2w_sortm_inc(BsDicIwrds *pSet, BsDicIwrdRd *pObj, BS_IDX_T pInc);

/**
 * <p>Type-safe wrapper. Find index of the first sorted i.word matched to given one.</p>
 * @param p_iword_- i.word to match
 * @param p_iwrds - sorted i.words array
 * @return index if found, BS_IDX_NULL if not found.
 * @set errno if error.
 **/
BS_IDX_T bsdiciwrds_find_first(BS_CHAR_T *p_iword, BsDicIwrds *p_iwrds);

/**
 * <p>Type-safe consumer of just read dic.word. It makes sorted i.words array.</p>
 * @param p_dword - pointer to return next word
 * @param p_iwrds - pointer to make sorted i.words array
 * @return 0 to continue iteration, otherwise stop iteration
 * @set errno if error.
 **/
typedef int BsDicIwrds_Dwrd_Csm(BsDicWord*, BsDicIwrds*);

/**
 * <p>Type-safe consumer of just read dic.word. It makes sorted i.words array.</p>
 * @param p_dword - pointer to return next word
 * @param p_iwrds - pointer to make sorted i.words array
 * @return 0 to continue iteration, otherwise stop iteration
 * @set errno if error.
 **/
int bsdiciwrds_dwrd_csm(BsDicWord *p_dword, BsDicIwrds *p_iwrds);

/**
 * <p>Iterates trough whole dictionary, just read d.word will be consumed
 * by bsdiciwrdsort_dwrd_csm.</p>
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
  BsDicWord_Iter_Tus *p_dw_iter_all, BsDicIwrds_Dwrd_Csm p_consumer, BsDicIwrds **p_iwrds_ret);

//derived from IWORDS I2WORDS:

#define BSDICI2WORDMINLEN 3
/**
 * <p>Record in DI2WS table (I2WPT) - i-2/3/4..word from whole d-word), e.g. "humor" from "sense of humor".
 * Any I2WORD must be at least 3 chars size, i.e. "off" will do, but "of" will not.</p>
 * @extends BSDICIWRDRDBASE
 * @member BS_IDX_T iphrase_idx; //current index of parent whole phrase - BsDicIwrdsRcd
 **/
typedef struct {
  BSDICIWRDRDBASE
  BS_IDX_T iphrase_idx;
} BsDicI2wrdRd;

/**
 * <p>Constructor.</p>
 * @param p_phrase phrase to copy from
 * @param p_i2wstart I2WORD index start in p_phrase
 * @param p_i2wsize I2WORD size
 * @param p_iphrase_idx index of parent phrase in IWORDS
 * @set errno if error.
 **/
BsDicI2wrdRd *bsdici2wrdrd_new(BS_CHAR_T *p_iphrase, int p_i2wstart, int p_i2wsize,
  BS_IDX_T p_iphrase_idx);

/**
 * <p>Destructor.</p>
 * @param pRcd I2WORD record
 * @return always NULL
 **/
BsDicI2wrdRd *bsdici2wrdrd_free(BsDicI2wrdRd *pRcd);

/**
 * <p>Comparator.</p>
 * @param p_rcd1 I2WORD record1
 * @param p_rcd2 I2WORD record2
 * @return p_rcd1 -1 less 0 equal 1 greater than p_rcd2
 **/
int bsdici2wrdrd_compare(BsDicI2wrdRd *p_rcd1, BsDicI2wrdRd *p_rcd2);

/**
 * <p>Second sorted dic.i-2/3/4..words table I2WPT. It's made of main IWORDS table.</p>
 * @extends BSDICIWORDSBASE(BsDicI2wrdRd)
 **/
typedef struct {
  BSDICIWORDSBASE(BsDicI2wrdRd)
} BsDicI2wrds;

/**
 * <p>Constructor and filler.</p>
 * @param pBufSz total records
 * @param p_iwrds - whole phrases
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicI2wrds *bsdici2wrds_new_fill(BS_IDX_T pBufSz, BsDicIwrds *p_iwrds);

/**
 * <p>Destructor.</p>
 * @param p_i2wrds allocated array of records i-words to sort or NULL
 * @return always NULL
 **/
BsDicI2wrds *bsdici2wrds_free(BsDicI2wrds *p_i2wrds);

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
BS_IDX_T bsdici2wrds_add_sort_inc(BsDicI2wrds *pSet, BsDicI2wrdRd *pObj, BS_IDX_T pInc);

/**
 * <p>Type-safe wrapper. Find index of the first sorted i.2word matched to given one.</p>
 * @param p_iword_- i.word to match
 * @param p_i2wrds
 * @return index if found, BS_IDX_NULL if not found.
 * @set errno if error.
 **/
BS_IDX_T bsdici2wrds_find_first(BS_CHAR_T *p_iword, BsDicI2wrds *p_i2wrds);
#endif
