/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ dictionary public API.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICT
#define BS_DEBUGL_DICT 40000

#include "BsStrings.h"
#include "BsDiIx.h"

/**
 * <p>Show selected string.</p>
 * @param pStr - string not NULL
 * @clears errno if error (only inner-self-handling)
 * @return if exactly word found
 **/
bool bsdict_show (BsString *pStr);

/**
 * <p>Clear cause history cleared.</p>
 **/
void bsdict_on_histclear ();

/**
 * <p>Selected dictionary, so scroll view to its text, if any.</p>
 * @param pDiIx - dic not NULL
 * @clears errno if error (only inner-self-handling)
 **/
void bsdict_dic_sel (BsDiIxBs *pDiIx);

/**
 * <p>Dictionary on/off switched, so hide/show its text from view, if any.</p>
 * @param pDiIx - dic not NULL
 * @clears errno if error (only inner-self-handling)
 **/
void bsdict_dic_switched (BsDiIxBs *pDiIx);
#endif
