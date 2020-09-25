/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2007             *
 * by the Xiph.Org Foundation https://xiph.org/                     *
 *                                                                  *
 ********************************************************************/

/* This file is (c) 2008-2012 Konstantin Isakov <ikm@goldendict.org>
 * Part of GoldenDict. Licensed under GPLv3 or later, see the LICENSE.goldendict file */

/**
 * <p>Beigesoft™ dictionary LSA library.</p>
 * @author Yury Demidenko, it's also based on goldendict and vorbis sources
 **/

#include "wchar.h"
#include "wctype.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

#include <vorbis/vorbisfile.h>

#include "BsError.h"
#include "BsDicLsa.h"
#include "BsFioWrap.h"

/**
 * <p>Vorbis file read delegator.</p>
 * @param pDtRt - data pointer to return
 * @param pSz - block size
 * @param pCnt - blocks count
 * @param pDs - dicFl-idx bundle
 * return file read result
 **/
static size_t
  s_bs_vorbis_read (void *pDtRt, size_t pSz, size_t pCnt, void *pDs)
{
  BsDiIxT2Bs *diIx = (BsDiIxT2Bs*) pDs;

  return fread (pDtRt, pSz, pCnt, diIx->dicFl);
}

/**
 * <p>Vorbis file tell delegator.</p>
 * @param pDs - dicFl-idx bundle
 * return file tell result
 **/
static long
  s_bs_vorbis_tell (void* pDs)
{
  BsDiIxT2Bs *diIx = (BsDiIxT2Bs*) pDs;
  long ret = ftell (diIx->dicFl);

  if ( ret > -1L )
                  { ret -= diIx->head->cntOfst; }
  return ret;
}

/**
 * <p>Vorbis file seek delegator.</p>
 * @param pDs - dicFl-idx bundle
 * @param pOfst - offset
 * @param pWhe - whence
 * return file seek result
 **/
static int
  s_bs_vorbis_seek (void *pDs, ogg_int64_t pOfst, int pWhe)
{
  BsDiIxT2Bs *diIx = (BsDiIxT2Bs*) pDs;

  if ( pWhe == SEEK_SET )
              { pOfst += diIx->head->cntOfst; }

  return fseek (diIx->dicFl, pOfst, pWhe);
}

/**
 * <p>Encode PCM data with vorbis. In case of playing "file in memory", this method
 * is useless (non-optimal) cause PCM with WAV header is already do.
 * Also, depending of decoding implementation, for non-HDD file storage making zipped
 * OGG files may be non-optimal.
 * Here is used playing a file instead of "file in memory" because of sound files
 * are placed into TMP, and they are "already cached".
 * Comments are original , i.e. vorbis's authors.</p>
 * @param pAuDt - PCM data array with or without header (with header is WAV)
 * @param pChan - channels, usually 1
 * @param pRate - bitrate, usually 11025
 * @return encoded OGG in memory, it's usually 3 times less than PCM, e.g. 22600PCM vs 5973OGG
 * @set errno if error
 **/
