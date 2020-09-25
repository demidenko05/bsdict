/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ dictionary settings dialog.</p>
 * @author Yury Demidenko
 **/

#include "string.h"
#include "stdlib.h"

#include "gtk/gtk.h"

#include "BsI18N.h"
#include "BsError.h"
#include "BsDictSettings.h"

//Data:
static BsDicObjs *sDics = NULL;

//Multi-purposes buffer:
static BsStrBuf *sStrBuf = NULL;

//Widgets:
static GtkWidget *sSetnWin = NULL, *sGtkList = NULL;

//Menu:
enum EMnVlsDiSt { EADD, ESAVE, EDELETE, EONOFF };

static int sMenuVals[] = { EADD, ESAVE, EDELETE, EONOFF };

//state:

static bool sIsIxRm = false;

  //ID for progress thread:
static int sTimer = -1;

  // 0.5 sec:
#define REFR_TIME 500

  //-1 means not selected row:
static int sSelRow = -1;

  //progress's sign switcher:
static int sPrgCnt = 0;

//Thread data locker:
static GMutex sMutex;

#define BS_THREAD_LOCK g_mutex_lock (&sMutex);

#define BS_THREAD_UNLOCK g_mutex_unlock (&sMutex);

//Dic opener args:
typedef struct {
  BS_IDX_T ixSt;
  BS_IDX_T ixEn;
  GThread *thrd;
} OpenArg;

OpenArg sOargAdd = { .ixSt = BS_IDX_NULL, .ixEn = BS_IDX_NULL, .thrd = NULL };

OpenArg sOargs[2] = { { .ixSt = BS_IDX_NULL, .ixEn = BS_IDX_NULL, .thrd = NULL },
                      { .ixSt = BS_IDX_NULL, .ixEn = BS_IDX_NULL, .thrd = NULL } };

