/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ GTK based dictionary final assembly.</p>
 * @author Yury Demidenko
 **/
#include "time.h"
#include "locale.h"
#include "string.h"
#include "wctype.h"
#include "stdlib.h"

#include "gtk/gtk.h"

#include "BsI18N.h"
#include "BsFatalLog.h"
#include "BsError.h"
#include "BsDict.h"
#include "BsDictSettings.h"
#include "BsDicHist.h"
#include "BsDicLsa.h"

#define BS_DEBUGL_DICT 40000
//Menu:
enum EMenuVals { EBSDM_SETTINGS, EBSDM_HISTORY };
static int sMenuVals[] = { EBSDM_SETTINGS, EBSDM_HISTORY };

//Last typed string:
#define BSL_LAST_STR_BUF_LN 199
static char sLastCstr[BSL_LAST_STR_BUF_LN + 1];

//For making double cliked word:
    //Last clicked word
static char sLstClkCstr[BSL_LAST_STR_BUF_LN + 1];
static long sLstClkMsec = -1L;

static GtkWidget *sMainWin = NULL, *sEntry = NULL, *sView = NULL;
static GtkListStore *sComplLst = NULL;

static BsDiFdWds *sDicsWrds = NULL;

    //request scoped collection to free:
static BsDiDtT2s *sAuDtSet = NULL;

/* Generic info dialog */
static void
  s_dialog_info (char *pMsg)
{
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new (GTK_WINDOW (sMainWin),
				   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				   GTK_MESSAGE_INFO, GTK_BUTTONS_OK, pMsg);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

/* Free allocated here data */
static void
  s_free_here()
{
  sDicsWrds = bsdifdwds_free (sDicsWrds);
  sAuDtSet = bsdidtt2s_free (sAuDtSet);
}

/* Open menu event */
static void
  s_menu_open (GtkWidget *pMnu)
{
  gtk_menu_popup (GTK_MENU (pMnu), NULL, NULL,
              NULL, NULL, 0, gtk_get_current_event_time ());
}

/* Add all tags into view */
static void
  s_add_all_tags ()
{
  GtkTextBuffer *buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (sView));
  // basic:
  gtk_text_buffer_create_tag (buf, "heading",
			      "size", 15 * PANGO_SCALE, NULL);

  gtk_text_buffer_create_tag (buf, "invisible",
			      "invisible", NULL, NULL);

  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_ITALIC),
			      "style", PANGO_STYLE_ITALIC, NULL);
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_BOLD),
			      "weight", PANGO_WEIGHT_BOLD, NULL);  
  // colors:
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_GRAY),
			      "foreground", "gray", NULL);
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_GREEN),
			      "foreground", "green", NULL);
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_RED),
			      "foreground", "red", NULL);
  // sound:
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_AUDIO),
			      "left_margin", 20, NULL);
  // margins:
  int spc = 10;
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_TAB1),
                              "left_margin", spc,  NULL);
  spc += 5;
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_TAB2),
                              "left_margin", spc,  NULL);
  spc += 4;
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_TAB3),
                              "left_margin", spc,  NULL);
  spc += 3;
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_TAB4),
                              "left_margin", spc,  NULL);
  spc += 3;
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_TAB5),
                              "left_margin", spc,  NULL);
  spc += 3;
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_TAB6),
                              "left_margin", spc,  NULL);
  spc += 3;
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_TAB7),
                              "left_margin", spc,  NULL);
  spc += 3;
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_TAB8),
                              "left_margin", spc,  NULL);
  spc += 2;
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_TAB9),
                              "left_margin", spc,  NULL);
  spc += 2;
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_TAB10),
                              "left_margin", spc,  NULL);
  // tooltip:
  gtk_text_buffer_create_tag (buf, bshyptag_name (EBSHT_TOOLTIP),
			      "foreground", "green", NULL);
}

/* On close event handler. */
static gboolean
  s_on_close (GtkWidget *pWidg, GdkEvent *pEvent, gpointer pDt)
{
  if ( bsdicsettings_is_busy () )
  {
    s_dialog_info (bsi18n_msg ("Please wait for dictionary indexing!"));
    return TRUE;
  }
  return FALSE;
}

