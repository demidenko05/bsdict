/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Tester#1 of BsDiIxTxRm.c.</p>
 * @author Yury Demidenko
 **/

#include "stdio.h"
#include "stdlib.h"
#include "locale.h"

#include "BsFatalLog.h"
#include "BsError.h"
#include "BsDicLsa.h"
#include "BsFioWrap.h"

static BsDiIxT2Rm *sDiIxRm = NULL;

static void
  s_test1 (int argc, char *argv[])
{
  BS_WCHAR_T wstr[100];
  BS_DO_E_RET (BsDiIxOst *opSt = bsdiixost_new ())
  BS_DO_E_OUT (sDiIxRm = bsdiixlsarm_create (argv[1], opSt))
  //for ( int i = 0; i < 3 && i < sDiIxRm->irt->size; i++ )
  for ( int i = 0; i < sDiIxRm->irt->size; i++ )
  {
    bsdicidxab_istr_to_wstr (sDiIxRm->irt->vals[i]->iwrd, wstr, sDiIxRm->head->ab);
    BSLOG_LOG (BSLTEST, "word#%d=%ls, ofst=%u, len=%u\n",i, wstr, sDiIxRm->irt->vals[i]->ofst, sDiIxRm->irt->vals[i]->len) 
  }
  
  BS_DO_E_OUT (bsdiixt2rm_save (sDiIxRm, argv[1]))
  FILE *wavf = NULL, *oggf = NULL;
  BsStrBuf *wavRam = NULL, *oggRam = NULL;
  if ( argc >= 4 )
  {
    unsigned int recOfst;
    unsigned int recLen;
    sscanf(argv[2], "%u", &recOfst);
    sscanf(argv[3], "%u", &recLen);
    BS_DO_E_OUT (wavRam = bsdiclsa_readau ((BsDiIxT2Bs*) sDiIxRm, recOfst, recLen, true))
    wavf = fopen ("exmp.wav", "wb");
    BS_IF_EN_OUT (wavf == NULL, BSE_OPEN_FILE)
    fwrite (wavRam->vals, 1, wavRam->size, wavf);

    //mapping GdWavHeader:
    GdWavHeader *wh = (GdWavHeader*) wavRam->vals;
    BSLOG_LOG (BSLTEST, "mapped chan=%d, rate=%u\n", wh->channels, wh->samplesPerSec);
    BS_DO_E_OUT (oggRam = bsdiclsa_encvorbis (wavRam, wh->channels, wh->samplesPerSec))
    oggf = fopen ("exmp.ogg", "wb");
    BS_IF_EN_OUT (oggf == NULL, BSE_OPEN_FILE)
    fwrite (oggRam->vals, 1, oggRam->size, oggf);
  }
out:
  bsdiixost_free (opSt);
  if ( wavf != NULL )
            { fclose (wavf); }
  if ( oggf != NULL )
            { fclose (oggf); }
  bsstrbuf_free (wavRam);
  bsstrbuf_free (oggRam);
  if ( sDiIxRm->dicFl != NULL )
  { 
    fclose (sDiIxRm->dicFl);
    sDiIxRm->dicFl = NULL;
  }
}

