/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

#ifndef BS_DEBUGL_DICFRMT
#define BS_DEBUGL_DICFRMT 20500

#include "stdio.h"

/**
 * <p>Beigesoft™ dictionary consumable formats shared library data and method abstractions.</p>
 * @author Yury Demidenko
 **/


//Data types:
typedef enum {
  DFRM_UNKNOWN, DFRM_DSL, DFRM_STARDICT, DFRM_LSA
} EBsDicFrmts;

//Methods:
/**
 * <p>
 * Recognize file dictionary format.
 * @param pDicFl dictionary file
 * @return recognized dictionary format.
 * @set errno if error.
 * </p>
 **/
EBsDicFrmts bsdicfrmt_get_format(FILE *pDicFl);
#endif
