/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ text dictionary with index content search type#1 library.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICIDXFILE
#define BS_DEBUGL_DICIDXFILE 30600

#include "stdio.h"
#include "wchar.h"

#include "BsStrings.h"
#include "BsDicFrmt.h"
#include "BsDicIdxIrtRaw.h"
#include "BsDiIx.h"

/**
 * <p>Index file's head of a text dictionary.</p>
 * @extends BSDIIXHEADBS
 * @member BS_IDX_T dwoltSz - total records in DWOLT (words in dictionary)
 * @member BS_IDX_T i2wptSz - total records in I2WPT
 **/
typedef struct {
  BSDIIXHEADBS
  BS_IDX_T dwoltSz;
  BS_IDX_T i2wptSz;
} BsDiIxHeadTx;

/**
 * <p>Dynamic constructor for further loading from IDX.
 * With empty IRT totals.
 * HIRT will be allocated by bsdiixheadtx_add_hirtrd
 * according IRT totals.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHeadTx *bsdiixheadtx_new_tl (void);

/**
 * <p>Dynamic constructor for further filling from IWORDSSORTED and IRTRAW.</p>
 * @param p_edic_frmt dictionary format
 * @param p_iwrdssort - ordered d.i.words array
 * @param pIrtTots IRT totals
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxHeadTx *bsdiixheadtx_new_tf (EBsDicFrmts p_edic_frmt, BsDicIwrds *p_iwrdssort,
                                   BsDicIdxIrtTots *pIrtTots);

/**
 * <p>Dynamic destructor.</p>
 * @param pHead - pointer to IFH
 * @return always NULL
 **/
BsDiIxHeadTx *bsdiixheadtx_free (BsDiIxHeadTx *pHead);

/**
 * <p>Add HIRT record.
 * It will allocate HIRT if it's NULL!</p>
 * @param pHead - pointer to IFH
 * @param pRcd HIRT record
 * @set errno if error.
 **/
void bsdiixheadtx_add_hirtrd (BsDiIxHeadTx *pHead, BsDiIxHirtRd *pRcd);

/**
 * <p>Index file's IRT float (idx_subwrd_size is persistent)
 * or fixed (idx_subwrd_size isn't persistent) size record.</p>
 * @extends BSIRTRCDBASE
 * @member BS_IDX_T i2wpt_start - index (start) in I2WPT, so end = i2wpt_start + i2wpt_quantity - 1
 **/
typedef struct {
  BSIRTRCDBASE
  BS_IDX_T i2wpt_start;
} BsDicIdxIrtRd;

#define BDI_IRTRD_FIXED_SIZE(p_max_irtwrd_size) BS_SMALL_LEN + BS_IDX_LEN*2 + BS_CHAR_LEN*p_max_irtwrd_size

/**
 * <p>Constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDicIdxIrtRd *bsdicidxirtrd_new();

#define BSDICIDXIRTRD_NEW_E_RET(p_als) BsDicIdxIrtRd *p_als=bsdicidxirtrd_new();\
  if (errno != 0) { BSLOG_ERR return; }

/**
 * <p>Destructor.</p>
 * @param p_irt_rcd IRT record.
 * @return always NULL
 **/
BsDicIdxIrtRd *bsdicidxirtrd_free(BsDicIdxIrtRd *p_irt_rcd);

typedef struct {
  BSDICDWOLTRCDBASE
} BsDcIxDwoltRd;

#define BDI_DWOLTRD_SIZE BS_FOFST_LEN + BS_SMALL_LEN

/**
 * <p>Constructor.</p>
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDcIxDwoltRd *bsdiixdwoltrd_new();

#define BSDICIDXDWOLTRD_E_RET(p_als) BsDcIxDwoltRd *p_als=bsdiixdwoltrd_new();\
  if (errno != 0) { BSLOG_ERR return; }

/**
 * <p>Destructor.</p>
 * @param p_dwolt_rcd DWOLT record.
 * @return always NULL
 **/
BsDcIxDwoltRd *bsdiixdwoltrd_free(BsDcIxDwoltRd *p_dwolt_rcd);

/**
 * <p>Base text dictionary with cached IDX head.</p>
 * @extends BSDIIXBST(BsDiIxHeadTx)
 **/
typedef struct {
  BSDIIXBST(BsDiIxHeadTx)
} BsDiIxTxBs;

/**
 * <p>Text dictionary with cached IDX head and IDX file.</p>
 * @extends BSDIIXBST(BsDiIxHeadTx)
 * @member FILE *idxFl - opened IDX
 * @member BsString nme - name
 * @member BS_FOFST_T irtOfst - offset IRT
 * @member BS_FOFST_T i2wptOfst - offset I2WPT
 * @member BS_FOFST_T dwoltOfst - offset DWOLT
 **/
typedef struct {
  BSDIIXBST(BsDiIxHeadTx)
  FILE *idxFl;
  BS_FOFST_T irtOfst;
  BS_FOFST_T i2wptOfst;
  BS_FOFST_T dwoltOfst;
} BsDiIxTx;

