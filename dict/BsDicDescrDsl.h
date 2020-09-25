/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ DSL dictionary word's description reader lib.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICDESCRDSL
#define BS_DEBUGL_DICDESCRDSL 31100

#include "BsDicDescr.h"
#include "BsStrings.h"

//public lib:

/**
 * <p>Read full description with substituted DIC's tags by HTML ones.</p>
 * @param pDicFl - dictionary
 * @param p_wstart offset d.word
 * @return full description as BsHypStrs
 * @set errno if error.
 **/
BsHypStrs *bsdicdescrdsl_read(FILE *pDicFl, BS_FOFST_T p_wstart);

/**
 * <p>Converts string tag into enum. It's a tolerate method.
 * It returns EBSHT_EMPTY if data wrong.</p>
 * @param pStrBuf string tag
 * @return enum tag
 **/
EBsHypTag bsdicdescrdsl_to_tag(BsStrBuf *pStrBuf);
#endif