BsStrBuf*
  bsdiclsa_encvorbis (BsStrBuf *pAuDt, int pChan, long pRate)
{
  #define READ 1024
  signed char readbuffer[READ + 44]; /* out of the data segment, not the stack */
  ogg_stream_state os; /* take physical pages, weld into a logical
                          stream of packets */
  ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
  ogg_packet       op; /* one raw packet of data for decode */

  vorbis_info      vi; /* struct that stores all the static vorbis bitstream
                          settings */
  vorbis_comment   vc; /* struct that stores all the user comments */

  vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
  vorbis_block     vb; /* local working space for packet->PCM decode */

  BS_IDX_T pcmCuIdx = BS_IDX_0;

  /* we cheat on the WAV header; we just bypass 44 bytes (simplest WAV
     header is 44 bytes) and assume that the data is 44.1khz, stereo, 16 bit
     little endian pcm samples. This is just an example, after all. */

  /* we cheat on the WAV header; we just bypass the header and never
     verify that it matches 16bit/stereo/44.1kHz.  This is just an
     example, after all. */

  for ( int i = 0 ; i < 30 && pcmCuIdx < pAuDt->size; i++ )
  {
    memcpy(readbuffer, pAuDt->vals + pcmCuIdx, 2);
    pcmCuIdx += 2;

    if ( !strncmp ((char*) readbuffer, "da", 2) )
    {
      pcmCuIdx += 6;
      break;
    }
  }
  bool isDbg = bslog_is_debug (BS_DEBUGL_DICLSA + 85);
  if ( isDbg )
            { BSLOG_LOG (BSLDEBUG,"PCM size=%ld, offset after header %ld\n", pAuDt->size, pcmCuIdx) }

  /********** Encode setup ************/

  vorbis_info_init(&vi);

  /* do not continue if setup failed; this can happen if we ask for a
     mode that libVorbis does not support (eg, too low a bitrate, etc,
     will return 'OV_EIMPL') */
  BS_IF_ENM_RETN ( vorbis_encode_init_vbr (&vi, pChan, pRate, 0.1) != 0, BSE_ERR, "Can't init encoder!\n")

  BS_DO_E_RETN (BsStrBuf *obj = bsstrbuf_new (pAuDt->size / 3))

  /* add a comment */
  vorbis_comment_init (&vc);
  vorbis_comment_add_tag (&vc, "ENCODER","encoder_example.c");

  /* set up the analysis state and auxiliary encoding storage */
  vorbis_analysis_init (&vd, &vi);
  vorbis_block_init (&vd, &vb);

  /* set up our packet->stream encoder */
  /* pick a random serial number; that way we can more likely build
     chained streams just by concatenation */
  srand (time (NULL));
  ogg_stream_init (&os, rand ());

  /* Vorbis streams begin with three headers; the initial header (with
     most of the codec setup parameters) which is mandated by the Ogg
     bitstream spec.  The second header holds any comment fields.  The
     third header holds the bitstream codebook.  We merely need to
     make the headers, then pass them to libvorbis one at a time;
     libvorbis handles the additional Ogg bitstream constraints */

  ogg_packet header;
  ogg_packet header_comm;
  ogg_packet header_code;

  vorbis_analysis_headerout (&vd, &vc, &header, &header_comm, &header_code);
  ogg_stream_packetin (&os, &header); /* automatically placed in its own
                                       page */
  ogg_stream_packetin (&os, &header_comm);
  ogg_stream_packetin (&os, &header_code);

  /* This ensures the actual
   * audio data will start on a new page, as per spec
   */
  while ( true )
  {
    int result = ogg_stream_flush (&os, &og);
    if ( result == 0 )
                  { break; }
    BS_DO_E_OUTE (bsstrbuf_add_uchars_inc (obj, og.header, og.header_len, BS_IDX_1000))
    BS_DO_E_OUTE (bsstrbuf_add_uchars_inc (obj, og.body, og.body_len, BS_IDX_1000))
  }

#define BS_AUDIO_STEREO 4
#define BS_AUDIO_MONO 2

  int steMono;
  if ( pChan == 1 )
  {
    steMono = BS_AUDIO_MONO;
  } else {
    steMono = BS_AUDIO_STEREO;
  }

  long left = pAuDt->size - pcmCuIdx + 1L;
  long bufMaxSz = READ;
  while ( true )
  {
    long cnt, i;

    if ( left == 0L )
    {
      /* end of file.  this can be done implicitly in the mainline,
         but it's easier to see here in non-clever fashion.
         Tell the library we're at end of stream so that it can handle
         the last frame and mark end of stream in the output properly */
      vorbis_analysis_wrote (&vd, 0);

    } else {
      if ( left < bufMaxSz )
      {
        cnt = left;
      } else {
        cnt = bufMaxSz;
      }
      if ( isDbg )
                { BSLOG_LOG (BSLDEBUG,"Read ofst=%ld cnt=%ld left=%ld\n", pcmCuIdx, cnt, left) }
      memcpy (readbuffer, pAuDt->vals + pcmCuIdx, cnt);
      /* data to encode */

      /* expose the buffer to submit data */
      float **buffer = vorbis_analysis_buffer (&vd, READ);

      /* uninterleave samples */
      for ( i = 0L; i < cnt / steMono; i++ ) {
        buffer[0][i] = ( ( readbuffer[i * steMono + 1 ] << 8 ) |
                         ( 0x00ff & (int) readbuffer[i * steMono] ) ) / 32768.f;
        if ( pChan > 1 )
        {
          buffer[1][i] = ( ( readbuffer[i * steMono + 3] << 8) |
                           ( 0x00ff & (int) readbuffer[i * steMono + 2] ) ) / 32768.f;
          if ( isDbg )
                    { BSLOG_LOG (BSLDEBUG,"Stereo i=%ld\n", i) }
        }
      }

      /* tell the library how much we actually submitted */
      vorbis_analysis_wrote (&vd, i);
    }
    /* vorbis does some data preanalysis, then divvies up blocks for
       more involved (potentially parallel) processing.  Get a single
       block for encoding now */
    while ( vorbis_analysis_blockout (&vd, &vb) == 1 )
    {
      /* analysis, assume we want to use bitrate management */
      vorbis_analysis (&vb, NULL);
      vorbis_bitrate_addblock (&vb);

      while ( vorbis_bitrate_flushpacket (&vd, &op) )
      {
        /* weld the packet into the bitstream */
        ogg_stream_packetin (&os, &op);
        /* write out pages (if any) */
        while ( true )
        {
          int result = ogg_stream_pageout (&os, &og);
          if ( result == 0 )
                          { break; }
          BS_DO_E_OUTE (bsstrbuf_add_uchars_inc (obj, og.header, og.header_len, BS_IDX_1000))
          BS_DO_E_OUTE (bsstrbuf_add_uchars_inc (obj, og.body, og.body_len, BS_IDX_1000))
          if ( isDbg )
                    { BSLOG_LOG (BSLDEBUG,"head len=%ld, body len=%ld\n", og.header_len, og.body_len) }

          /* this could be set above, but for illustrative purposes, I do
             it here (to show that vorbis does know where the stream ends) */

          if ( ogg_page_eos (&og) )
                          { break; }
        }
      }
    }
    if ( left == 0L )
                    { break; }
    pcmCuIdx += cnt;
    left -= cnt;
  }
  
  goto out;

oute:
  obj = bsstrbuf_free (obj);

out:
  /* clean up and exit.  vorbis_info_clear() must be called last */
  ogg_stream_clear (&os);
  vorbis_block_clear(&vb);
  vorbis_dsp_clear (&vd);
  vorbis_comment_clear (&vc);
  vorbis_info_clear (&vi);
  /* ogg_page and ogg_packet structs always point to storage in
     libvorbis.  They're never freed or manipulated directly */

  return obj;
}

