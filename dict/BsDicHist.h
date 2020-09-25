/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ dictionary history dialog.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICHIST
#define BS_DEBUGL_DICHIST 40400

#include "BsStrings.h"

#define BS_DICHIST_FLNM "/bsdict.txt"

/**
 * <p>Lazy get history. This is also for moving inside history.
 * Clears errno if error.</p>
 * @return history, maybe NULL or empty (size == 0)
 * @clears errno if error (only inner-self-handling)
 **/
BsStrings *bsdichist_lget ();

/**
 * <p>Get current IDX. This is for moving inside history.</p>
 * @return current IDX, -1 means NULL
 **/
int bsdichist_get_cuidx ();

/**
 * <p>Try to set current IDX. This is for moving inside history.
 * If new is out of bounds, then just warn logging.</p>
 * @param current IDX
 **/
void bsdichist_tset_cuidx (int pCuIdx);

/**
 * <p>Add string without duplicates. Clears errno if error.</p>
 * @param pStr - string not NULL
 * @return duplicate's index or BS_IDX_NULL if OK
 * @set errno if error
 **/
BS_IDX_T bsdichist_add_rdi (BsString *pStr);

/**
 * <p>Show window.</p>
 * @param pMnWin - main window
 **/
void bsdichist_show (GtkWidget *pMnWin);

/* On exit event handler. */
void bsdichist_on_exit ();
#endif