/**
 * <p>Constructor of IDX BASE fillits head from IDX file.</p>
 * @param pDicFl - dictionary
 * @param pIdx_file - dictionary's IDX
 * @param pHead - idx-head with totals and not-yet filled HIRT
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxTx *bsdiixtx_new (FILE *pDicFl, FILE *pIdx_file, BsDiIxHeadTx *pHead);

/**
 * <p>Destructor include foreign head and closing file.</p>
 * @param pDiIx IDX with head
 * @return always NULL
 **/
BsDiIxTx *bsdiixtx_destroy (BsDiIxTx *pDiIx);

/**
 * <p>Full dictionary with full index in memory data.</p>
 * @extends BSDIIXBST(BsDiIxHeadTx)
 * data evaluated during opening/creating IDX:
 * @member BsDicIdxIrtRd **irt
 * @member BS_IDX_T *i2wpt
 * @member BsDcIxDwoltRd **dwolt
 **/
typedef struct {
  BSDIIXBST(BsDiIxHeadTx)
  BsDicIdxIrtRd **irt;
  BS_IDX_T *i2wpt;
  BsDcIxDwoltRd **dwolt;
} BsDiIxTxRm;

#define BDI_I2WPTRD_SIZE BS_IDX_LEN

/**
 * <p>Constructor of IDX RAM (in memory) to fill from IDX file or IRTRAW and IWORDSSORT.</p>
 * @param pDicFl - dictionary
 * @param pHead - idx-head with totals and not-yet filled HIRT
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDiIxTxRm *bsdiixtxrm_new (FILE *pDicFl, BsDiIxHeadTx *pHead);

/**
 * <p>Destructor of IDX RAM (in memory) and closing file.
 * It frees foreign object head.</p>
 * @param pDiIxRm IDX RAM
 * @return always NULL
 **/
BsDiIxTxRm *bsdiixtxrm_destroy (BsDiIxTxRm *pDiIxRm);

/**
 * <p>Fills IDX RAM (in memory) with data from IRTRAW and DIWORDSSORTED.</p>
 * @param pDiIxRm IDX RAM.
 * @param p_iwrds - ordered d.i.words array
 * @param p_irtraw - IRT raw in memory
 * @set errno if error.
 **/
void bsdiixtxrm_fill(BsDiIxTxRm *pDiIxRm, BsDicIwrds *p_iwrds,
  BsDicIdxIrtRaw *p_irtraw);

/**
 * <p>Validate IDX RAM (in memory).</p>
 * @param pDiIxRm IDX RAM.
 * @set errno if error.
 **/
void bsdiixtxrm_validate(BsDiIxTxRm *pDiIxRm);

/**
 * <p>Save (write/overwrite) IDX RAM (in memory) into file.</p>
 * @param pDiIxRm IDX RAM.
 * @param pPth - dictionary path.
 * @set errno if error.
 **/
void bsdiixtxrm_save(BsDiIxTxRm *pDiIxRm, char *pPth);

/**
 * <p>Load IDX RAM (in memory) from IDX file.</p>
 * @param pPth - dictionary path.
 * @return object or NULL if error or if IDX file not found
 * @set errno if error. "IDX file not found" is not error!
 **/
BsDiIxTxRm* bsdiixtxrm_load (char *pPth);

#define BSDICIDXRAM_OPEN_E_RET(p_als, p_dicpth) BsDiIxTxRm* p_als=bsdiixtxrm_load(p_dicpth);\
  if (errno != 0) { BSLOG_ERR return; }

/**
 * <p>Load IDX Base (head) from IDX file.</p>
 * @param pPth - dictionary path.
 * @return object or NULL if error or if IDX file not found
 * @set errno if error. "IDX file not found" is not error!
 **/
BsDiIxTx* bsdiixtx_load (char *pPth);

#define BSDICIDXBASE_OPEN_E_RET(p_als, p_dicpth) BsDiIxTx* p_als=bsdiixtx_load(p_dicpth);\
  if (errno != 0) { BSLOG_ERR return; }

/**
 * <p>Load IDX Base (head) from IDX file.</p>
 * @param pPth - dictionary path.
 * @param pHeadRt - pointer to return filled head
 * @param pDicFlRt - pointer to return opened dictionary
 * @param pIdx_file_ret - pointer to return opened IDX file
 * @set errno if error. "IDX file not found" is not error!
 **/
void bsdiixheadtx_load(char *pPth, BsDiIxHeadTx** pHeadRt,
  FILE ** pDicFlRt, FILE **pIdx_file_ret);

/**
 * <p>Create IDX RAM (in memory).</p>
 * @param pPth - dictionary path.
 * @param pOpSt - opening state data shared with client
 * @return object or NULL if stopped error
 * @set errno if error.
 **/
BsDiIxTxRm* bsdiixtxrm_create (char *pPth, BsDiIxOst* pOpSt);

/**
 * <p>Open DIC IDX, i.e. load or create then load.</p>
 * @param pPth - dictionary path.
 * @param pOpSt - opening state data shared with client
 * @param pIsIxRm - client prefers IRT (index records table) in memory (RAM) than in file
 * @return object or NULL if error
 * @set errno if error.
 **/
BsDiIxTxBs* bsdiixtx_open (char *pPth, BsDiIxOst* pOpSt, bool pIsIxRm);

#endif