/**
 * <p>Read audio data from LSA file.</p>
 * @param pDiIx - dictionary with index.
 * @param pRecOfst - audio record offset from vorbis start
 * @param pRecLen - audio record len
 * @param pMkHd - if make WAV header
 * @return audio data WAV file in memory (PCM data with optional RIFF header according GoldenDict source)
 * @set errno if error.
 **/
BsStrBuf*
  bsdiclsa_readau (BsDiIxT2Bs *pDiIx, unsigned int pRecOfst, unsigned int pRecLen, bool pMkHd)
{
#define BSDICLSA_BUF_SZ 4096
  char pcmout[BSDICLSA_BUF_SZ];
  int curSec;
  OggVorbis_File vf;
  BsStrBuf *obj = NULL;
  ov_callbacks ovDelg = { s_bs_vorbis_read, s_bs_vorbis_seek, NULL, s_bs_vorbis_tell };
  BS_DO_E_RETN (bsfseek_goto (pDiIx->dicFl, pDiIx->head->cntOfst))
  BS_IF_ENM_RETN (ov_open_callbacks (pDiIx, &vf, NULL, 0, ovDelg) < 0, BSE_ERR, "Can't open vorbis file\n")

  vorbis_info *vi = ov_info (&vf, -1);
  BS_IF_ENM_OUTE (vi == NULL, BSE_ERR, "Can't get vorbis info\n")
  bool isDbg = bslog_is_debug (BS_DEBUGL_DICLSA + 85);
  if ( isDbg )
            { BSLOG_LOG (BSLDEBUG,"Bitstream is %d channel, %ldHz\n", vi->channels ,vi->rate) }

  BS_IF_ENM_OUTE (ov_pcm_seek(&vf, pRecOfst) != 0, BSE_ERR, "Can't seek vorbis record!\n")

  BS_IDX_T blen = pRecLen * 2;
  long left = pRecLen * 2; //TODO 2 for 1 channel?
  if ( pMkHd )
        {  blen += sizeof (GdWavHeader); }

  BS_DO_E_OUTE (obj = bsstrbuf_new (blen))

  if ( pMkHd )
  {
    BS_DO_E_OUTE (bsstrbuf_add_str_inc (obj, "RIFF", BS_IDX_100))
    uint32_t ui32 = blen - 8; //riffLength
    BS_DO_E_OUTE (bsstrbuf_add_inc_tus(obj, &ui32, 4, BS_IDX_100)) //TODO 2 typesafe wrappers
    BS_DO_E_OUTE (bsstrbuf_add_str_inc (obj, "WAVEfmt ", BS_IDX_100))
    ui32 = 16; //fmtLength
    BS_DO_E_OUTE (bsstrbuf_add_inc_tus(obj, &ui32, 4, BS_IDX_100))
    uint16_t ui16 = 1; //formatTag
    BS_DO_E_OUTE (bsstrbuf_add_inc_tus(obj, &ui16, 2, BS_IDX_100))
    ui16 = vi->channels;
    BS_DO_E_OUTE (bsstrbuf_add_inc_tus(obj, &ui16, 2, BS_IDX_100))
    ui32 = vi->rate; //samplesPerSec
    BS_DO_E_OUTE (bsstrbuf_add_inc_tus(obj, &ui32, 4, BS_IDX_100))
    ui32 = vi->channels * vi->rate * 2; //bytesPerSec
    BS_DO_E_OUTE (bsstrbuf_add_inc_tus(obj, &ui32, 4, BS_IDX_100))
    ui16 = vi->channels * 2; //blockAlign
    BS_DO_E_OUTE (bsstrbuf_add_inc_tus(obj, &ui16, 2, BS_IDX_100))
    ui16 = 16; //bitsPerSample
    BS_DO_E_OUTE (bsstrbuf_add_inc_tus(obj, &ui16, 2, BS_IDX_100))
    BS_DO_E_OUTE (bsstrbuf_add_str_inc (obj, "data", BS_IDX_100))
    ui32 = left; //dataLength
    BS_DO_E_OUTE (bsstrbuf_add_inc_tus(obj, &ui32, 4, BS_IDX_100))
  }

  long cnt;
  while ( true )
  {
    if ( left < BSDICLSA_BUF_SZ )
    {
      cnt = left;
    } else {
      cnt = BSDICLSA_BUF_SZ;
    }
    long ret = ov_read (&vf, pcmout, cnt, 0, 2, 1, &curSec);
    if ( ret == 0L )
    { 
      if ( isDbg )
                { BSLOG_LOG (BSLDEBUG,"End of bitstream left=%ldHz\n", left) }
      break;
    }
    if ( ret > cnt )
    {
      BSLOG_LOG(BSLERROR, "Wrong read bitstream ret=%ld\n", ret);
      goto oute;
    }
    if ( ret < 0 )
    {
      if ( ret == OV_EBADLINK )
      {
        BSLOG_LOG(BSLERROR, "Corrupt bitstream section! Exiting.\n");
        goto oute;
      }
      BSLOG_LOG(BSLERROR, "bitstream error #%d.\n", ret);
    } else {
      BS_DO_E_OUTE (bsstrbuf_add_chars_inc (obj, pcmout, ret, BS_IDX_100))
      left -= ret;
      if ( left <= 0L )
                  { break; }
    }
  }
  ov_clear (&vf);
  return obj;

oute:
  bsstrbuf_free (obj);
  ov_clear (&vf);
  return NULL;
}