static void 
  s_play_au (GtkWidget *pBtn, gpointer pDt)
{
  //vars:
  FILE *oggf;
  BsStrBuf *wavRam, *oggRam, *sbuf;
  BsDiDtT2* diDt;
  //init0
  oggf = NULL; wavRam = NULL; oggRam = NULL; sbuf = NULL;
  //code:
  diDt = (BsDiDtT2*) pDt;
  BS_DO_CEE_OUT (sbuf = bsstrbuf_new (diDt->wrd->len + 53)) 
  bsstrbuf_add_str_inc (sbuf, "/tmp/", 5);
  bsstrbuf_add_str_inc (sbuf, diDt->wrd->val, 5);
  bsstrbuf_add_str_inc (sbuf, ".ogg", 5);
  bsstrbuf_add_inc (sbuf, 0, 5);
  oggf = fopen (sbuf->vals, "rb");
  if ( oggf == NULL )
  {
    BS_DO_CEE_OUT (wavRam = bsdiclsa_readau (diDt->diIx, diDt->ofst, diDt->len, true))
    GdWavHeader *wh = (GdWavHeader*) wavRam->vals;
    BS_DO_E_OUT (oggRam = bsdiclsa_encvorbis (wavRam, wh->channels, wh->samplesPerSec))
    oggf = fopen (sbuf->vals, "wb");
    BS_IF_EN_OUT (oggf == NULL, BSE_OPEN_FILE)
    fwrite (oggRam->vals, 1, oggRam->size, oggf);
    fclose (oggf);
    oggf = NULL;
  }
  bsstrbuf_ins_str_inc (sbuf, "ffplay -nodisp -autoexit -loglevel quiet ", 0, 5);
  system (sbuf->vals);
out:
  if ( oggf != NULL )
            { fclose (oggf); }
  bsstrbuf_free (sbuf);
  bsstrbuf_free (wavRam);
  bsstrbuf_free (oggRam);
}


/* Write dictionary's word description, buffer must be initialized before it. */
static gboolean 
  s_write_wrddesc (char *pCstr, BsDiIxBs *pDiIx,
                   BsDiFdWd *pFdWrd, GtkTextIter *pEnd)
{
  BsDicObjs *wdics = bsdicsettings_lget_dics ();
  if ( wdics == NULL || wdics->size < BS_IDX_1 )
                  { return FALSE; }

  BS_DO_CEE_RETF (BS_IDX_T idx = bsdicobjs_find_diix (wdics, pDiIx))
  if ( idx == BS_IDX_NULL )
                  { return FALSE; }

  BS_DO_CEE_RETF (BsHypStrs *hyStrs = wdics->vals[idx]->diix_read (pDiIx, pFdWrd))
  if ( hyStrs == NULL )
                  { return FALSE; }

  GtkTextBuffer *buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (sView));
  GtkTextIter start;
  gtk_text_buffer_insert (buf, pEnd, "\n", -1);
  gtk_text_buffer_insert_with_tags_by_name (buf, pEnd, pCstr, -1, "heading", NULL);

  gtk_text_buffer_insert (buf, pEnd, " - ", -1);
  gtk_text_buffer_insert (buf, pEnd, pDiIx->head->nme->val, -1);

  gtk_text_buffer_insert (buf, pEnd, "\n\n", -1);
  for ( int i = 0; i < hyStrs->bsize; i++ )
  {
    if ( hyStrs->vals[i] != NULL )
    {
      gtk_text_buffer_insert (buf, pEnd, hyStrs->vals[i]->str->val, -1);
      if ( hyStrs->vals[i]->tags != NULL && hyStrs->vals[i]->tags->size > BS_IDX_0 )
      {
        start = *pEnd;
        char *strEnd = hyStrs->vals[i]->str->val + hyStrs->vals[i]->str->len;
        int len = g_utf8_pointer_to_offset (hyStrs->vals[i]->str->val, strEnd);
        gtk_text_iter_backward_chars (&start, len);
        for ( int j = 0; j < hyStrs->vals[i]->tags->size; j++ )
        {
          char *tagNm = bshyptag_name (hyStrs->vals[i]->tags->vals[j]);
          if ( tagNm != NULL )
          {
            gtk_text_buffer_apply_tag_by_name (buf, tagNm, &start, pEnd);
            if ( hyStrs->vals[i]->tags->vals[j] == EBSHT_AUDIO )
            {
              GtkTextChildAnchor *ancr = gtk_text_buffer_create_child_anchor (buf, pEnd);
              GtkWidget *btn = gtk_button_new_with_label (bsi18n_msg ("Play"));
              gtk_text_view_add_child_at_anchor ((GtkTextView*) sView, btn, ancr);
              gtk_widget_show_all (btn);
              BsDiDtT2 *diDt = bsdidtt2_new (hyStrs->vals[i]->str, (BsDiIxT2Bs*) pDiIx, hyStrs->vals[i]->ofst, hyStrs->vals[i]->len);
              if ( diDt != NULL )
              { 
                g_signal_connect (btn, "clicked", G_CALLBACK (s_play_au), diDt);
                bsdidtt2s_add_inc (sAuDtSet, diDt, BS_IDX_10);
              }
              errno = 0;
             }
          } else {
            BSLOG_LOG (BSLWARN, "There is no hyper-tag#%d\n", hyStrs->vals[i]->tags->vals[j])
          }
        }
      }
    }
  }
  gtk_text_buffer_insert (buf, pEnd, "\n\n", -1);
  hyStrs = bshypstrs_free (hyStrs); //text buffer hold values
  return TRUE;
}

