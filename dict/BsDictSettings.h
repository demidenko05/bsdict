/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ dictionary settings dialog.</p>
 * @author Yury Demidenko
 **/

#ifndef BS_DEBUGL_DICTSETTINGS
#define BS_DEBUGL_DICTSETTINGS 40300

#include "BsDicObj.h"

#define BSDICSET_PATH "/.bsdict.conf"

#define BSDS_DISABLED "#Disabled="

#define BSDS_ISIXRM "#IsIxRm="

#define BSDS_NAME "#Name="

#define BSDS_PATH "#Path="

/**
 * <p>Lazy get w.dictionaries. It's always invoked by BSDICT first.</p>
 * @return w.dictionaries, maybe NULL
 * @set errno if error.
 **/
BsDicObjs *bsdicsettings_lget_dics (); //TODO 0 it's init plus MT safe iterator plus dic's consumer

/**
 * <p>Settings window.</p>
 * @param pMnWin - main window
 **/
void bsdicsettings_show (GtkWidget *pMnWin);

/**
 * <p>If settings busy, i.e. indexing a dic.</p>
 * @return if settings busy
 **/
bool bsdicsettings_is_busy ();

/* On exit event handler. */
void bsdicsettings_on_exit ();
#endif