/**
 * <p>Constructor.</p>
 * @param pWdOw word to own
 * @param pRcd record
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDSoundRd*
  bsdsoundrd_new (BsString *pWdOw, BsDiIxT2IdxIrd *pRcd)
{
  BsDSoundRd *obj = malloc (sizeof (BsDSoundRd));
  if ( obj != NULL )
  {
    obj->wrd = pWdOw;
    obj->ofst = pRcd->ofst;
    obj->len = pRcd->len;
  } else {
    if ( errno == 0 ) { errno = ENOMEM; }
    BSLOG_ERR
  }
  return obj;
}

/**
 * <p>Destructor.</p>
 * @param pRcd record
 * @return always NULL
 **/
BsDSoundRd*
  bsdsoundrd_free (BsDSoundRd *pRcd)
{
  if ( pRcd != NULL )
  {
    if ( pRcd->wrd != NULL )
    {
      bsstring_free (pRcd->wrd);
    }
    free (pRcd);
  }
  return NULL;
}

//Making DIC with IDX main lib:

/**
 * <p>Read all words from dictionary file.</p>
 * @param pDiIxRm opened rewind dictionary bundle
 * @param pIcDs - iconv descriptor
 * @param pDiWd_Csm consumer of just read d.word audio offset and length
 * @param pInstr consumer's additional instrument
 * @return BSR_OK_END or consumer's last return value, e.g. BSR_OK_ENOUGH or errno
 * @set errno if error.
 **/
