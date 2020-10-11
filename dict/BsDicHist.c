/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ dictionary history dialog.</p>
 * @author Yury Demidenko
 **/

#include "string.h"

#include "gtk/gtk.h"

#include "BsI18N.h"
#include "BsError.h"
#include "BsDict.h"
#include "BsDicHist.h"

//Shared data:
  //History APP-scoped, elements on demand scoped shared with dict.entry:
static BsStrings *sHist = NULL;
  //current index in non-sorted history:
static int sCuIdx = -1;

//Data:
  //Sorting type AB(true)/historical+manually
static bool sSortAb = false;
  //Do not refresh content of selected entry
static bool sNoRefCnt = false;
  //AB indexes set APP-scoped:
static BsIdxSet *sIdxSetAb = NULL;

//GUI:
  //Widgets:
static GtkWidget *sHisWin = NULL, *sGtkList = NULL;
static GtkToolItem *sBtnUp = NULL, *sBtnDown = NULL;

  //Menu:
typedef enum {
  EREPLACE, EADD, ESAVE, EEXPORT, EDELETE, ECLEAR, ETOGGLE_SORT } EMnVlsDiHi;

static int sMenuVals[] = {
  EREPLACE, EADD, ESAVE, EEXPORT, EDELETE, ECLEAR, ETOGGLE_SORT };

//local lib:
/* Generic info dialog */
static void
  s_dialog_info (char *pMsg)
{
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new (GTK_WINDOW (sHisWin),
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
  dialog = gtk_message_dialog_new (GTK_WINDOW (sHisWin),
    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, pQues);
  response = gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  if ( response == GTK_RESPONSE_OK )
                { return TRUE; }
  return FALSE;
}

/* Choose history to save dialog. */
static char*
  s_choose_fhist ()
{
  GtkWidget *flChsr;
  flChsr = gtk_file_chooser_dialog_new (bsi18n_msg ("Select file to save history"),
                (GtkWindow*) sHisWin, GTK_FILE_CHOOSER_ACTION_SAVE,
                GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN,
                GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(flChsr), FALSE);
  GtkFileFilter *flt = gtk_file_filter_new();
  gtk_file_filter_add_pattern(flt, "*.txt");
  gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (flChsr), flt);
  gtk_dialog_run (GTK_DIALOG (flChsr));

  GFile *gfle = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (flChsr));
  char *ret = NULL;
  if ( gfle != NULL )
  {
    if ( g_file_query_exists (gfle, NULL) )
    {
      if ( s_dialog_confirm ("Overwrite existing file?") )
      {
        ret = g_file_get_path (gfle);
      }
    } else {
      ret = g_file_get_path (gfle);
    }
  }
  gtk_widget_destroy (flChsr);
  return ret;
}

/* Refresh GUI list */
static void
  s_refresh_list ()
{
  int i, j;
  if ( sHist == NULL )
                { return ; }

  gtk_list_clear_items (((GtkList*) sGtkList), 0, -1);

  if ( sSortAb )
  {
    errno = 0;
    if ( sIdxSetAb == NULL )
    {
      sIdxSetAb = bsstrings_create_idxset (sHist, sHist->size + BS_IDX_100);
    }
    else if ( sIdxSetAb->size == BS_IDX_0 )
    {
      bsstrings_redo_idxset (sHist, sIdxSetAb, sHist->size + BS_IDX_100);
    }
  }

  for ( i = sHist->size - 1; i >= 0; i-- )
  {
    j = i;
    if ( sSortAb && sIdxSetAb != NULL && sIdxSetAb->size == sHist->size )
                { j = sIdxSetAb->vals[i]; }
    GtkWidget *lstItm = gtk_list_item_new_with_label (sHist->vals[j]->val);

    gtk_container_add (GTK_CONTAINER (sGtkList), lstItm);

    gtk_widget_show(lstItm);
  }
  errno = 0;
  if ( sCuIdx != -1 )
  {
    i = sHist->size - 1 - sCuIdx;
    if ( sSortAb )
    {
      for ( j = 0; j <= sIdxSetAb->size; j++ )
      {
        if ( strcmp (sHist->vals[sCuIdx]->val, sHist->vals[sIdxSetAb->vals[j]]->val) == 0 )
        {
          i = sHist->size - 1 - j;
          break;
        }
      }
    }
    gtk_list_select_item (((GtkList*) sGtkList), i);
  }
}