/* Generic info dialog */ //TODO 1 into lib
static void
  s_dialog_info (char *pMsg)
{
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new (GTK_WINDOW (sSetnWin),
				   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				   GTK_MESSAGE_INFO, GTK_BUTTONS_OK, pMsg);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

/* Confirmation dialog, returns 1 (TRUE) if confirmed */
static gboolean
  s_dialog_confirm (char *pQues)
{
  GtkWidget *dialog;
  gint response;
  dialog = gtk_message_dialog_new (GTK_WINDOW (sSetnWin),
    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, pQues);
  response = gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  if ( response == GTK_RESPONSE_OK )
                { return TRUE; }
  return FALSE;
}

/**
 * <p>Open dic in dics.</p>
 * @param pArg index start-end
 * @set errno if error.
 **/
static void
  s_open_dic (OpenArg *pArg)
{

  BsDicObj *wdici; //cached dic for unlocked opening

  bool isDbg = bslog_is_debug (BS_DEBUGL_DICTSETTINGS + 70);
  
  if ( isDbg )
      { BSLOG_LOG (BSLDEBUG, "Try to open dics OARG#%p from "BS_IDX_FMT" to "BS_IDX_FMT"\n", pArg, pArg->ixSt, pArg->ixEn) }

  for ( BS_IDX_T l = pArg->ixSt; l <= pArg->ixEn; l++ )
  {
    wdici = NULL;
    BS_THREAD_LOCK
                // return if no dic:
      if ( sDics == NULL || !( l >= BS_IDX_0 && l < sDics->size ) )
                              { return; }

      if ( isDbg )
          { BSLOG_LOG (BSLDEBUG, "Dic %s, state init =%d\n", sDics->vals[l]->nme->val, sDics->vals[l]->opSt->stt) }

      if ( sDics->vals[l]->opSt->stt != EBSDS_DISABLED
                      && sDics->vals[l]->diIx == NULL )
                                { wdici = sDics->vals[l]; }
    BS_THREAD_UNLOCK
    
    if ( wdici != NULL )
    {
      // try to open dic with index:
      bsdicobj_open (wdici);
      
      if ( wdici->diIx != NULL )
      {
        BS_THREAD_LOCK  //try to set new indexed diIx:

          if ( bsdicobjs_find_ref (sDics, wdici) == BS_IDX_NULL )
          {
            wdici->diIx = wdici->diix_destroy (wdici->diIx);
          }

        BS_THREAD_UNLOCK
      } //else: e.g. it can be canceled
    }
  }
} 

/**
 * <p>Open dic in dics with thread.</p>
 * @param pArg index start-end
 * @return always 0
 **/
static gpointer
  s_open_dic_thrd (gpointer pArg)
{
  OpenArg *arg = (OpenArg*) pArg;
  BS_DO_CEERR (s_open_dic (arg))
  g_thread_unref (arg->thrd);
  arg->thrd = NULL;
  if ( bslog_is_debug (BS_DEBUGL_DICTSETTINGS) )
      { BSLOG_LOG (BSLINFO, "Thread exiting...\n") }
  return NULL;
}

/* Free allocated here data */
static void
  s_free_here ()
{
  sDics = bsdicobjs_free (sDics);
  sStrBuf = bsstrbuf_free (sStrBuf);
  g_mutex_clear (&sMutex);
}

/* Save settings. */
static void
  s_save ()
{
  const char *homed = g_get_home_dir();
  char pth[strlen(homed) + 15];
  strcpy (pth, homed);
  strcat (pth, BSDICSET_PATH);
  FILE *flConf = fopen (pth, "w");
  BS_IF_EN_RET (flConf == NULL, BSE_OPEN_FILE)

  BS_THREAD_LOCK
    if ( sDics != NULL )
    {
      int disbl;
      fprintf (flConf, BSDS_ISIXRM"%d\n", sIsIxRm);
      for ( int i = 0; i < sDics->size; i++ )
      {
        fprintf (flConf, BSDS_PATH"%s\n", sDics->vals[i]->pth->val);
        fprintf (flConf, BSDS_NAME"%s\n", sDics->vals[i]->nme->val);
        fprintf (flConf, BSDS_ISIXRM"%d\n", sDics->vals[i]->pref->isIxRm);
        if ( sDics->vals[i]->opSt->stt == EBSDS_DISABLED )
        {
          disbl = 1;
        } else {
          disbl = 0;
        }
        fprintf (flConf, BSDS_DISABLED"%d\n", disbl);
      }
    }
  BS_THREAD_UNLOCK

  s_dialog_info (bsi18n_msg ("Settings have been saved!"));
  fclose (flConf);
}

/* Choose file dialog. */
static void
  s_dialog_choose ()
{
  if ( sOargAdd.thrd != NULL )
      { return; }

  GtkWidget *flChsr;
  flChsr = gtk_file_chooser_dialog_new (bsi18n_msg ("Select dictionary"),
                (GtkWindow*) sSetnWin, GTK_FILE_CHOOSER_ACTION_OPEN,
                GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN,
                GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(flChsr), FALSE);

  GtkFileFilter *flt = gtk_file_filter_new();

  gtk_file_filter_add_pattern(flt, "*.dsl");
  gtk_file_filter_add_pattern(flt, "*.dict");
  gtk_file_filter_add_pattern(flt, "*.lsa");

  gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (flChsr), flt);
  gtk_dialog_run (GTK_DIALOG (flChsr));

  GFile *file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (flChsr));
  BsDicObj *dic = NULL;
  if ( file != NULL )
  {
    //it maybe folder on close, also filter wrong!
    gchar *bnm = g_file_get_basename (file);
    char *ext = strrchr(bnm, '.');
    if ( ext != NULL )
    {
      BS_DO_CEE_OUT (dic = bsdicobj_new (g_file_get_path (file), sIsIxRm))
      
      BS_THREAD_LOCK
        if ( sDics == NULL )
        {
          BS_DO_CEERR ( sDics = bsdicobjs_new (BS_IDX_10) )
          errno = 0;
        }
        if ( sDics != NULL )
        {
          if ( bsdicobjs_find (sDics, dic) == BS_IDX_NULL && errno == 0 )
          {
            BS_IDX_T idx = bsdicobjs_add_inc (sDics, dic, BS_IDX_10);
            if ( idx != BS_IDX_NULL )
            {
              dic = NULL;
              sOargAdd.ixSt = idx;
              sOargAdd.ixEn = idx;
              sOargAdd.thrd = NULL;
              sOargAdd.thrd = g_thread_new (NULL, s_open_dic_thrd, &sOargAdd);
            }
          }
        }
      BS_THREAD_UNLOCK
    }
  }