int
  bsdiixlsa_iter (BsDiIxT2Rm *pDiIxRm, iconv_t pIcDs, BsDiAuWd_Csm *pDiWd_Csm, void *pInstr)
{
  unsigned int left, ofst = 0, len;
  int i, j;
  char buf[400]; BS_WCHAR_T wbuf[200], *ext;
  bool isDbg = ( pDiIxRm->head->cntOfst == BS_IDX_NULL
                  && bslog_is_debug (BS_DEBUGL_DICLSA + 85) );
  left = pDiIxRm->head->irtSz;
  while ( !feof (pDiIxRm->dicFl) && !ferror (pDiIxRm->dicFl) )
  { 
    i = -1;
    do {
      i++;
      BS_DO_E_OUTE (bsfread_char (buf + i, pDiIxRm->dicFl))
    } while ( i < 398 && buf[i] != 0xD );
    buf[i] = 0;
    char *inbuf = buf;
    char *outbuf = (char*) wbuf;
    size_t inbytes = i; //without 0xD or 0
    size_t outbytes = i * sizeof (wchar_t);
    size_t n = iconv (pIcDs, &inbuf, &inbytes, &outbuf, &outbytes);
    BS_IF_ENM_OUTE (n == (size_t) -1, BSE_ERR, "iconv: %m\n")
    ext = wcsstr (wbuf, L".wav");
    if ( ext != NULL )
                    { ext[0] = 0; }
    for ( j = 0; ; j++ )
    {
      if ( wbuf[j] == 0 )
                      { break; }
      if ( iswalpha (wbuf[j]) )
                      { wbuf[j] = towlower (wbuf[j]); }
    }
    BS_DO_E_OUTE (bsfread_chars (buf, 4, pDiIxRm->dicFl))
    if ( buf[0] != 0x0 || buf[1] != 0xA || buf[2] != 0x0 || buf[3] != (char) 0xFF )
    {
      BSLOG_LOG (BSLERROR, "Wrong end [%hhx, %hhx, %hhx, %hhx]\n", buf[0], buf[1], buf[2], buf[3])
      errno = BSE_WRONG_FDATA;
      return errno;
    }
    if ( left != pDiIxRm->head->irtSz )
    {
      BS_DO_E_OUTE (bsfread_uint (&ofst, pDiIxRm->dicFl))
      BS_DO_E_OUTE (bsfread_char (buf, pDiIxRm->dicFl))
      BS_IF_ENM_OUTE ( buf[0] != (char) 0xFF, BSE_WRONG_FDATA, "Wrong offset end 0xFF\n")
    }
    BS_DO_E_OUTE (bsfread_uint (&len, pDiIxRm->dicFl))
    if ( isDbg )
            { BSLOG_LOG (BSLDEBUG, "i=%d, word=%ls, ofst=%u, len=%u\n", i, wbuf, ofst, len) }
    BS_DO_E_OUTE (int csr = pDiWd_Csm (pDiIxRm, wbuf, ofst, len, pInstr))
    if ( csr != 0 )
                  { return csr; }
    left--;
    if ( left == 0 )
    {
      if ( pDiIxRm->head->cntOfst == BS_IDX_NULL)
      {
        pDiIxRm->head->cntOfst = ftell (pDiIxRm->dicFl);
        BS_DO_E_OUTE (bsfread_chars (buf, 4, pDiIxRm->dicFl))
        buf[4] = 0;
        BSLOG_LOG (BSLINFO, "sound offset=%ld, description=%s\n", pDiIxRm->head->cntOfst, buf)
      }
      break;
    }
  }
  return BSR_OK_END;

oute:
  BSLOG_LOG (BSLERROR, "Error on dicFl offset=%ld:\n", ftell (pDiIxRm->dicFl));
  return errno;
}

/**
 * <p>Create LSA with index bundle in memory from dictionary file.</p>
 * @param pPth - dictionary path.
 * @param pOpSt - opening state data shared with client
 * @return opened dictionary with index bundle in memory
 * @set errno if error.
 **/