static void
  s_test2 (int argc, char *argv[])
{

  //vars:
  BS_CHAR_T iwrd[30];
  unsigned int ofstRc, lenRc;
  BS_CHAR_T irtstr[30];
  BS_WCHAR_T wstr[30];
  BS_FOFST_T ofst;
  BsDiIxT2IdxIrd *rcd;
  BsString *str;
  BsDSoundRd *dsrcd;
  //code:
  rcd = NULL;
  str = NULL;
  dsrcd = NULL;
  BS_DO_E_RET (BsDiIxOst *opSt = bsdiixost_new ())
  BS_DO_E_OUT (BsDiIxT2 *diIx = bsdiixlsa_open (argv[1], opSt))
  ofst = 0L * ( BSDIIXT2IRTRD_FIX_SZ (diIx->head->mxIrWdSz) ) + diIx->irtOfst;
  BS_DO_E_OUT (bsfseek_goto (diIx->idxFl, ofst))
  BS_DO_E_OUT (bsfread_bschars (irtstr, diIx->head->mxIrWdSz, diIx->idxFl))
  BS_DO_E_OUT (bsfread_uint (&ofstRc, diIx->idxFl))
  BS_DO_E_OUT (bsfread_uint (&lenRc, diIx->idxFl))
  bsdicidxab_istr_to_wstr (irtstr, wstr, diIx->head->ab);
  BSLOG_LOG (BSLTEST, "First word=%ls, ofst=%u, len=%u, IRT offset=%ld, mxIrWdSz=%d\n", wstr, ofstRc, lenRc, diIx->irtOfst, diIx->head->mxIrWdSz) 
  ofst = 1L * ( BSDIIXT2IRTRD_FIX_SZ (diIx->head->mxIrWdSz) ) + diIx->irtOfst;
  BS_DO_E_OUT (bsfseek_goto (diIx->idxFl, ofst))
  BS_DO_E_OUT (bsfread_bschars (irtstr, diIx->head->mxIrWdSz, diIx->idxFl))
  BS_DO_E_OUT (bsfread_uint (&ofstRc, diIx->idxFl))
  BS_DO_E_OUT (bsfread_uint (&lenRc, diIx->idxFl))
  bsdicidxab_istr_to_wstr (irtstr, wstr, diIx->head->ab);
  BSLOG_LOG (BSLTEST, "Second word=%ls, ofst=%u, len=%u, IRT offset=%ld, mxIrWdSz=%d\n", wstr, ofstRc, lenRc, diIx->irtOfst, diIx->head->mxIrWdSz) 
  BS_IF_ENM_OUT (diIx == NULL, BSE_TEST_ERR, "Can't load just saved LSA IDX!\n")
  BS_IF_ENM_OUT (bsstring_compare (diIx->head->nme, sDiIxRm->head->nme) != 0, BSE_TEST_ERR, "diIx->head->nme != sDiIxRm->head->nme!\n")
  BS_IF_ENM_OUT (diIx->head->irtSz != sDiIxRm->head->irtSz, BSE_TEST_ERR, "diIx->head->irtSz != sDiIxRm->head->irtSz!\n")
  BS_IF_ENM_OUT (diIx->head->irtSz == 0, BSE_TEST_ERR, "diIx->head->irtSz == 0!\n")
  BS_IF_ENM_OUT (diIx->head->hirtSz != sDiIxRm->head->hirtSz, BSE_TEST_ERR, "diIx->head->hirtSz != sDiIxRm->head->hirtSz!\n")
  BS_IF_ENM_OUT (diIx->head->mxIrWdSz != sDiIxRm->head->mxIrWdSz, BSE_TEST_ERR, "diIx->head->mxIrWdSz != sDiIxRm->head->mxIrWdSz!\n")
  BS_IF_ENM_OUT (diIx->head->hirt == NULL, BSE_TEST_ERR, "diIx->head->hirt == NULL!\n")
  BS_IF_ENM_OUT (diIx->head->hirt[0]->fchar != sDiIxRm->head->hirt[0]->fchar, BSE_TEST_ERR, "diIx->head->hirt[0]->fchar != sDiIxRm->head->hirt[0]->fchar!\n")
  BS_IF_ENM_OUT (diIx->head->hirt[diIx->head->hirtSz - 1]->fchar != sDiIxRm->head->hirt[diIx->head->hirtSz - 1]->fchar, BSE_TEST_ERR, "diIx->head->hirt[end]->fchar != sDiIxRm->head->hirt[end]->fchar!\n")
  {
    char *cstr = argv[4];
    if ( cstr != NULL && strlen (cstr) >  0)
    {
      bsdicidxab_str_to_istr(cstr, iwrd, diIx->head->ab);
      BS_IDX_T irtStart = BS_IDX_0;
      BS_IDX_T irtEnd = diIx->head->irtSz - BS_IDX_1;
      if ( diIx->head->hirt != NULL )
      {
        BS_DO_E_OUT (bsdicidx_find_irtrange (diIx->head->hirt, diIx->head->hirtSz, iwrd, &irtStart, &irtEnd))
      }
      BS_DO_E_OUT (rcd = bsdiixt2tst_find_irtrd(diIx, iwrd, irtStart, irtEnd))
      if ( rcd != NULL )
      {
        BSLOG_LOG (BSLTEST, "For word %s sound offset=%u, len=%u\n", cstr, rcd->ofst, rcd->len)
      } else {
        BSLOG_LOG (BSLTEST, "For word %s sound data not found\n", cstr)
      }
      BS_DO_E_OUT (str = bsstring_new (cstr))
      BS_DO_E_OUT (dsrcd = bsdiclsatst_find (diIx, str))
      if ( dsrcd != NULL )
      {
        BSLOG_LOG (BSLTEST, "bsdiclsa_find - For word %s sound offset=%u, len=%u\n", dsrcd->wrd->val, dsrcd->ofst, dsrcd->len)
      } else {
        BSLOG_LOG (BSLTEST, "bsdiclsa_find - For word %s sound data not found\n", str->val)
      }
    }
  }
out:
  bsdiixost_free (opSt);
  if ( dsrcd != NULL )
  {
    bsdsoundrd_free (dsrcd);
  } else {
    bsstring_free (str);
  }
  bsdiixt2idxird_free (rcd);
  bsdiixt2_destroy (diIx);
}

