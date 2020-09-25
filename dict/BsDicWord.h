/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ text dictionary word iterator shared library type-safe data and method abstractions.
 * It handles format-dependent reading words/phrases from a text dictionary.
 * </p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICWORD
#define BS_DEBUGL_DICWORD 20000

#include "stdio.h"

#include "BsBase.h"

#define BS_DIC_ENTRY_BUFFER_SIZE 1000 //TODO 0

//Data types:
typedef struct {
  BS_WCHAR_T *word; //or phrase
  BS_FOFST_T offset;
  BS_SMALL_T lenChrs; //in bytes (chars) without terminator 32767 max
} BsDicWord;

//Methods types (denoted with '_'):
/**
 * <p>Type-unsafe consumer of just read dic.word.</p>
 * @param pRdWrd - just read d.word to consume
 * @param pInst - instrument to consume d.word
 * @return 0 to continue iteration, otherwise stop iteration
 * @set errno if error.
 **/
typedef int BsDicWord_Consume_Tus(BsDicWord *pRdWrd, void *pInst);

/**
 * <p>
 * Type-unsafe iterator trough whole dictionary, just read dic.word will be consumed
 * by given consumer, if consumer returned non-zero, then iteration will stop.
 * Client must invoke another type-safe wrapper of this iterator.
 * </p>
 * @param pDicFl - opened unwound dictionary file
 * @param pDeBufSz - client expected/predicted this max entry size in dictionary
 * @param pWd_Csm - dic.word consumer
 * @param pInst - instrument to consume d.word
 * @return BSR_OK_END or consumer's last return value, e.g. BSR_OK_ENOUGH or errno
 * @set errno if error.
 **/
typedef int BsDicWord_Iter_Tus(FILE *pDicFl, int pDeBufSz, BsDicWord_Consume_Tus *pWd_Csm, void *pInst);
#endif
