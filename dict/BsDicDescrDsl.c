/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

#include "stdlib.h"
#include "string.h"

#include "BsError.h"
#include "BsLog.h"
#include "BsFioWrap.h"
#include "BsDicDescrDsl.h"

/**
 * <p>Beigesoft™ DSL dictionary word's description reader lib.</p>
 * @author Yury Demidenko
 **/

/**
 * <p>Read full description with substituted DIC's tags by HTML ones.</p>
 * @param pDicFl - dictionary
 * @param p_wstart offset d.word
 * @return full description as
 * @set errno if error.
 **/
BsHypStrs*
  bsdicdescrdsl_read (FILE *pDicFl, BS_FOFST_T p_wstart)
{
  BS_DO_E_RETN(bsfseek_goto (pDicFl, p_wstart))
  BSSTRINGBUFFER_NEW_E_RETN (descBuf, BSDICDESCR_BUF_SZ)
  BsHypStrs *hstrs = NULL;
  BsHypTags *htags = NULL;
  BSSTRINGBUFFER_NEW_E_OUTE (tag_buf, 20L)
  BS_DO_E_OUTE(hstrs = bshypstrs_new (100L))
  BS_DO_E_OUTE(htags = bshyptags_new (BSDICDESCR_TAGS_MAX_SIZE))
  //goto description:
  bool is_prev_nl = false;
  char chr;
  while ( true )
  {
    BS_DO_E_OUTE (bsfread_char (&chr,  pDicFl))
    if ( chr == '\n' )
    {
      is_prev_nl = true;
    } else {
      if ( is_prev_nl && ( chr == '\t' || chr == ' ') )
                              { break; }
      is_prev_nl = false;
    }
  }
  //parse description:
  is_prev_nl = false;
  bool is_tag = false;
  bool is_end_tag = false;
  while ( true )
  {
    BS_DO_E_OUTE (bsfread_char (&chr, pDicFl))
    if ( chr == '\n' )
    {
      is_prev_nl = true;
      BS_DO_E_OUTE (bsstrbuf_add_inc (descBuf, chr, BSDICDESCR_BUF_SZ))
    } else {
      if ( is_prev_nl && !(chr == '\t' || chr == ' ') ) //new word
                                { break; }
      is_prev_nl = false;
      if ( chr == '[' )
      {
        if ( descBuf->size > BS_IDX_0 ) {
          if ( descBuf->vals[descBuf->size - BS_IDX_1] == '\\' )
          { //remove escaped link start:
            bsstrbuf_clear_last (descBuf);
            BS_DO_E_OUTE (bsstrbuf_add_inc (descBuf, chr, BSDICDESCR_BUF_SZ))
            continue;
          }
          //save old string:  //TODO 1 remove TAB tags? in the same string (until new line)
          BS_DO_E_OUTE (bsstrbuf_add_inc (descBuf, 0, BS_IDX_1))
          BS_DO_E_OUTE (bshypstrs_add_inc (hstrs, bshypstr_new (
                descBuf->vals, htags, UINT_MAX, UINT_MAX), 20L))
          bsstrbuf_clear (descBuf);
        }
        is_tag = true;
      } else {
        if ( is_tag )
        {
          if ( chr == ']' )
          {
            if ( !is_end_tag )
            {
              EBsHypTag tag = bsdicdescrdsl_to_tag (tag_buf);
              if (tag != EBSHT_EMPTY)
                    { BS_DO_E_OUTE (bshyptags_add (htags, tag)) }
            }
            is_tag = false;
            is_end_tag = false;
            bsstrbuf_clear (tag_buf);
          } else if (chr == '/') {
            is_end_tag = true;
            BS_DO_E_OUTE (bshyptags_remove_last_tlrn (htags))
          } else if ( !is_end_tag ) {
            BS_DO_E_OUTE (bsstrbuf_add_inc (tag_buf, chr, BSDICDESCR_BUF_SZ))
          }
        } else {
          if (chr == ']')
          {
            if ( descBuf->size > BS_IDX_0 
                  && descBuf->vals[descBuf->size - BS_IDX_1] == '\\' )
            { //remove escaped link start:
              bsstrbuf_clear_last (descBuf);
              BS_DO_E_OUTE (bsstrbuf_add_inc (descBuf, chr, BSDICDESCR_BUF_SZ))
              continue;
            }
            BSLOG_LOG (BSLWARN, "can't escape link closing\n");
          } else if (chr != '\t') {
            if ( chr == '<' && descBuf->size > BS_IDX_0 
                  && descBuf->vals[descBuf->size - BS_IDX_1] == '<' )
            { //remove link start quasi-tag:
              bsstrbuf_clear_last (descBuf);
              continue;
            }
            if ( chr == '>' && descBuf->size > BS_IDX_0 
                  && descBuf->vals[descBuf->size - BS_IDX_1] == '>' )
            { //remove link end quasi-tag:
              bsstrbuf_clear_last (descBuf);
              continue;
            }
            if ( chr == ' ' && descBuf->size > BS_IDX_0 
                  && descBuf->vals[descBuf->size - BS_IDX_1] == '\\' )
            { //remove useless \ in empty string:
              bsstrbuf_clear_last (descBuf);
            }
            BS_DO_E_OUTE (bsstrbuf_add_inc (descBuf, chr, BSDICDESCR_BUF_SZ))
          }
        }
      }
    }
  }
  bsstrbuf_free (descBuf);
  bsstrbuf_free (tag_buf);
  bshyptags_free (htags);
  return hstrs;
oute:
  bsstrbuf_free (descBuf);
  bsstrbuf_free (tag_buf);
  bshyptags_free (htags);
  bshypstrs_free (hstrs);
  return NULL;
}