/* On history changed (only add/delete) event */
static void
  s_on_hist_adddel ()
{
  if ( sIdxSetAb != NULL )
        { bsidxset_clear (sIdxSetAb); }
}

/* Save settings. */
static gboolean
  s_save (EMnVlsDiHi pTyp)
{
  if ( sHist == NULL || sHist->size == BS_IDX_0 )
                                        { return FALSE; }
  BsString *pth = NULL;
  if ( pTyp == ESAVE )
  {
    const char *hdir = g_get_home_dir();
    BS_DO_CEE_RETF (pth = bsstring_newbuf (strlen (hdir) + 16))
    strcpy (pth->val, hdir);
    strcat (pth->val, BS_DICHIST_FLNM);
  } else {
    char *cpth = s_choose_fhist();
    if ( cpth != NULL )
    { 
      BS_DO_CEE_RETF (pth = bsstring_new (cpth))
    } else {
      return FALSE;
    }
  }
  FILE *flConf = fopen (pth->val, "w");
  bsstring_free (pth);

  if ( flConf == NULL )
  {
    errno = BSE_OPEN_FILE;
    BSLOG_ERR
    return FALSE;
  }
  for ( int i = 0; i < sHist->size; i++ )
  {
    fprintf (flConf, "%s\n", sHist->vals[i]->val);
  }
  fclose (flConf);
  return TRUE;
}

/* Open history dialog. */
static void
  s_open_fhist (EMnVlsDiHi pTyp)
{
  bsdichist_lget ();
  
  if ( sHist == NULL )
              { return; }
  
  GtkWidget *flChsr;
  flChsr = gtk_file_chooser_dialog_new (bsi18n_msg ("Select history"),
                (GtkWindow*) sHisWin, GTK_FILE_CHOOSER_ACTION_OPEN,
                GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN,
                GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(flChsr), FALSE);

  GtkFileFilter *flt = gtk_file_filter_new();

  gtk_file_filter_add_pattern(flt, "*.txt");

  gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (flChsr), flt);
  gtk_dialog_run (GTK_DIALOG (flChsr));

  GFile *file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (flChsr));
  if ( file != NULL )
  {
    //it maybe folder on close, also filter wrong!
    gchar *bnm = g_file_get_basename (file);
    char *ext = strrchr(bnm, '.');
    if ( ext != NULL )
    {
      FILE *flConf = fopen(g_file_get_path (file), "r");
      char buf[300];

      if ( flConf == NULL )
                    { goto out; }

      
      if ( pTyp == EREPLACE && sHist->size > BS_IDX_0 )
      { 
        if ( s_dialog_confirm (bsi18n_msg ("Replace history?")) )
        {
          bsstrings_clear (sHist);
          sCuIdx = -1;
          s_on_hist_adddel ();
          //s_refresh_list (); //TODO excessive?
        } else {
          return;
        }
      }

      while ( !feof (flConf) && !ferror (flConf) )
      {
        int crdd = fscanf (flConf, "%299[^\n]", buf);
        if ( crdd == 1 )
        {
          BS_DO_CEE_BREAK (BsString *str = bsstring_new (buf))
          if ( bsstrings_find (sHist, str) == BS_IDX_NULL )
          {
            if ( bsstrings_add_inc (sHist, str, BS_IDX_10) == BS_IDX_NULL )
            {
              errno = 0;
              bsstring_free (str);
              break;
            }
          } else {
            bsstring_free (str);
          }
        }
        fscanf (flConf, "%*[^\n]");
        fscanf (flConf, "%*[\n]");
      }
      sCuIdx = sHist->size - 1;
      fclose (flConf);
      s_on_hist_adddel ();
      s_refresh_list ();
    }
  }
out:
  gtk_widget_destroy (flChsr);
}

/* move item up */
static void
  s_item_up (gpointer pData)
{
  if ( sCuIdx >= 0 && bsstrings_move_up (sHist, sCuIdx) )
  {
    sCuIdx++;
    sNoRefCnt = true;
    s_refresh_list ();
  }
}