out:
  bsdicobj_free (dic);
  gtk_widget_destroy (flChsr);
}

/* Refresh GUI list. Invoked only by refresher! */
static void
  s_refresh_list ()
{
  gtk_list_clear_items (((GtkList*) sGtkList), 0, -1);

  if ( sPrgCnt < 2 ) {
    sPrgCnt++;
  } else {
    sPrgCnt = 0;
  }
  for ( int i = 0; i < sDics->size; i++ )
  {
    bsstrbuf_clear (sStrBuf);
    if ( sDics->vals[i]->opSt->stt == EBSDS_DISABLED )
    {
      BS_DO_CEE_RET (bsstrbuf_add_str_inc (sStrBuf,
                          bsi18n_msg ("(disabled) "), BS_IDX_100))
      BS_DO_CEE_RET (bsstrbuf_add_str_inc (sStrBuf,
                           sDics->vals[i]->nme->val, BS_IDX_100))
    } else {
      if ( sDics->vals[i]->opSt->stt == EBSDS_OPENED )
      {
        BS_DO_CEE_RET (bsstrbuf_add_str_inc (sStrBuf,
                                               "<b>", BS_IDX_100))
        BS_DO_CEE_RET (bsstrbuf_add_str_inc (sStrBuf,
                           sDics->vals[i]->diIx->head->nme->val, BS_IDX_100))
        BS_DO_CEE_RET (bsstrbuf_add_str_inc (sStrBuf,
                                              "</b>", BS_IDX_100))
      } else if ( sDics->vals[i]->opSt->stt == EBSDS_OPENING
                  || sDics->vals[i]->opSt->stt == EBSDS_INDEXING ) {
        char cprogr;
        if ( sPrgCnt == 0 ) {
          cprogr = '/';
        } else if ( sPrgCnt == 1 ) {
          cprogr = '-';
        } else {
          cprogr = '\\';
        }
        char *stst;
        if ( sDics->vals[i]->opSt->stt == EBSDS_OPENING )
        {
          stst = bsi18n_msg ("opening ");
        } else {
          stst = bsi18n_msg ("indexing ");
        }
        BS_DO_CEE_RET (bsstrbuf_add_str_inc (sStrBuf,
                                               "<b>", BS_IDX_100))
        BS_DO_CEE_RET (bsstrbuf_add_str_inc (sStrBuf,
                                                stst, BS_IDX_100))
        BS_DO_CEE_RET (bsstrbuf_add_inc (sStrBuf,
                                              cprogr, BS_IDX_100))
        BS_DO_CEE_RET (bsstrbuf_add_str_inc (sStrBuf,
                                            "</b>\t", BS_IDX_100))
        BS_DO_CEE_RET (bsstrbuf_add_str_inc (sStrBuf,
                           sDics->vals[i]->nme->val, BS_IDX_100))
      } else {
        BS_DO_CEE_RET (bsstrbuf_add_str_inc (sStrBuf,
                                             "<span background=\"red\">", BS_IDX_100))
        BS_DO_CEE_RET (bsstrbuf_add_str_inc (sStrBuf,
                           sDics->vals[i]->nme->val, BS_IDX_100))
        BS_DO_CEE_RET (bsstrbuf_add_str_inc (sStrBuf,
                                            "</span>", BS_IDX_100))
      }
    }

    BS_DO_CEE_RET (char *str = bsstrbuf_tochars (sStrBuf))

    GtkWidget *lstItm = gtk_list_item_new_with_label (str);

    gtk_container_add (GTK_CONTAINER (sGtkList), lstItm);

    if ( sDics->vals[i]->opSt->stt != EBSDS_DISABLED )
    {
      GtkItem gi = GTK_LIST_ITEM (lstItm)->item;

      GList *glst = gtk_container_get_children (GTK_CONTAINER (&gi));

      gtk_label_set_text ((GtkLabel*) glst->data, str);

      gtk_label_set_use_markup ((GtkLabel*) glst->data, TRUE);
    }
    gtk_widget_show(lstItm);
  }
  if ( sSelRow != -1 )
     { gtk_list_select_item (((GtkList*) sGtkList), sSelRow); }
}