/**
 * <p>Converts string tag into enum. It's a tolerate method.
 * It returns EBSHT_EMPTY if data wrong.</p>
 * @param pStrBuf string tag
 * @return enum tag
 **/
EBsHypTag
  bsdicdescrdsl_to_tag (BsStrBuf *pStrBuf)
{
  if (pStrBuf->size > BS_IDX_0) {
    //spam-tags:
    if ( strncmp ("com", pStrBuf->vals, 3) == 0 )
    {
      return EBSHT_EMPTY;
    }
    else if ( strncmp ("trn", pStrBuf->vals, 3) == 0 )
    {
       return EBSHT_EMPTY;
    }//others:
    else if ( strncmp ("c green", pStrBuf->vals, 7) == 0 )
    {
      return EBSHT_GREEN;
    }
    else if ( strncmp ("c silver", pStrBuf->vals, 8) == 0 )
    {
      return EBSHT_GRAY;
    }
    else if ( strncmp ("c gray", pStrBuf->vals, 6) == 0 )
    {
      return EBSHT_GRAY;
    }
    else if ( strncmp ("c red", pStrBuf->vals, 5) == 0 ) {
      return EBSHT_RED;
    }
    else if ( strncmp ("c ", pStrBuf->vals, 2) == 0 )
    {
      return EBSHT_EMPTY;
    }
    else if ( strncmp ("m", pStrBuf->vals, 1) == 0 ) {
      int idx;
      sscanf (pStrBuf->vals, "m%d", &idx);
      if (idx > 10) {
        idx = 10;
      } else if (idx < 0) {
        idx = 1;
      }
      return EBSHT_TAB1 + idx - 1;
    } else if ( strncmp ("p", pStrBuf->vals, 1) == 0 ) {
      return EBSHT_TOOLTIP;
    } else if ( strncmp ("i", pStrBuf->vals, 1) == 0 ) {
      return EBSHT_ITALIC;
    } else if ( strncmp ("c", pStrBuf->vals, 1) == 0 ) {
      return EBSHT_GREEN;
    } else if ( strncmp ("b", pStrBuf->vals, 1) == 0 ) {
      return EBSHT_BOLD;
    }
  } else {
    pStrBuf->vals[pStrBuf->size - BS_IDX_1] = 0;
    BSLOG_LOG (BSLWARN, "Tag '%s' doesn't recognized!\n", pStrBuf->vals)
    return EBSHT_EMPTY;
  }
  //BSLOG_LOG (BSLWARN, "Attempt to get tag from empty array!\n")
  return EBSHT_EMPTY;
}
