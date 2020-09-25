/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

#include "wchar.h"

#include "BsBase.h"
#include "BsDicFrmt.h"

/**
 * <p>Beigesoft™ dictionary consumable formats shared library implementation.</p>
 * @author Yury Demidenko
 **/

//Methods:
/**
 * <p>
 * Recognize file dictionary format.
 * @param pDicFl dictionary file
 * @return recognized dictionary format.
 * @set errno if error.
 * </p>
 **/
EBsDicFrmts bsdicfrmt_get_format(FILE *pDicFl) {
  BS_WCHAR_T wordb[200];
  int max_strs = 116;
  EBsDicFrmts rez = DFRM_UNKNOWN;
  while (!feof(pDicFl) && !ferror(pDicFl) && max_strs-- > 0) {
    if (fwscanf(pDicFl, L"%199l[^\n]", wordb) == 1) {
      if (wcsstr(wordb, L"[m1]") != NULL || wcsstr(wordb, L"[m2]") != NULL
        || wcsstr(wordb, L"[com]") != NULL || wcsstr(wordb, L"[trn]") != NULL) {
        rez = DFRM_DSL;
        break;
      } else if (wcsstr(wordb, L"<k>") != NULL || wcsstr(wordb, L"<tr>") != NULL) {
        rez = DFRM_STARDICT;
        break;
      }
    }
    fwscanf(pDicFl, L"%*l[^\n]"); //trash after 199
    fwscanf(pDicFl, L"%*l[\n]");
  }
  rewind(pDicFl);
  return rez;
}