/* move item down */
static void
  s_item_down (gpointer pData)
{
  if ( sCuIdx > 0 && bsstrings_move_down(sHist, sCuIdx) )
  {
    sCuIdx--;
    sNoRefCnt = true;
    s_refresh_list ();
  }
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
  if ( sGtkList != NULL //invoked on destroy!?
    && ((GtkList*) sGtkList)->selection != NULL )
  {
    int i = g_list_index (((GtkList*) sGtkList)->children,
                          ((GtkList*) sGtkList)->selection->data);
    
    sCuIdx = sHist->size - 1 - i;
    if ( sSortAb )
    {
      for ( int j = 0; j <= sIdxSetAb->size; j++ )
      {
        if ( strcmp (sHist->vals[j]->val, sHist->vals[sIdxSetAb->vals[sCuIdx]]->val) == 0 )
        {
          sCuIdx = j;
          break;
        }
      }
    }
    if ( !sNoRefCnt )
    {
      bsdict_show (sHist->vals[sCuIdx]);
    } else {
      sNoRefCnt = false;
    }
  }
}

/* On menu item select event handler. */
static void
  s_on_menu_item_click (int *pMenuVal)
{
  if ( *pMenuVal == EADD )
  {
    s_open_fhist (EADD);
  }
  else if ( *pMenuVal == EREPLACE )
  {
    s_open_fhist (EREPLACE);
  }
  else if ( *pMenuVal == ECLEAR )
  {
    if ( sHist != NULL && sHist->size > BS_IDX_0
                && s_dialog_confirm (bsi18n_msg ("Clear history?")) )
    {
      bsdict_on_histclear ();
      bsstrings_clear (sHist);
      sCuIdx = -1;
      s_on_hist_adddel ();
      s_refresh_list ();
    }
  }
  else if ( *pMenuVal == EDELETE )
  {
    if ( sCuIdx != -1 )
    {
      if ( !s_dialog_confirm (bsi18n_msg ("Delete selected?")) )
                  { return; }
      if ( sHist->size == 1 )
              { bsdict_on_histclear (); }
      errno = 0;
      bsdict_on_histclear ();
      bsstrings_remove_shrink (sHist, sCuIdx);
      if ( sCuIdx > 0 )
      {
        sCuIdx--;
      } else if ( sHist->size > 1 ) {
        sCuIdx = sHist->size - 1;
      } else {
        sCuIdx = -1;
      }
      s_on_hist_adddel ();
      s_refresh_list ();
      errno = 0;
    }
  }
  else if ( *pMenuVal == ETOGGLE_SORT )
  {
    sSortAb = !sSortAb;
    gtk_widget_set_sensitive ((GtkWidget*) sBtnUp, !sSortAb);
    gtk_widget_set_sensitive ((GtkWidget*) sBtnDown, !sSortAb);
    sNoRefCnt = true;
    s_refresh_list ();
  }
  else if ( *pMenuVal == ESAVE )
  {
    if ( s_save (ESAVE) )
      { s_dialog_info (bsi18n_msg ("History have been saved!")); }
  }
  else if ( *pMenuVal == EEXPORT )
  {
    if ( s_save (EEXPORT) )
      {  s_dialog_info (bsi18n_msg ("History have been exported!")); }
  }
}

//public lib:
/* On exit event handler. */
void
  bsdichist_on_exit ()
{
  if ( bslog_is_debug (BS_DEBUGL_DICHIST) )
          { BSLOG_LOG (BSLINFO, "Destroying...\n"); }
  s_save (ESAVE);
  sHist = bsstrings_free (sHist);
  sIdxSetAb = bsidxset_free (sIdxSetAb);
  if ( sHisWin != NULL )
          { gtk_widget_destroy (sHisWin); }
}

/**
 * <p>Lazy get history. This is also for moving inside history.
 * Clears errno if error.</p>
 * @return history, maybe NULL or empty (size == 0)
 * @clears errno if error (only inner-self-handling)
 **/