/* On entry word selected event */
gboolean 
  s_word_selected (GtkEntryCompletion *pCmpl,
               GtkTreeModel *pMdl, GtkTreeIter *pItr)
{
  gchar *cstr = NULL;
  
  gtk_tree_model_get (GTK_TREE_MODEL (pMdl), pItr, 0, &cstr, -1);
  gtk_entry_set_text (GTK_ENTRY (sEntry), cstr); //TODO 1 propagation
  gtk_editable_set_position (GTK_EDITABLE (sEntry), -1);
  
  BsDiFdWd *dcswrd = bsdifdwds_find (sDicsWrds, cstr);
  if ( dcswrd != NULL )
  {
    GtkTextBuffer *buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (sView));
    gtk_text_buffer_set_text (buf, "", 0);
    GtkTextIter end;
    gtk_text_buffer_get_iter_at_offset (buf, &end, 0);
    int d;
	bsdidtt2s_clear (sAuDtSet);
    for ( d = 0; d < dcswrd->dicOfLns->size; d ++ )
    {
      s_write_wrddesc (cstr, dcswrd->dicOfLns->vals[d]->diIx, dcswrd, &end);
    }
    for ( d = 0; d < dcswrd->dicOfsts->size; d ++ )
    {
      s_write_wrddesc (cstr, dcswrd->dicOfsts->vals[d]->diIx, dcswrd, &end);
    }
    BS_DO_CEE_OUT (BsString *strh = bsstring_clone (dcswrd->wrd))
    BS_IDX_T dupIdx = bsdichist_add_rdi (strh);
    if ( errno != 0 || dupIdx != BS_IDX_NULL )
            { bsstring_free (strh); }
  } else {
    BSLOG_LOG (BSLERROR, "Can't find %s in dics-words\n", cstr );
  }
out:
  g_free (cstr);

  return TRUE;
}

/* On entry key-down event */
static gboolean
  s_on_keydown (GtkWidget *pWdg, GdkEventKey *pEv, gpointer pDt)
{
  const char *cstr = gtk_entry_get_text ((GtkEntry*) sEntry);
  if ( cstr == NULL || strlen (cstr) == 0 //filter trash
          || strncmp (sLastCstr, cstr, BSL_LAST_STR_BUF_LN) == 0 ) //filter cursor moving
                              { return TRUE; }

  BsDicObjs *wdics = bsdicsettings_lget_dics ();
  if ( wdics == NULL || wdics->size < BS_IDX_1 )
                              { return TRUE; }

  strncpy (sLastCstr, cstr, BSL_LAST_STR_BUF_LN);
  
  gtk_list_store_clear (sComplLst);
  GtkTreeIter iter;
  bsdifdwds_clear (sDicsWrds);

  for ( int i = 0; i < wdics->size; i++ )
  {
    errno = 0;
    if ( wdics->vals[i]->opSt->stt == EBSDS_OPENED )
    {
      BS_DO_E_CONT (wdics->vals[i]->diixfind_mtch (wdics->vals[i]->diIx, sDicsWrds, sLastCstr))
    }
  }
  for ( int i = 0; i < sDicsWrds->size; i++ )
  {
    gtk_list_store_append (sComplLst, &iter);
    gtk_list_store_set (sComplLst, &iter, 0, sDicsWrds->vals[i]->wrd->val, -1);
  }
  return TRUE;
}

/**
 * <p>On mouse double click/tap.
 * If this is a word, but not current or tooltip, then seek word.
 * </p>
 * */