BsDiIxT2Rm*
  bsdiixlsarm_create (char *pPth, BsDiIxOst* pOpSt)
{
  FILE *dicFl = fopen (pPth, "rb");
  BS_IF_EN_RETN (dicFl == NULL, BSE_OPEN_FILE)
  BS_WCHAR_T firstChars[BSDIAU_FIRSTCHARARRSZ];
  int i;
  for ( i = 0; i < BSDIAU_FIRSTCHARARRSZ; i++ )
                          { firstChars[i] = 0; }    
  BsDiIxHeadT2 *head = NULL;
  BsDiIxT2Rm *obj = NULL;
  unsigned int tot;
  iconv_t cd;
  cd = iconv_open ("WCHAR_T", "UTF-16");
  BS_IF_ENM_OUTE (cd == (iconv_t) -1, BSE_ERR, "Can't make iconv UTF-16->WCHAR_T\n")
  BS_DO_E_OUTE (bsfseek_goto (dicFl, 9L))
  BS_DO_E_OUTE (bsfread_uint (&tot, dicFl))
  BSLOG_LOG (BSLINFO, "File=%s, total words=%u\n", pPth, tot);
  BS_DO_E_OUTE (head = bsdiixheadt2_new_tf (tot))
  char *nm = strrchr (pPth, '/');
  if ( nm != NULL ) {
    nm = nm + 1;
  } else {
    nm = pPth;
  }
  BS_DO_E_OUTE (head->nme = bsstring_new (nm))
  BS_DO_E_OUTE (obj = bsdiixt2rm_new (head, dicFl))
  //1. make AB, first chars count:
  BS_DO_E_OUTE (bsdiixlsa_iter (obj, cd, (BsDiAuWd_Csm*) bsdiixt2_csm_ab, (void*) firstChars))
  for ( i = 0; i < BSDIAU_FIRSTCHARARRSZ; i++ )
  { 
    if ( firstChars[i] == 0 )
                      { break; }
  }    
  obj->head->hirtSz = i;
  BSLOG_LOG (BSLINFO, "Created name=%s; AB chars total=%d, ispace=%d; mxIrWdSz=%d; HIRT size=%d; wchars:\n", obj->head->nme->val, obj->head->ab->chrsTot, obj->head->ab->ispace, obj->head->mxIrWdSz, obj->head->hirtSz);
  for ( i = 0; i < obj->head->ab->chrsTot; i++ )
      { bslog_log (BSLONLYMSG, " [%d / %d]", i + 1, obj->head->ab->wchars[i]); }
  bslog_log (BSLONLYMSG, "\n");

  //2. make IRT in memory:
  BS_DO_E_OUTE (bsfseek_goto (dicFl, 13L))
  BS_DO_E_OUTE (obj->irt = bsdiixt2irt_new (tot))
  BS_DO_E_OUTE (bsdiixlsa_iter (obj, cd, (BsDiAuWd_Csm*) bsdiixt2_csm_irt, (void*) obj->irt))
  BSLOG_LOG (BSLINFO, "Created LSA IRT bufSz=%ld size=%ld:\n", obj->irt->bsize, obj->irt->size)

  //3. make HIRT:
  if ( obj->head->hirtSz == BSDIAU_FIRSTCHARARRSZ )
  {
    //NO HIRT:
    obj->head->hirtSz = 0;
    BSLOG_LOG (BSLINFO, "No HIRT cause first chars count > %d\n", BSDIAU_FIRSTCHARARRSZ)
  } else if ( obj->head->hirtSz < 1 ) {
    errno = BSE_ALG_ERR;
    BSLOG_LOG (BSLERROR, "Wrong HIRT first chars count = %d\n", obj->head->hirtSz)
    goto oute;
  } else {
    BSDIIXHIRTRDMK_CREATE (hirtrdmk, obj->head->ab->chrsTot)
    for ( BS_IDX_T l = BS_IDX_0; l < obj->irt->size; l++ )
    {
      if ( hirtrdmk.fchar != obj->irt->vals[l]->iwrd[0] )
      {
        if ( hirtrdmk.fchar != 0 )
        { //next first char
          BSDIIXHIRTRD_NEW_FILL_E_OUTE (hirtRd, &hirtrdmk)
          BS_DO_E_OUTE (bsdiixheadt2_add_hirtrd (obj->head, hirtRd))
        }
        //new set of words with new first char
        bsdiixhirtrdmk_init (&hirtrdmk);
        hirtrdmk.fchar = obj->irt->vals[l]->iwrd[0];
        hirtrdmk.irtIdx = l;
      }
      if ( obj->irt->vals[l]->iwrd[1] != 0 )
      {
        BS_DO_E_OUTE (bsdiixhirtrdmk_make (&hirtrdmk, obj->irt->vals[l]->iwrd[1], l))
      }
    }
    //last accumulated HIRT data:
    BSDIIXHIRTRD_NEW_FILL_E_OUTE (hirtRd, &hirtrdmk)
    BS_DO_E_OUTE (bsdiixheadt2_add_hirtrd (obj->head, hirtRd))
    BS_DO_E_OUTE (bsdicidxhirt_validate (obj->head->hirt, obj->head->hirtSz, obj->head->ab))
    bool isDbg = bslog_is_debug (BS_DEBUGL_DICLSA);
    if ( isDbg )
    {
      BSLOG_LOG (BSLDEBUG, "Created HIRT size=%d:\n", obj->head->hirtSz)
      for ( int i = 0; i < obj->head->hirtSz; i++ )
      {
        bslog_log (BSLONLYMSG, "    #%d fchar=%hu irtIdx="BS_IDX_FMT" sch_sz=%d\n", i, obj->head->hirt[i]->fchar, obj->head->hirt[i]->irtIdx, obj->head->hirt[i]->schrsSz);
        if ( obj->head->hirt[i]->schrsSz > 0 )
        {
          for ( int j = 0; j < obj->head->hirt[i]->schrsSz; j++ )
            { bslog_log (BSLONLYMSG, "      schar=%hu irtIdx="BS_IDX_FMT"\n", obj->head->hirt[i]->schars[j], obj->head->hirt[i]->irtIdxs[j]); }
        }
      }
      bslog_log(BSLONLYMSG, "\n");
    } else {
      BSLOG_LOG (BSLINFO, "Created HIRT size=%d\n", obj->head->hirtSz)
    }
  }
  goto out;
  
oute:
  bslog_log (BSLONLYMSG, "First chars:\n");
  for ( i = 0; i < BSDIAU_FIRSTCHARARRSZ; i++ )
  { 
    if ( firstChars[i] != 0 )
      { bslog_log (BSLONLYMSG, " #%d %lc", i, firstChars[i]); }
  }
  bslog_log (BSLONLYMSG, "\n");

  if ( obj != NULL )
  {
    if ( obj->irt != NULL )
        { BSLOG_LOG (BSLERROR, "Error on IRT size=%ld:\n", obj->irt->size) }
    obj = bsdiixt2rm_destroy (obj);
  } else {
    bsdiixheadt2_free (head);
    fclose (dicFl);
  }

out:
  if ( cd != (iconv_t) -1 )
              { iconv_close (cd); }
  return obj;
}