static void
  s_test3 (int argc, char *argv[])
{
  //vars:
  BsDiIxOst *opSt;
  BsDiFdWds *dicWrds;
  BsDiIxT2 *diIx;
  char *cstr;
  //init0:
  opSt = NULL; dicWrds = NULL; diIx = NULL;
  //code:
  if ( argc < 5 )
          { return; }
  BS_DO_E_OUT (dicWrds = bsdifdwds_new (BS_IDX_100))
  BS_DO_E_OUT (opSt = bsdiixost_new ())
  BS_DO_E_OUT (diIx = bsdiixlsa_open (argv[1], opSt))
  cstr = argv[4];
  if ( cstr != NULL && strlen (cstr) >  0)
  {
    BS_DO_E_OUT (bsdiclsafind_mtch (diIx, dicWrds, cstr))
    for ( int i = 0; i < dicWrds->size; i++ )
            { bslog_log(BSLONLYMSG, "dicWrds->vals[%d]->wrd->val = %s\n", i, dicWrds->vals[i]->wrd->val); }
  }
out:
  bsdiixost_free (opSt);
  bsdifdwds_free (dicWrds);
  bsdiixt2_destroy (diIx);
}

int
  main (int argc, char *argv[])
{
  if ( argc < 2 )
  {
    perror ("WRONG params");
    return BSE_WRONG_PARAMS;
  }
  setlocale (LC_ALL, ""); //it set to default system locale, e.g. en_US.UTF-8
  BS_DO_E_GOTO (BsLogFiles *bslf=bslogfiles_new (1), outlog)
  bslog_files_set_path (bslf, 0, "tst_BsDicLsa.log");
  if ( errno != 0 )
  {
    bslf = bslogfiles_free (bslf);
    goto outlog;
  }
  bslog_set_maximum_size (6000000L);
  bslog_init (bslf);
outlog:
  bsfatallog_init_fatal_signals ();
  errno = 0;
  bslog_set_debug_floor (BS_DEBUGL_DICLSA + 90);
  bslog_set_debug_ceiling (BS_DEBUGL_DICLSA + 99);
  BS_DO_E_OUT (s_test1 (argc, argv))
  BS_DO_E_OUT (s_test2 (argc, argv))
  bslog_set_debug_floor (BS_DEBUGL_DIIXT2 + 80);
  bslog_set_debug_ceiling (BS_DEBUGL_DIIXT2 + 80);
  s_test3 (argc, argv);

out:
  if ( errno != 0 )
  {
    BSLOG_ERR
  }
  bsdiixt2rm_destroy (sDiIxRm);
  bslog_destroy ();
  return errno;
}