/* Refreshing thread */
static gboolean
  s_refresh (gpointer pData)
{
  BS_THREAD_LOCK

    if ( sDics != NULL )
        { s_refresh_list(); }

  BS_THREAD_UNLOCK 
  return TRUE;
}

//User's actions:

/* On menu item select event handler. */
static void
  s_on_menu_item_click (int *pMenuVal)
{
  if ( *pMenuVal == EADD )
  {
    s_dialog_choose ();
  }
  else if ( *pMenuVal == ESAVE ) {
    s_save ();
  }
  else if ( *pMenuVal == EONOFF ) {
    if ( sSelRow == -1 )
                { return; }
    BS_THREAD_LOCK
      if ( sDics->vals[sSelRow]->opSt->stt == EBSDS_OPENED )
      {
        sDics->vals[sSelRow]->opSt->stt = EBSDS_DISABLED;
      }
      else if ( sDics->vals[sSelRow]->opSt->stt == EBSDS_DISABLED )
      {
        if ( sDics->vals[sSelRow]->diIx != NULL )
        {
          sDics->vals[sSelRow]->opSt->stt = EBSDS_OPENED;
        } else {
          sDics->vals[sSelRow]->opSt->stt = EBSDS_OPENING;
          sOargAdd.ixSt = sSelRow;
          sOargAdd.ixEn = sSelRow;
          sOargAdd.thrd = NULL;
          sOargAdd.thrd = g_thread_new (NULL, s_open_dic_thrd, &sOargAdd);
        }
      }
    BS_THREAD_UNLOCK
  }
  else if ( *pMenuVal == EDELETE ) {
    if ( sSelRow == -1
      || !s_dialog_confirm (bsi18n_msg ("Delete selected?")) )
                { return; }
    BS_THREAD_LOCK
      BsDicObj *diObj = sDics->vals[sSelRow];
      bsdicobjs_remove_shrink (sDics, sSelRow);
      bsdicobj_free (diObj);
    BS_THREAD_UNLOCK
  }
}

/* move item up */
static void
  s_item_up (gpointer pData)
{
  BS_THREAD_LOCK
    if ( sSelRow > 0 && bsdicobjs_move_down(sDics, sSelRow) )
    {
      sSelRow--;
    }
  BS_THREAD_UNLOCK
}

/* move item up */
static void
  s_item_down (gpointer pData)
{
  BS_THREAD_LOCK

    if ( sSelRow >= 0 && bsdicobjs_move_up (sDics, sSelRow) )
                { sSelRow++; }

  BS_THREAD_UNLOCK
}

/* Menu opener */
static void
  s_menu_open (GtkWidget *pMnu)
{
  gtk_menu_popup (GTK_MENU (pMnu), NULL, NULL,
                  NULL, NULL, 0, gtk_get_current_event_time ());
}

/* On select row (double click) event handler. */
static void
  s_on_row_select (GtkWidget *pData1, gpointer pData2)
{
  if ( ((GtkList*) sGtkList)->selection )
  {
    sSelRow = g_list_index (((GtkList*) sGtkList)->children,
                            ((GtkList*) sGtkList)->selection->data);
  }
}