BsStrings*
  bsdichist_lget ()
{
  if ( sHist != NULL )
                { return sHist; }
    
  bool isDbg = bslog_is_debug (BS_DEBUGL_DICHIST);
#ifdef BS_DEBUG_BASIC_ENABLED
  if ( !isDbg )
  {
    if (bslog_get_debug_ceiling () == 0 )
    {
      bslog_set_debug_floor (BS_DEBUGL_DICHIST);
      bslog_set_debug_ceiling (BS_DEBUGL_DICHIST);
    } else {
      bslog_add_dbgrange (BS_DEBUGL_DICHIST, BS_DEBUGL_DICHIST);
    }
    isDbg = bslog_is_debug (BS_DEBUGL_DICHIST);
  }
#endif

  BS_DO_CEERR (sHist = bsstrings_new (BS_IDX_100))
  if ( errno != 0 )
                { return sHist; }

  const char *homed = g_get_home_dir();
  char fpth[strlen(homed) + 15];

  strcpy(fpth, homed);
  strcat(fpth, BS_DICHIST_FLNM);
  FILE *flConf = fopen(fpth, "r");

  if ( flConf == NULL )
                { return sHist; }

  char buf[300];
  while ( !feof (flConf) && !ferror (flConf) )
  {
    int crdd = fscanf (flConf, "%299[^\n]", buf);
    if ( crdd == 1 )
    {
      BS_DO_CEE_OUT (BsString *str = bsstring_new (buf))
      if ( bsstrings_add_inc (sHist, str, BS_IDX_10) == BS_IDX_NULL )
      {
        bsstring_free (str);
        errno = 0;
      }
    }
    fscanf (flConf, "%*[^\n]");
    fscanf (flConf, "%*[\n]");
  }
  if ( sHist->size > BS_IDX_0 )
            { sCuIdx = sHist->size - 1; }
out:
  errno = 0;
  fclose (flConf);
  return sHist;
}

/**
 * <p>Get current IDX. This is for moving inside history.</p>
 * @return current IDX, -1 means NULL
 **/
int
  bsdichist_get_cuidx ()
{
  return sCuIdx;
}

/**
 * <p>Try to set current IDX. This is for moving inside history.
 * If new is out of bounds, then just warn logging.</p>
 * @param current IDX
 **/
void
  bsdichist_tset_cuidx (int pCuIdx)
{
  if ( sHist == NULL || pCuIdx < 0 || pCuIdx > sHist->size - 1 )
  {
    BSLOG_LOG (BSLWARN, "Can't set CUIDX=%d\n", pCuIdx);
    return;
  }
  sCuIdx = pCuIdx;
}

/**
 * <p>Add string without duplicates. Clears errno if error.</p>
 * @param pStr - string not NULL
 * @return duplicate's index or BS_IDX_NULL if OK
 * @set errno if error
 **/
BS_IDX_T
  bsdichist_add_rdi (BsString *pStr)
{
  bsdichist_lget ();
  if ( sHist == NULL || pStr == NULL )
                                { return BS_IDX_NULL; }

  errno = 0;
  BS_IDX_T l = bsstrings_find (sHist, pStr);
  if ( l == BS_IDX_NULL )
  {
    BS_DO_CEERR (l = bsstrings_add_inc (sHist, pStr, BS_IDX_10))
    if ( errno == 0 )
    {
      sCuIdx = l;
      s_on_hist_adddel ();
      l = BS_IDX_NULL;
    }
  } else {
    sCuIdx = l;
    //bsstrings_move (sHist, l, sHist->size - 1); keep hystory!!!
    //l = sHist->size - 1;
  }
  if ( errno == 0 && sHisWin != NULL )
  {
    sNoRefCnt = true;
    s_refresh_list ();
  }
  return l;
}

/**
 * <p>Show window.</p>
 * @param pMnWin - main window
 **/