/**
 * <p>Open DIC IDX, i.e. load or create then load.</p>
 * @param pPth - dictionary path.
 * @param pOpSt - opening state data shared with client
 * @return object or NULL if error
 * @set errno if error.
 **/
BsDiIxT2*
  bsdiixlsa_open (char *pPth, BsDiIxOst* pOpSt)
{
  BsDiIxT2 *obj;
  BsDiIxT2Rm *diIxRm;
  diIxRm = NULL;
  BS_DO_E_OUTE (obj = bsdiixt2_load (pPth))
  if ( obj != NULL )
  {
    pOpSt->prgr = 100;
    pOpSt->stt = EBSDS_OPENED;
    return obj;
  }
  pOpSt->stt = EBSDS_INDEXING;

  BS_DO_E_OUTE (diIxRm = bsdiixlsarm_create (pPth, pOpSt))

  BS_DO_E_OUTE (bsdiixt2rm_save (diIxRm, pPth))

  diIxRm = bsdiixt2rm_destroy (diIxRm);

  BS_DO_E_OUTE (obj = bsdiixt2_load (pPth))
  
  if ( obj != NULL )
  {
    pOpSt->prgr = 100;
    pOpSt->stt = EBSDS_OPENED;
    return obj;
  }

oute:
  bsdiixt2rm_destroy (diIxRm);
  bsdiixt2_destroy (obj);
  pOpSt->stt = EBSDS_ERROR;
  return NULL;
}