/* On close event handler. */
static gboolean
  s_on_close (GtkWidget *pWidg, GdkEvent *pEvent, gpointer pDt)
{
  if (sTimer != -1) {
    g_source_remove (sTimer);
    sTimer = -1;
  }
  return FALSE;
}

//Public lib:

/**
 * <p>Lazy get w.dictionaries.</p>
 * @set errno if error.
 **/
void
  bsdicsettingstst_load_dics()
{
  const char *homed = g_get_home_dir();
  char fpth[strlen(homed) + 15];

  strcpy(fpth, homed);
  strcat(fpth, BSDICSET_PATH);
  FILE *flConf = fopen(fpth, "r");

  if ( flConf == NULL )
                { return; }

  int dsbl, isIxRm, crdd;

  bsstrbuf_clear (sStrBuf);
  crdd = fscanf (flConf, BSDS_ISIXRM"%d", &isIxRm);
  if ( crdd != 1 )
  {
    errno = BSE_READ_FILE;
    BSLOG_ERR
    goto out;
  }
  
  sIsIxRm = isIxRm;
  
  fscanf (flConf, "%*[\n]");

  while ( !feof (flConf) && !ferror (flConf) )
  {
    bsstrbuf_clear (sStrBuf);
    crdd = fscanf (flConf, BSDS_PATH"%[^\n]", sStrBuf->vals);

    if ( crdd != 1 )
    {
      if ( sStrBuf->bsize == BS_IDX_0
            || sStrBuf->bsize > BS_IDX_10000 )
      {
        errno = BSE_READ_FILE;
        BSLOG_ERR
        goto out;
      }
      BS_DO_CEE_OUT (bsstrbuf_inc (sStrBuf, BS_IDX_100))
      continue;
    }

    fscanf (flConf, "%*[\n]");
    fscanf (flConf, BSDS_NAME"%*[^\n]");
    fscanf (flConf, "%*[\n]");

    crdd = fscanf (flConf, BSDS_ISIXRM"%d", &isIxRm);
    if ( crdd != 1 )
    {
      errno = BSE_READ_FILE;
      BSLOG_ERR
      goto out;
    }
    fscanf (flConf, "%*[\n]");

    crdd = fscanf (flConf, BSDS_DISABLED"%d", &dsbl);
    if ( crdd != 1 )
    {
      errno = BSE_READ_FILE;
      BSLOG_ERR
      goto out;
    }

    BS_DO_CEE_OUT (BsDicObj *dic = bsdicobj_new (sStrBuf->vals, isIxRm))
    if ( dsbl )
            { dic->opSt->stt = EBSDS_DISABLED; }

    if ( sDics == NULL )
        { BS_DO_CEE_OUT (sDics = bsdicobjs_new (BS_IDX_10)) }

    BS_DO_CEE_OUT (bsdicobjs_add_inc (sDics, dic, BS_IDX_2))

    fscanf (flConf, "%*[^\n]");
    fscanf (flConf, "%*[\n]");
  }
out:
  fclose (flConf);
}

/**
 * <p>Lazy get w.dictionaries. It's always invoked by BSDICT first.</p>
 * @return w.dictionaries, maybe NULL
 * @set errno if error.
 **/