static gboolean
  s_event_after (GtkWidget *pTxView, GdkEvent  *pEv)
{
  if ( bsdichist_lget () == NULL )
            { return FALSE; }

  if ( pEv->type != GDK_BUTTON_RELEASE )
            { return FALSE; }

  if ( ((GdkEventButton*) pEv)->button != 1 )
            { return FALSE; }

  GtkTextBuffer *buf = NULL;
  GtkTextIter start, end, iter;
  
  buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (pTxView));
  
  gtk_text_buffer_get_selection_bounds (buf, &start, &end);
  if ( gtk_text_iter_get_offset (&start) != gtk_text_iter_get_offset (&end) )
            { return FALSE; }

  gint x, y;
  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (pTxView), 
                    GTK_TEXT_WINDOW_WIDGET, ((GdkEventButton*) pEv)->x,
                    ((GdkEventButton*) pEv)->y, &x, &y);
  gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (pTxView), &iter, x, y);

  if ( !gtk_text_iter_inside_word (&start) )
            { return FALSE; }

  start = iter;
  if ( !gtk_text_iter_backward_word_start (&start) )
            { return FALSE; }

  end = iter;
  if ( !gtk_text_iter_forward_word_end (&end) )
            { return FALSE; }

  // return if no text or it's equal to current:
  char *cstr = gtk_text_iter_get_visible_slice (&start, &end);

  if ( cstr == NULL )
            { return FALSE; }

  int len = strlen (cstr);
  if ( len == 0 )
            { return FALSE; }

  GtkTextTag *tagToolTip = gtk_text_tag_table_lookup (buf->tag_table,
                                    bshyptag_name (EBSHT_TOOLTIP));

  GSList *tags = gtk_text_iter_get_tags (&iter);
  for ( GSList *tagp = tags;  tagp != NULL;  tagp = tagp->next )
  {
      GtkTextTag *tag = tagp->data;
      if ( tag == tagToolTip )
                      { return FALSE; }
  }

  //to lower case: //TODO 1 into a lib method
  char cstrl[len + 5];
  BS_WCHAR_T wstr[len + 5]; //small buffer cause SEGF!
  errno = 0;
  int i = mbstowcs (wstr, cstr, len + 1);
  if ( i <= 0 || errno != 0 )
  {
    errno = 0;
    BSLOG_LOG (BSLWARN, "mbstowcs fail on %s, rz=%d\n", cstr, i)
    strcpy (cstrl, cstr);
  } else {
    for ( i = 0; ; i++ )
    {
      if ( wstr[i] == 0 )
                      { break; }
      if ( iswalpha (wstr[i]) )
                      { wstr[i] = towlower (wstr[i]); }
    }
    i = wcstombs (cstrl, wstr, len + 2);
    if ( i <= 0 || errno != 0 )
    {
      errno = 0;
      BSLOG_LOG (BSLWARN, "wcstombs fail on %ls, rz=%d\n", wstr, i)
      strcpy (cstrl, cstr);
    }
  }
  struct timespec tsp;
  if ( timespec_get(&tsp, TIME_UTC) != 0 )
  {
    long msec = tsp.tv_sec * 1000L + (tsp.tv_nsec / 1000000LL) % 1000L;
    long tmDif = msec - sLstClkMsec;
    if ( tmDif < 50L || tmDif > 1000L
      || strncmp (sLstClkCstr, cstrl, BSL_LAST_STR_BUF_LN) != 0 )
    {
      strncpy (sLstClkCstr, cstrl, BSL_LAST_STR_BUF_LN);
      sLstClkMsec = msec;
      return FALSE;
    }
  }
  BS_DO_CEE_RETF (BsString *str = bsstring_new (cstrl))
  BS_IDX_T dupIdx = bsdichist_add_rdi ( str);
  if ( dupIdx != BS_IDX_NULL )
  {
    bsstring_free (str);
    str = bsdichist_lget ()->vals[dupIdx];
  }
  bsdict_show (str);
  return FALSE;
}

/* On menu item select event handler. */
static void
  s_on_menu_item_click (int *pMenuVal)
{
  if ( *pMenuVal == EBSDM_SETTINGS ) {
    bsdicsettings_show (sMainWin);
    return;
  }
  if ( *pMenuVal == EBSDM_HISTORY ) {
    bsdichist_show (sMainWin);
    return;
  }
}

/* On exit event handler. */
static void s_on_exit() {
  if ( bslog_is_debug (BS_DEBUGL_DICT) )
          { BSLOG_LOG(BSLINFO, "Destroying...\n"); }

  bsdichist_on_exit ();
  bsdicsettings_on_exit ();
  s_free_here ();
  sMainWin = NULL;
  bslog_destroy ();
  gtk_main_quit ();
}

