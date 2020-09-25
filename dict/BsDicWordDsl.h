/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ DSL dictionary iterator shared library type-safe methods abstraction.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICWORDDSL
#define BS_DEBUGL_DICWORDDSL 20100

#include "BsDicWord.h"

/**
 * <p>Escape back-slash in given word, e.g. "/X\\\(';..;'\)/X\\"->"/X\(';..;')/X\".</p>
 * @param pWrd word to escape \
 **/
void bsdicword_escape_bslash(BS_WCHAR_T *pWrd);

/**
 * <p>
 * Iterates trough whole dictionary, just read dic.word will be consumed
 * by given consumer, if consumer returned non-zero, then iteration will stop.
 * </p>
 * @param pDicFl - opened unwound dictionary file
 * @param p_dic_entry_buffer_size - client expected/predicted this max entry size in dictionary
 * @param p_consume - pointer to dic.word consume delegator
 * @param p_consinst - instrument to consume d.word
 * @return BSR_OK_END or consumer's last return value, e.g. BSR_OK_ENOUGH or errno
  * @set errno if error.
 **/
int bsdicworddsl_iter_tus(FILE *pDicFl, int p_dic_entry_buffer_size, BsDicWord_Consume_Tus *p_consume, void *p_consinst);
#endif