BsDicObjs*
  bsdicsettings_lget_dics()
{    
  if ( sDics != NULL )
            { return sDics; }

  if ( sStrBuf == NULL )
            { BS_DO_CEE_RETN (sStrBuf = bsstrbuf_new (BS_IDX_300)) }

  bool isDbg = bslog_is_debug (BS_DEBUGL_DICTSETTINGS);
#ifdef BS_DEBUG_BASIC_ENABLED
  if ( !isDbg )
  {
    if (bslog_get_debug_ceiling () == 0 )
    {
      bslog_set_debug_floor (BS_DEBUGL_DICTSETTINGS);
      bslog_set_debug_ceiling (BS_DEBUGL_DICTSETTINGS);
    } else {
      bslog_add_dbgrange (BS_DEBUGL_DICTSETTINGS, BS_DEBUGL_DICTSETTINGS);
    }
    isDbg = bslog_is_debug (BS_DEBUGL_DICTSETTINGS);
  }
#endif
  g_mutex_init (&sMutex);

  BS_THREAD_LOCK
        //1. try to load dics's initial attributes from settings:
    if ( sDics == NULL )
              {  bsdicsettingstst_load_dics (); }

    if ( isDbg && sDics == NULL )
    {
      BSLOG_LOG (BSLINFO, "No saved settings\n");
    } else if ( isDbg && sDics != NULL ) {
      BSLOG_LOG (BSLINFO, "Saved settings size="BS_IDX_FMT"\n", sDics->size);
    }
  BS_THREAD_UNLOCK

  if ( sDics != NULL && sDics->size > 0 )
  {
            //2 usual new thread[s] loading:
    int thrdCnt;

    if ( sDics->size > BS_IDX_1 )
    {
      thrdCnt = 2;
      BS_IDX_T idxMiddle = sDics->size / BS_IDX_2;
      sOargs[0].ixSt = BS_IDX_0;
      sOargs[0].ixEn = idxMiddle - BS_IDX_1;
      sOargs[0].thrd = NULL;
      sOargs[1].thrd = NULL;
      sOargs[1].ixSt = idxMiddle;
      sOargs[1].ixEn = sDics->size - BS_IDX_1;
    } else {
      thrdCnt = 1;
      sOargs[0].thrd = NULL;
      sOargs[0].ixSt = BS_IDX_0;
      sOargs[0].ixEn = sDics->size - BS_IDX_1;
    }
              //launching thread[s]:
    for ( int i = 0; i < thrdCnt; i++ )
    { //direct pthreads do not receive properly arguments!
      sOargs[i].thrd = g_thread_new (NULL, s_open_dic_thrd, &sOargs[i]);
    }
  }
  return sDics;
}


/**
 * <p>If settings busy, i.e. indexing a dic.</p>
 * @return if settings busy
 **/
bool
  bsdicsettings_is_busy ()
{
  if ( sOargAdd.thrd != NULL || sOargs[0].thrd != NULL
        || sOargs[1].thrd != NULL )
  {
    return true;
  }
  return false;
}

/* On exit event handler. */
void
  bsdicsettings_on_exit ()
{
  if ( bslog_is_debug (BS_DEBUGL_DICTSETTINGS) )
          { BSLOG_LOG(BSLINFO, "Destroying...\n"); }

  if (sTimer != -1)
  {
    g_source_remove (sTimer);
    sTimer = -1;
  }
  if (sOargAdd.thrd != NULL)
      { g_thread_join (sOargAdd.thrd); }
  if (sOargs[0].thrd != NULL)
      { g_thread_join (sOargs[0].thrd); }
  if (sOargs[1].thrd != NULL)
      { g_thread_join (sOargs[1].thrd); }

  s_free_here ();
  
  if ( sSetnWin != NULL )
      { gtk_widget_destroy (sSetnWin); }
}

/**
 * <p>Settings sSetnWin.</p>
 * @param pMnWin - main sSetnWin
 **/
