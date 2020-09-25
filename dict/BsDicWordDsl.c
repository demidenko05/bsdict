/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ DSL dictionary iterator shared library implementation, data type 2.</p>
 * @author Yury Demidenko
 **/

#include "stdio.h"
#include "stdlib.h"
#include "wchar.h"
#include "wctype.h"
#include "string.h"

#include "BsLog.h"
#include "BsDicWordDsl.h"

/**
 * <p>Find back-slash in given word.</p>
 * @param pWrd word to find \
 * @return pointer to start back-slash string or NULL
 **/
BS_WCHAR_T *bsdicword_find_bslash(BS_WCHAR_T *pWrd) {
  int idx = -1;
  for (int i = 0; ; i++) {
    if (pWrd[i] == 0) {
      break;
    }
    if (pWrd[i] == L'\\') {
      idx = i;
      break;
    }
  }
  if (idx >=0) {
    return pWrd + idx;
  }
  return NULL;
}

/**
 * <p>Replace tab to space.</p>
 * @param pWrd word to fix
 **/
void bsdicword_replace_tabs(BS_WCHAR_T *pWrd) {
  for (int i = 0; ; i++) {
    if (pWrd[i] == 0) {
      break;
    }
    if (pWrd[i] == L'\t') {
      pWrd[i] = L' ';
    }
  }
}

/**
 * <p>Escape leading and trailing spaces and tabs.</p>
 * @param pWrd word to escape leading and trailing spaces and tabs
 **/
void bsdicword_escape_bounds_spaces(BS_WCHAR_T *pWrd) {
  //1.leading
  int i;
  while (pWrd[0] == ' ' || pWrd[0] == L'\t' || pWrd[0] == L'\r') {
    for (i = 0; ; i++) {
      pWrd[i] = pWrd[i + 1];
      if (pWrd[i] == 0) {
        break;
      }
    }
  }
  //1.trailing
  int idxend = wcslen(pWrd) - 1;
  while (pWrd[idxend] == L' ' || pWrd[idxend] == L'\t' || pWrd[idxend] == L'\r') {
    pWrd[idxend] = 0;
    idxend--;
  }
}

/**
 * <p>Escape back-slash in given word, e.g. "/X\\\(';..;'\)/X\\"->"/X\(';..;')/X\".</p>
 * @param pWrd word to escape \
 **/
void bsdicword_escape_bslash(BS_WCHAR_T *pWrd) {
  BS_WCHAR_T *escp = bsdicword_find_bslash(pWrd);//wcschr(pWrd, L'\\');
  while (escp != NULL) {
    for (int i = 0; ; i++) {
      escp[i] = escp[i + 1];
      if (escp[i] == 0) {
        break;
      }
    }
    if (escp[0] == L'\\') { //keep escaped BSL:
      if (escp[1] == 0) { //the end:
        break;
      } else {
        escp = bsdicword_find_bslash(escp + 1);//wcschr(escp + 1, L'\\');
      }
    } else {
      escp = bsdicword_find_bslash(escp);//wcschr(escp, L'\\');
    }
  }
}

/**
 * <p>
 * Type-unsafe iterator trough whole dictionary, just read dic.word will be consumed
 * by given consumer, if consumer returned non-zero, then iteration will stop.
 * Client must invoke another type-safe wrapper of this iterator.
 * </p>
 * @param pDicFl - opened unwound dictionary file
 * @param p_dic_entry_buffer_size - client expected/predicted this max entry size in dictionary
 * @param p_consume - pointer to dic.word consumer
 * @param p_consinst - instrument to consume d.word
 * @return BSR_OK_END or consumer's last return value, e.g. BSR_OK_ENOUGH or errno
 * @set errno if error.
 **/
int bsdicworddsl_iter_tus(FILE *pDicFl, int p_dic_entry_buffer_size, BsDicWord_Consume_Tus *p_consume, void *p_consinst) {
  BS_WCHAR_T wordb[p_dic_entry_buffer_size];
  char wordc[p_dic_entry_buffer_size];
  //EF BB BF UTF8 start but FEFF actually read
  //FF FE UTF16 start
  BsDicWord bsdword;
  BS_WCHAR_T frmt_word[11], frmt_cont[25];
  int i = 0;
  frmt_cont[i++] = L'%';
  frmt_cont[i++] = L'*';
  frmt_cont[i++] = L'[';
  frmt_cont[i++] = 0xFEFF; //actually read
  frmt_cont[i++] = L'#';
  frmt_cont[i++] = L']';
  frmt_cont[i++] = L'%';
  swprintf(frmt_cont + i, 5, L"%d", p_dic_entry_buffer_size - 1);
  wcscat(frmt_cont, L"l[^\n]");
  //skip comments:
  while (!feof(pDicFl) && !ferror(pDicFl)) {
    int crdd = fwscanf(pDicFl, frmt_cont, wordb);
    if (crdd != 1) {
      break;
    }
    fwscanf(pDicFl, L"%*l[\n]");
  }
  frmt_word[0] = '%';
  swprintf(frmt_word + 1, 5, L"%d", p_dic_entry_buffer_size - 1);
  wcscat(frmt_word, L"l[^\n]");
  frmt_cont[0] = '\0';
  wcscat(frmt_cont, L"%*l[\t ]%");
  swprintf(frmt_cont + 8, 5, L"%d", p_dic_entry_buffer_size - 1);
  wcscat(frmt_cont, L"l[^\n]");
  bool is_debug = bslog_is_debug(BS_DEBUGL_DICWORDDSL);
  if (is_debug) {
    BSLOG_LOG(BSLDEBUG, "frmt_cont=%ls\n", frmt_cont)
    BSLOG_LOG(BSLDEBUG, "frmt_word=%ls\n", frmt_word)
  }
  while (!feof(pDicFl) && !ferror(pDicFl)) {
    int crdd = fwscanf(pDicFl, frmt_cont, wordb); //word's content
    if (crdd != 1) {
      long int poz = ftell(pDicFl);
      crdd = fwscanf(pDicFl, frmt_word, wordb); //word
      if (crdd == 1) {
        if (is_debug) { BSLOG_LOG(BSLDEBUG, "WORD %ls\n", wordb) }
        int strwchl = wcslen(wordb);
        wcstombs(wordc, wordb, strwchl*4);
        bsdword.lenChrs = strlen(wordc);
        bsdicword_escape_bslash(wordb);
        bsdicword_escape_bounds_spaces(wordb);
        bsdicword_replace_tabs(wordb);
        if (wordb[0] != 0) {
          strwchl = wcslen(wordb);
          for (int i = 0; i < strwchl; i++) {
            if (iswalpha(wordb[i])) {
              bsdword.word = wordb;
              bsdword.offset = poz;
              int csmret = 0;
              BS_DO_E_RETE(csmret = p_consume(&bsdword, p_consinst))
              if (csmret == 0) {
                break;
              } else {
                return csmret;
              }
            }
          }
        }
      }
    } else {
      if (is_debug) { BSLOG_LOG(BSLDEBUG, "CONT %ls\n", wordb) }
      while (crdd == 1) {
        crdd = fwscanf(pDicFl, frmt_word, wordb); //rest of word's content
        if (is_debug && crdd == 1) { BSLOG_LOG(BSLDEBUG, "CONT_REST %ls\n", wordb) }
      }
    }
    fwscanf(pDicFl, L"%*l[\n]");
  }
  return BSR_OK_END;
}