/**
 * <p>Just make sound description.</p>
 * @param pDiIx - DIC with IDX
 * @param pFdWrd - found word with data to search content
 * @return full description as BsHypStrs
 * @set errno if error.
 **/
BsHypStrs*
  bsdiclsa_read (BsDiIxT2 *pDiIx, BsDiFdWd *pFdWrd)
{
  BsHypStrs *hstrs;
  BsHypTags *htags;
  int i;
  hstrs = NULL; htags = NULL;
  for ( i = 0; i < pFdWrd->dicOfLns->size; i++ )
  {
    if ( (BsDiIxT2*) pFdWrd->dicOfLns->vals[i]->diIx == pDiIx )
    {
      BS_DO_E_OUTE (hstrs = bshypstrs_new (BS_IDX_1))
      BS_DO_E_OUTE (htags = bshyptags_new (BS_IDX_1))
      BS_DO_E_OUTE (bshyptags_add (htags, EBSHT_AUDIO))
      BS_DO_E_OUTE (bshypstrs_add_inc (hstrs, bshypstr_new (
            pFdWrd->wrd->val, htags, pFdWrd->dicOfLns->vals[i]->ofst,
                                  pFdWrd->dicOfLns->vals[i]->len), BS_IDX_1))
      bshyptags_free (htags);
      return hstrs;
    }
  }
oute:
  bshyptags_free (htags);
  bshypstrs_free (hstrs);
  return NULL;
}

//Find lib:

/**
 * <p>Find all matched words in given dictionary and IDX.</p>
 * @param pDiIx - DIC with IDX
 * @param pFdWrds - collection to add found record
 * @param pSbwrd - sub-word to match
 * @set errno if error.
 **/
void
  bsdiclsafind_mtch (BsDiIxT2 *pDiIx, BsDiFdWds *pFdWrds, char *pSbwrd)
{
  int len = strlen (pSbwrd);
  if ( len < 1 )
        { return; }
  BS_CHAR_T iwrd[len + 1];
  BS_DO_E_RET (bsdicidxab_str_to_istr(pSbwrd, iwrd, pDiIx->head->ab))
  //1.find idxStartr of first up to two chars and idxEndr:
  BS_IDX_T irtStart = BS_IDX_0;
  BS_IDX_T irtEnd = pDiIx->head->irtSz - BS_IDX_1;
  if ( pDiIx->head->hirt != NULL )
  {
    BS_DO_E_RET (bsdicidx_find_irtrange (pDiIx->head->hirt, pDiIx->head->hirtSz, iwrd, &irtStart, &irtEnd))
  }
  BS_DO_E_RET (BsDiIxT2IdxIrd *warec = bsdiixt2tst_find_irtrd (pDiIx, iwrd, irtStart, irtEnd))
  if ( warec != NULL )
  {
    bsdiixt2tst_find_mtch (pDiIx, iwrd, warec, pFdWrds);
    bsdiixt2idxird_free (warec);
  }
}

/**
 * <p>Find exactly matched word with sound data in given dictionary and IDX.</p>
 * @param pDiIx IDX with head, opened DIC and IDX
 * @param pWrd - word to match
 * @return dic.sound-record or NULL if not found or error
 * @set errno if error.
 **/
BsDSoundRd*
  bsdiclsatst_find (BsDiIxT2 *pDiIx, BsString *pWrd)
{
  BS_CHAR_T iwrd[pWrd->len + 1];
  BS_DO_E_RETN (bsdicidxab_str_to_istr(pWrd->val, iwrd, pDiIx->head->ab))
  //1.find idxStartr of first up to two chars and idxEndr:
  BS_IDX_T irtStart = BS_IDX_0;
  BS_IDX_T irtEnd = pDiIx->head->irtSz - BS_IDX_1;
  if ( pDiIx->head->hirt != NULL )
  {
    BS_DO_E_RETN (bsdicidx_find_irtrange (pDiIx->head->hirt, pDiIx->head->hirtSz, iwrd, &irtStart, &irtEnd))
  }
  BS_DO_E_RETN (BsDiIxT2IdxIrd *warec = bsdiixt2tst_find_irtrd (pDiIx, iwrd, irtStart, irtEnd))
  BsDSoundRd *obj = NULL;
  if ( warec != NULL && bsdicidx_istr_cmp (iwrd, warec->iwrd) == 0 )
  {
    obj = bsdsoundrd_new (pWrd, warec);
  }
  bsdiixt2idxird_free (warec);
  return obj;
}