void 
  bsdichist_show (GtkWidget *pMnWin)
{
  if ( sHisWin == NULL )
  {
    //window:
    sHisWin = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_screen (GTK_WINDOW (sHisWin), gtk_widget_get_screen (pMnWin));

    gtk_window_set_default_size (GTK_WINDOW (sHisWin), 200, 450); //TODO 1 previous user's chosen size and position

    gtk_window_set_title (GTK_WINDOW (sHisWin), bsi18n_msg ("BsDict, history"));

    gtk_container_set_border_width (GTK_CONTAINER (sHisWin), 8);

    gtk_window_set_icon_name (GTK_WINDOW (sHisWin), "bssoft");

    //widgets:
    GtkWidget *vbox = gtk_vbox_new (FALSE, 0);

    gtk_container_set_border_width (GTK_CONTAINER(vbox), 5);

    gtk_container_add (GTK_CONTAINER (sHisWin), vbox);

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

    GtkWidget *menuItm = gtk_menu_item_new_with_label (bsi18n_msg ("Add from file"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItm);
    g_signal_connect_swapped(menuItm, "activate",
                    G_CALLBACK (s_on_menu_item_click), sMenuVals + EADD); //TODO 2 pass only enum val
    gtk_widget_show (menuItm);

    menuItm = gtk_menu_item_new_with_label (bsi18n_msg ("Replace from file"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItm);
    g_signal_connect_swapped(menuItm, "activate",
                    G_CALLBACK (s_on_menu_item_click), sMenuVals + EREPLACE);
    gtk_widget_show (menuItm);

    menuItm = gtk_menu_item_new_with_label (bsi18n_msg ("Save into file"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItm);
    g_signal_connect_swapped(menuItm, "activate",
                    G_CALLBACK (s_on_menu_item_click), sMenuVals + EEXPORT);
    gtk_widget_show (menuItm);

    menuItm = gtk_menu_item_new_with_label (bsi18n_msg ("Save"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItm);
    g_signal_connect_swapped(menuItm, "activate",
                    G_CALLBACK (s_on_menu_item_click), sMenuVals + ESAVE);
    gtk_widget_show (menuItm);

    menuItm = gtk_menu_item_new_with_label (bsi18n_msg ("Toggle sort"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItm);
    g_signal_connect_swapped(menuItm, "activate",
                    G_CALLBACK (s_on_menu_item_click), sMenuVals + ETOGGLE_SORT);
    gtk_widget_show (menuItm);

    menuItm = gtk_menu_item_new_with_label (bsi18n_msg ("Delete selected"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItm);
    g_signal_connect_swapped(menuItm, "activate",
                    G_CALLBACK (s_on_menu_item_click), sMenuVals + EDELETE);
    gtk_widget_show (menuItm);

    menuItm = gtk_menu_item_new_with_label (bsi18n_msg ("Clear"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItm);
    g_signal_connect_swapped(menuItm, "activate",
                    G_CALLBACK (s_on_menu_item_click), sMenuVals + ECLEAR);
    gtk_widget_show (menuItm);

        //tool bar:
    GtkWidget *toolBar = gtk_toolbar_new ();

    gtk_box_pack_start(GTK_BOX (vbox), toolBar, FALSE, FALSE, 0);

        //tool bar buttons:
    sBtnUp = gtk_tool_button_new_from_stock (GTK_STOCK_GO_UP);

    gtk_tool_item_set_tooltip_text (sBtnUp, bsi18n_msg ("Move up"));

    gtk_toolbar_insert(GTK_TOOLBAR (toolBar), sBtnUp, -1);

    sBtnDown = gtk_tool_button_new_from_stock (GTK_STOCK_GO_DOWN);

    gtk_tool_item_set_tooltip_text (sBtnDown, bsi18n_msg ("Move down"));

    gtk_toolbar_insert(GTK_TOOLBAR (toolBar), sBtnDown, -1);

    GtkToolItem *btnMenu = gtk_tool_button_new_from_stock (GTK_STOCK_INDEX);

    gtk_tool_item_set_tooltip_text (btnMenu, bsi18n_msg ("Menu"));

    gtk_toolbar_insert(GTK_TOOLBAR (toolBar), btnMenu, -1);

          //signals:
    g_signal_connect (sHisWin, "destroy", G_CALLBACK (gtk_widget_destroyed), &sHisWin);

    g_signal_connect (sBtnUp, "clicked", G_CALLBACK (s_item_up), NULL);

    g_signal_connect (sBtnDown, "clicked", G_CALLBACK (s_item_down), NULL);

    g_signal_connect_swapped (btnMenu, "clicked", G_CALLBACK (s_menu_open), menu);
  }

  if ( !gtk_widget_get_visible (sHisWin) )
  {
    gtk_widget_set_sensitive ((GtkWidget*) sBtnUp, !sSortAb);
    gtk_widget_set_sensitive ((GtkWidget*) sBtnDown, !sSortAb);
    bsdichist_lget ();
    gtk_widget_show_all (sHisWin);
    s_refresh_list ();
  }
}