//public lib:

/**
 * <p>Clear cause history cleared.</p>
 **/
void
  bsdict_on_histclear ()
{
  if ( sMainWin == NULL )
              { return; }

  gtk_editable_delete_text (GTK_EDITABLE (sEntry), 0, -1);
  //gtk_entry_set_text (GTK_ENTRY (sEntry), "");
  gtk_editable_set_position (GTK_EDITABLE (sEntry), -1);
}

/**
 * <p>Show selected string.</p>
 * @param pStr - string not NULL
 * @return if exactly word found
 * @clears errno if error (only inner-self-handling)
 **/
bool
  bsdict_show (BsString *pStr)
{
  if ( pStr == NULL ||  pStr->val == NULL || sMainWin == NULL )
                    { return false; }
  
  BsDicObjs *wdics = bsdicsettings_lget_dics ();
  if ( wdics == NULL || wdics->size < BS_IDX_1 )
                              { return false; }

  gtk_entry_set_text (GTK_ENTRY (sEntry), pStr->val);
  gtk_editable_set_position (GTK_EDITABLE (sEntry), -1);

  GtkTextBuffer *buf = NULL;
  GtkTextIter end;
  bool ret = false;
  BS_DO_CEE_RETF (BsDiFdWds *fdWrds = bsdifdwds_new (BS_IDX_100))
  for ( int d = 0; d < wdics->size; d++ )
  {
    if ( wdics->vals[d]->opSt->stt == EBSDS_OPENED )
    {
      BS_DO_CEE_CONT (wdics->vals[d]->diixfind_mtch (wdics->vals[d]->diIx, fdWrds, pStr->val))
    }
  }
  bsdidtt2s_clear (sAuDtSet);
  for ( int i = 0; i < fdWrds->size; i++ )
  {
    if ( bsstring_compare (fdWrds->vals[i]->wrd, pStr) == 0 )
    {
      ret = true;
      if ( buf == NULL )
      {
        buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (sView));
        gtk_text_buffer_set_text (buf, "", 0);
        gtk_text_buffer_get_iter_at_offset (buf, &end, 0);
      }
      int j;
      for ( j = 0; j < fdWrds->vals[i]->dicOfLns->size; j++ )
      {
        s_write_wrddesc (pStr->val, fdWrds->vals[i]->dicOfLns->vals[j]->diIx,
                         fdWrds->vals[i], &end);
      }
      for ( j = 0; j < fdWrds->vals[i]->dicOfsts->size; j++ )
      {
        s_write_wrddesc (pStr->val, fdWrds->vals[i]->dicOfsts->vals[j]->diIx,
                         fdWrds->vals[i], &end);
      }
    }
  }
  bsdifdwds_free (fdWrds);
  return ret;
}

/**
 * <p>Selected dictionary, so scroll view to its text, if any.</p>
 * @param pDiIx - dic not NULL
 * @clears errno if error (only inner-self-handling)
 **/
void
  bsdict_dic_sel (BsDiIxBs *pDiIx)
{
  if ( pDiIx == NULL || sMainWin == NULL )
                    { return; }
  
}

/**
 * <p>Dictionary on/off switched, so hide/show its text from view, if any.</p>
 * @param pDiIx - dic not NULL
 * @clears errno if error (only inner-self-handling)
 **/
void
  bsdict_dic_switched (BsDiIxBs *pDiIx)
{
  if ( pDiIx == NULL || sMainWin == NULL )
                    { return; }
  
}