void
  bsdicsettings_show (GtkWidget *pMnWin)
{
  if ( sSetnWin == NULL )
  {
    //window:
    sSetnWin = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_screen (GTK_WINDOW (sSetnWin), gtk_widget_get_screen (pMnWin));

    gtk_window_set_default_size (GTK_WINDOW (sSetnWin), 400, 150);

    gtk_window_set_title (GTK_WINDOW (sSetnWin), bsi18n_msg ("BsDict, settings"));

    gtk_container_set_border_width (GTK_CONTAINER (sSetnWin), 8);

    gtk_window_set_icon_name (GTK_WINDOW (sSetnWin), "bssoft");

    //widgets:
    GtkWidget *vbox = gtk_vbox_new (FALSE, 0);

    gtk_container_set_border_width (GTK_CONTAINER(vbox), 5);

    gtk_container_add (GTK_CONTAINER (sSetnWin), vbox);

    //gtklist:
    GtkWidget *scrolWin = gtk_scrolled_window_new (NULL, NULL);

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolWin),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_box_pack_start (GTK_BOX (vbox), scrolWin, TRUE, TRUE, 0);

    sGtkList = gtk_list_new ();

    ((GtkList*) sGtkList)->selection_mode = GTK_SELECTION_SINGLE;

    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolWin), sGtkList);

    g_signal_connect (sGtkList, "selection-changed", G_CALLBACK (s_on_row_select), NULL);

        //menu:
    GtkWidget *menu = gtk_menu_new ();

    GtkWidget *menuItm = gtk_menu_item_new_with_label (bsi18n_msg ("Add dictionary"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItm);
    g_signal_connect_swapped(menuItm, "activate",
                    G_CALLBACK (s_on_menu_item_click), sMenuVals + EADD);
    gtk_widget_show (menuItm);

    menuItm = gtk_menu_item_new_with_label (bsi18n_msg ("Save"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItm);
    g_signal_connect_swapped(menuItm, "activate",
                    G_CALLBACK (s_on_menu_item_click), sMenuVals + ESAVE);
    gtk_widget_show (menuItm);

    menuItm = gtk_menu_item_new_with_label (bsi18n_msg ("Toggle on/off"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItm);
    g_signal_connect_swapped(menuItm, "activate",
                    G_CALLBACK (s_on_menu_item_click), sMenuVals + EONOFF);
    gtk_widget_show (menuItm);

    menuItm = gtk_menu_item_new_with_label (bsi18n_msg ("Delete selected"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItm);
    g_signal_connect_swapped(menuItm, "activate",
                    G_CALLBACK (s_on_menu_item_click), sMenuVals + EDELETE);
    gtk_widget_show (menuItm);

        //tool bar:
    GtkWidget *toolBar = gtk_toolbar_new ();

    gtk_box_pack_start(GTK_BOX (vbox), toolBar, FALSE, FALSE, 0);

        //tool bar buttons:
    GtkToolItem *btnUp = gtk_tool_button_new_from_stock (GTK_STOCK_GO_UP);

    gtk_tool_item_set_tooltip_text (btnUp, bsi18n_msg ("Move up"));

    gtk_toolbar_insert(GTK_TOOLBAR (toolBar), btnUp, -1);

    GtkToolItem *btnDown = gtk_tool_button_new_from_stock (GTK_STOCK_GO_DOWN);

    gtk_tool_item_set_tooltip_text (btnDown, bsi18n_msg ("Move down"));

    gtk_toolbar_insert(GTK_TOOLBAR (toolBar), btnDown, -1);

    GtkToolItem *btnMenu = gtk_tool_button_new_from_stock (GTK_STOCK_INDEX);

    gtk_tool_item_set_tooltip_text (btnMenu, bsi18n_msg ("Menu"));

    gtk_toolbar_insert(GTK_TOOLBAR (toolBar), btnMenu, -1);

          //signals:
    g_signal_connect (sSetnWin, "destroy", G_CALLBACK (gtk_widget_destroyed), &sSetnWin);

    g_signal_connect (btnUp, "clicked", G_CALLBACK (s_item_up), NULL);

    g_signal_connect (btnDown, "clicked", G_CALLBACK (s_item_down), NULL);

    g_signal_connect_swapped (btnMenu, "clicked", G_CALLBACK (s_menu_open), menu);

    g_signal_connect (sSetnWin, "delete-event", G_CALLBACK (s_on_close), NULL);
  }
  if ( !gtk_widget_get_visible (sSetnWin) )
        { gtk_widget_show_all (sSetnWin); }

  if ( sTimer == -1 )  //Refresher:
        { sTimer = gdk_threads_add_timeout (REFR_TIME, s_refresh, NULL); }
}
