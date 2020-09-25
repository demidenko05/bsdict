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

#ifndef BS_DEBUGL_DICLSA
#define BS_DEBUGL_DICLSA 31600

#include <vorbis/vorbisenc.h>

#include "BsDiIxT2.h"

// A crude .wav header which is sufficient for our needs
typedef struct 
{
  char riff[ 4 ]; // RIFF
  uint32_t riffLength;
  char waveAndFmt[ 8 ]; // WAVEfmt%20
  uint32_t fmtLength; // 16
  uint16_t formatTag; // 1
  uint16_t channels; // 1 or 2
  uint32_t samplesPerSec;
  uint32_t bytesPerSec;
  uint16_t blockAlign;
  uint16_t bitsPerSample; // 16
  char data[ 4 ]; // data
  uint32_t dataLength;
} GdWavHeader;

/**
 * <p>D.SOUND record.</p>
 * @extends DSDISRDT2
 * @member BsString *wrd - word
 **/
typedef struct {
  BsString *wrd;
  DSDISRDT2
} BsDSoundRd;

/**
 * <p>Constructor.</p>
 * @param pWdOw word to own
 * @param pRcd record
 * @return object or NULL when error
 * @set errno if error.
 **/
BsDSoundRd *bsdsoundrd_new (BsString *pWdOw, BsDiIxT2IdxIrd *pRcd);

/**
 * <p>Destructor.</p>
 * @param pRcd record
 * @return always NULL
 **/
BsDSoundRd *bsdsoundrd_free (BsDSoundRd *pRcd);

//Making DIC with IDX main lib:

/**
 * <p>Read all words from dictionary file.</p>
 * @param pDiIxRm opened rewind dictionary bundle
 * @param pIcDs - iconv descriptor
 * @param pDiWd_Csm consumer of just read d.word, audio offset and length
 * @param pInstr consumer's additional instrument
 * @return BSR_OK_END or consumer's last return value, e.g. BSR_OK_ENOUGH or errno
 * @set errno if error.
 **/
int bsdiixlsa_iter (BsDiIxT2Rm *pDiIxRm, iconv_t pIcDs, BsDiAuWd_Csm *pDiWd_Csm, void *pInstr);

/**
 * <p>Create LSA with index bundle in memory from dictionary file.</p>
 * @param pPth - dictionary path.
 * @param pOpSt - opening state data shared with client
 * @return opened dictionary with index bundle in memory
 * @set errno if error.
 **/
BsDiIxT2Rm* bsdiixlsarm_create (char *pPth, BsDiIxOst* pOpSt);

/**
 * <p>Open DIC IDX, i.e. load or create then load.</p>
 * @param pPth - dictionary path.
 * @param pOpSt - opening state data shared with client
 * @return object or NULL if error
 * @set errno if error.
 **/
BsDiIxT2 *bsdiixlsa_open (char *pPth, BsDiIxOst* pOpSt);

/**
 * <p>Just make sound description.</p>
 * @param pDiIx - DIC with IDX
 * @param pFdWrd - found word with data to search content
 * @return full description as BsHypStrs
 * @set errno if error.
 **/
BsHypStrs *bsdiclsa_read (BsDiIxT2 *pDiIx, BsDiFdWd *pFdWrd);

//Find lib:

/**
 * <p>Find all matched words in given dictionary and IDX.</p>
 * @param pDiIx - DIC with IDX
 * @param pFdWrds - collection to add found record
 * @param pSbwrd - sub-word to match
 * @set errno if error.
 **/
void bsdiclsafind_mtch (BsDiIxT2 *pDiIx, BsDiFdWds *pFdWrds, char *pSbwrd);

//useful for tests purposes:
/**
 * <p>Find exactly matched word with sound data in given dictionary and IDX.</p>
 * @param pDiIx IDX with head, opened DIC and IDX
 * @param pWrd - word to match
 * @return dic.sound-record or NULL if not found or error
 * @set errno if error.
 **/
BsDSoundRd *bsdiclsatst_find (BsDiIxT2 *pDiIx, BsString *pWrd);

//Sound lib:
/**
 * <p>Vorbis file read delegator.</p>
 * @param pDtRt - data pointer to return
 * @param pSz - block size
 * @param pCnt - blocks count
 * @param pDs - dic-idx bundle
 * return file read result
 **/
size_t bs_vorbis_read (void *pDtRt, size_t pSz, size_t pCnt, void *pDs);

/**
 * <p>Vorbis file seek delegator.</p>
 * @param pDs - dic-idx bundle
 * @param pOfst - offset
 * @param pWhe - whence
 * return file seek result
 **/
int bs_vorbis_seek (void *pDs, ogg_int64_t pOfst, int pWhe);

/**
 * <p>Vorbis file tell delegator.</p>
 * @param pDs - dic-idx bundle
 * return file tell result
 **/
long bs_vorbis_tell (void* pDs);

/**
 * <p>Read audio data from LSA file.</p>
 * @param pDiIx - dictionary with index.
 * @param pRecOfst - audio record offset from vorbis start
 * @param pRecLen - audio record len
 * @param pMkHd - if make WAV header
 * @return audio data WAV file in memory (PCM data with optional RIFF header according GoldenDict source)
 * @set errno if error.
 **/
BsStrBuf* bsdiclsa_readau (BsDiIxT2Bs *pDiIx, unsigned int pRecOfst, unsigned int pRecLen, bool pMkHd);

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
BsStrBuf* bsdiclsa_encvorbis (BsStrBuf *pAuDt, int pChan, long pRate);
#endif