int main(int argc, gchar *argv[]) {
  setlocale(LC_ALL, ""); //it sets to default system locale, e.g. en_US.UTF-8
  const char *homed = g_get_home_dir ();
  char lpth[strlen (homed) + 15];
  strcpy (lpth, homed);
  strcat (lpth, "/.bsdict.log");
  BS_DO_CEE_GOTO (BsLogFiles *bslf = bslogfiles_new (1), outlog)
  bslog_files_set_path (bslf, 0, lpth);
  if ( errno != 0 ) {
    bslf = bslogfiles_free (bslf);
    goto outlog;
  }
  bslog_init (bslf);
outlog:
  bsfatallog_init_fatal_signals ();
  errno = 0;

  sLastCstr[0] = 0;
  sLstClkCstr[0] = 0;
  sDicsWrds = bsdifdwds_new (BS_IDX_100);
  sAuDtSet = bsdidtt2s_new (BS_IDX_100);
  bool isDbg = bslog_is_debug (BS_DEBUGL_DICT);
#ifdef BS_DEBUG_BASIC_ENABLED
  if ( !isDbg )
  {
    if (bslog_get_debug_ceiling () == 0 )
    {
      bslog_set_debug_floor (BS_DEBUGL_DICT);
      bslog_set_debug_ceiling (BS_DEBUGL_DICT);
    } else {
      bslog_add_dbgrange (BS_DEBUGL_DICT, BS_DEBUGL_DICT);
    }
    isDbg = bslog_is_debug (BS_DEBUGL_DICT);
  }
#endif

  //init, main sMainWin,  widgets:
  gtk_init (&argc, &argv);
  sMainWin = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_widget_set_size_request (GTK_WIDGET(sMainWin), 400, 300);

  gtk_window_set_title (GTK_WINDOW (sMainWin), bsi18n_msg ("Beigesoft™ Dictionary"));

  gtk_window_set_icon_name (GTK_WINDOW (sMainWin), "bssoft");
  
  GtkWidget *vbox = gtk_vbox_new(FALSE, 5);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);
  gtk_container_add (GTK_CONTAINER (sMainWin), vbox);
  
  GtkWidget *hbox = gtk_hbox_new(FALSE, 10);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

  sEntry = gtk_entry_new ();
  gtk_box_pack_start (GTK_BOX (hbox), sEntry, TRUE, TRUE, 0);

  GtkWidget *btMnu = gtk_button_new_with_label ("...");
  gtk_box_pack_start (GTK_BOX (hbox), btMnu, FALSE, FALSE, 0);

  GtkWidget *scrlWin = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrlWin),
                        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_box_pack_start (GTK_BOX (vbox), scrlWin, TRUE, TRUE, 0);

  sView = gtk_text_view_new ();
  
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (sView), GTK_WRAP_WORD);

  gtk_text_view_set_editable (GTK_TEXT_VIEW (sView), FALSE);

  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (sView), 5);

  gtk_container_add (GTK_CONTAINER (scrlWin), sView);
  
  s_add_all_tags ();

  //menu:
  GtkWidget *menu = gtk_menu_new ();
  GtkWidget *menuItm = gtk_menu_item_new_with_label ("Settings");
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuItm);
  g_signal_connect_swapped (menuItm, "activate",
          G_CALLBACK (s_on_menu_item_click), sMenuVals + EBSDM_SETTINGS);
  gtk_widget_show (menuItm);

  menuItm = gtk_menu_item_new_with_label ("History");
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuItm);
  g_signal_connect_swapped (menuItm, "activate",
          G_CALLBACK (s_on_menu_item_click), sMenuVals + EBSDM_HISTORY);
  gtk_widget_show (menuItm);

  //completion:
  GtkEntryCompletion *cmpl = gtk_entry_completion_new ();

  gtk_entry_set_completion (GTK_ENTRY (sEntry), cmpl);
  g_object_unref (cmpl);
  
  sComplLst = gtk_list_store_new (1, G_TYPE_STRING);

  GtkTreeModel *cmplMod = GTK_TREE_MODEL (sComplLst);
  gtk_entry_completion_set_model (cmpl, cmplMod);
  g_object_unref (cmplMod);
  gtk_entry_completion_set_text_column (cmpl, 0);

  //signals:
  /*g_signal_connect (sView, "key-press-event", 
                    G_CALLBACK (key_press_event), NULL);
  g_signal_connect (sView, "motion-notify-event", 
                    G_CALLBACK (motion_notify_event), NULL);
  g_signal_connect (sView, "visibility-notify-event", 
                    G_CALLBACK (visibility_notify_event), NULL);*/
  g_signal_connect (sView, "event-after", 
                    G_CALLBACK (s_event_after), NULL);

  g_signal_connect (cmpl, "match-selected",
                    G_CALLBACK (s_word_selected), NULL);
  g_signal_connect_swapped (btMnu, "clicked",
                    G_CALLBACK (s_menu_open), menu);
  g_signal_connect (sEntry, "key-release-event",
                    G_CALLBACK (s_on_keydown), NULL);
  g_signal_connect (sMainWin, "destroy",
                    G_CALLBACK (s_on_exit), NULL);
  g_signal_connect (sMainWin, "delete-event",
                    G_CALLBACK (s_on_close), NULL);
  //launch all:
  gtk_widget_show_all (sMainWin);

  bsdicsettings_lget_dics ();

  gtk_main();
  
  return 0;
}
