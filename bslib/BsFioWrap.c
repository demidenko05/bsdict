/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ stdio file read/write type-safe wrappers.</p>
 * @author Yury Demidenko
 **/

//#include "stdio.h"

#include "BsFioWrap.h"
#include "BsError.h"
#include "BsLog.h"


/**
 * <p>Goto file position (SEEK_SET).</p>
 * @param pFile - file
 * @param pOfst - offset to go
 * @set errno if error.
 **/
void bsfseek_goto(FILE *pFile, BS_FOFST_T pOfst) {
  int rz = fseek(pFile, pOfst, SEEK_SET);
  if (rz != 0) {
    if (errno == 0) { errno = BSE_SEEK_FILE; }
    BSLOG_LOG(BSLERROR, "\n  Can't goto offset=%ld in file#%p\n", pOfst, pFile)
  }
}
/**
 * <p>Write unsigned char into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_uchar(unsigned char *pData, FILE *pFile) {
  int cnt = 1;
  int wcr = fwrite(pData, sizeof(unsigned char), cnt, pFile);
  if (wcr != cnt) {
    if (errno == 0) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Read enum from given file.</p>
 * @param pFile - file
 * @return enum value
 * @set errno if error.
 **/
int bsfread_enum(FILE *pFile) {
  int cnt = 1;
  int ev;
  int rcr = fread(&ev, sizeof(int), cnt, pFile);
  if (rcr != cnt) {
    if (errno == 0) { errno = BSE_READ_FILE; }
    BSLOG_ERR
  }
  return ev;
}

/**
 * <p>Read char from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_char(char *pDataRet, FILE *pFile) {
  int cnt = 1;
  int rcr = fread(pDataRet, sizeof(char), cnt, pFile);
  if (rcr != cnt) {
    if (errno == 0) { errno = BSE_READ_FILE; }
    BSLOG_LOG (BSLERROR, "file#%p, offset=%ld\n", pFile, ftell (pFile)) 
  }
}

/**
 * <p>Read unsigned char from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_uchar(unsigned char *pDataRet, FILE *pFile) {
  int cnt = 1;
  int rcr = fread(pDataRet, sizeof(unsigned char), cnt, pFile);
  if (rcr != cnt) {
    if (errno == 0) { errno = BSE_READ_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Read int from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_int(int *pDataRet, FILE *pFile) {
  int cnt = 1;
  int rcr = fread(pDataRet, sizeof(int), cnt, pFile);
  if (rcr != cnt) {
    if (errno == 0) { errno = BSE_READ_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Read bool from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void
  bsfread_bool (bool *pDataRet, FILE *pFile)
{
  int cnt = 1;
  int rcr = fread(pDataRet, sizeof (bool), cnt, pFile);
  if ( rcr != cnt )
  {
    if ( errno == 0 ) { errno = BSE_READ_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Read unsigned int from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void
  bsfread_uint (unsigned int *pDataRet, FILE *pFile)
{
  int cnt = 1;
  int rcr = fread (pDataRet, sizeof (unsigned int), cnt, pFile);
  if ( rcr != cnt ) {
    if ( errno == 0 ) { errno = BSE_READ_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Read BS_CHAR_T from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bschar(BS_CHAR_T *pDataRet, FILE *pFile) {
  int cnt = 1;
  int rcr = fread(pDataRet, BS_CHAR_LEN, cnt, pFile);
  if (rcr != cnt) {
    if (errno == 0) { errno = BSE_READ_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Read BS_FOFST_T from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bsfoffset(BS_FOFST_T *pDataRet, FILE *pFile) {
  int cnt = 1;
  int rcr = fread(pDataRet, BS_FOFST_LEN, cnt, pFile);
  if (rcr != cnt) {
    if (errno == 0) { errno = BSE_READ_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Read BS_IDX_T from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bsindex(BS_IDX_T *pDataRet, FILE *pFile) {
  int cnt = 1;
  int rcr = fread(pDataRet, BS_IDX_LEN, cnt, pFile);
  if (rcr != cnt) {
    if (errno == 0) { errno = BSE_READ_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Read BS_SMALL_T from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bssmall(BS_SMALL_T *pDataRet, FILE *pFile) {
  int cnt = 1;
  int rcr = fread(pDataRet, BS_SMALL_LEN, cnt, pFile);
  if (rcr != cnt) {
    if (errno == 0) { errno = BSE_READ_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Write int into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_int(int *pData, FILE *pFile) {
  int cnt = 1;
  int wcr = fwrite(pData, sizeof(int), cnt, pFile);
  if (wcr != cnt) {
    if (errno == 0) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Write bool into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void
  bsfwrite_bool (bool *pData, FILE *pFile)
{
  int cnt = 1;
  int wcr = fwrite(pData, sizeof (bool), cnt, pFile);
  if ( wcr != cnt )
  {
    if ( errno == 0 ) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}


/**
 * <p>Write unsigned int into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void
  bsfwrite_uint (unsigned int *pData, FILE *pFile)
{
  int cnt = 1;
  int wcr = fwrite(pData, sizeof(unsigned int), cnt, pFile);
  if ( wcr != cnt )
  {
    if ( errno == 0 ) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Write enum into given file.</p>
 * @param pEnumVl - enum constant
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_enum(int pEnumVl, FILE *pFile) {
  int cnt = 1;
  int wcr = fwrite(&pEnumVl, sizeof(int), cnt, pFile);
  if (wcr != cnt) {
    if (errno == 0) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Write long into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_long(long *pData, FILE *pFile) {
  int cnt = 1;
  int wcr = fwrite(pData, sizeof(long), cnt, pFile);
  if (wcr != cnt) {
    if (errno == 0) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Write BS_SMALL_T into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bssmall(BS_SMALL_T *pData, FILE *pFile) {
  int cnt = 1;
  int wcr = fwrite(pData, BS_SMALL_LEN, cnt, pFile);
  if (wcr != cnt) {
    if (errno == 0) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Write BS_IDX_T into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bsindex(BS_IDX_T *pData, FILE *pFile) {
  int cnt = 1;
  int wcr = fwrite(pData, BS_IDX_LEN, cnt, pFile);
  if (wcr != cnt) {
    if (errno == 0) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Write BS_IDX_T array into given file.</p>
 * @param pData - pointer to data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bsindexs(BS_IDX_T *pData, int pCnt, FILE *pFile) {
  int wcr = fwrite(pData, BS_IDX_LEN, pCnt, pFile);
  if (wcr != pCnt) {
    if (errno == 0) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Write BS_FOFST_T into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bsfoffset(BS_FOFST_T *pData, FILE *pFile) {
  int cnt = 1;
  int wcr = fwrite(pData, BS_FOFST_LEN, cnt, pFile);
  if (wcr != cnt) {
    if (errno == 0) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Write BS_WCHAR_T into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bswchar(BS_WCHAR_T *pData, FILE *pFile) {
  int cnt = 1;
  int wcr = fwrite(pData, BS_WCHAR_LEN, cnt, pFile);
  if (wcr != cnt) {
    if (errno == 0) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Write BS_WCHAR_T array into given file.</p>
 * @param pData - pointer to data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bswchars(BS_WCHAR_T *pData, int pCnt, FILE *pFile) {
  int wcr = fwrite(pData, BS_WCHAR_LEN, pCnt, pFile);
  if (wcr != pCnt) {
    if (errno == 0) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Read BS_WCHAR_T array from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bswchars(BS_WCHAR_T *pDataRet, int pCnt, FILE *pFile) {
  int rcr = fread(pDataRet, BS_WCHAR_LEN, pCnt, pFile);
  if (rcr != pCnt) {
    if (errno == 0) { errno = BSE_READ_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Read BS_CHAR_T array from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bschars(BS_CHAR_T *pDataRet, int pCnt, FILE *pFile) {
  int rcr = fread(pDataRet, BS_CHAR_LEN, pCnt, pFile);
  if (rcr != pCnt) {
    if (errno == 0) { errno = BSE_READ_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Read char array from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_chars(char *pDataRet, int pCnt, FILE *pFile) {
  int rcr = fread(pDataRet, sizeof(char), pCnt, pFile);
  if (rcr != pCnt) {
    if (errno == 0) { errno = BSE_READ_FILE; } //it happens when file was read by wfread
    BSLOG_LOG(BSLERROR, "Can't read %d chars from offset "BS_FOFST_FMT", file=%p\n", pCnt, ftell(pFile), pFile)
  }
}

/**
 * <p>Read float size string from given file without 0 terminator,
 * string started with uchar length.</p>
 * @param pFile - file
 * @return read string or NULL if error
 * @set errno if error.
 **/
BsString*
  bsfread_bsstr_lenuchar (FILE *pFile)
{
  int rcr;
  unsigned char len;
  BsString *str = NULL;
  rcr = fread(&len, sizeof (unsigned char), 1, pFile);
  if ( rcr != 1 )
  {
    if ( errno == 0 ) { errno = BSE_READ_FILE; }
    BSLOG_ERR
    return NULL;
  }
  int sz = ((int) len) + 1;
  BS_DO_E_RETN (str = bsstring_newbuf (sz))
  rcr = fread (str->val, sizeof (char), str->len, pFile);
  if ( rcr != str->len )
  {
    if ( errno == 0 ) { errno = BSE_READ_FILE; }
    BSLOG_ERR
    str = bsstring_free (str);
  }
  return str;
}

/**
 * <p>Write float size string into given file without 0 terminator,
 * string started with uchar length.</p>
 * @param pStr - string
 * @param pFile - file
 * @set errno if error.
 **/
void
  bsfwrite_bsstr_lenuchar (BsString *pStr, FILE *pFile)
{
  int wcr;
  unsigned char len;
  if ( pStr->len > UCHAR_MAX )
  {
    errno = BSE_WRONG_PARAMS;
    BSLOG_LOG (BSLERROR, "pStr->len=%d > UCHAR_MAX!\n", pStr->len)
    return;
  }
  len = (unsigned char) pStr->len;
  wcr = fwrite (&len, sizeof (unsigned char), 1, pFile);
  if ( wcr != 1 )
  {
    if ( errno == 0 ) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
  wcr = fwrite (pStr->val, sizeof (char), pStr->len, pFile);
  if ( wcr != pStr->len )
  {
    if ( errno == 0 ) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Read string from given file.</p>
 * @param pStr - string buffer
 * @param pFile - file
 * @set errno if error.
 **/
void
  bsfread_bsstring (BsString *pStr, FILE *pFile)
{
  int rcr = fread (pStr->val, sizeof (char), pStr->len, pFile);
  if ( rcr != pStr->len )
  {
    if ( errno == 0 ) { errno = BSE_READ_FILE; } //it happens when file was read by wfread
    BSLOG_LOG(BSLERROR, "Can't read %d chars from offset "BS_FOFST_FMT", file=%p\n", pStr->len, ftell (pFile), pFile)
  }
}

/**
 * <p>Read BS_IDX_T array from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bsindexes(BS_IDX_T *pDataRet, int pCnt, FILE *pFile) {
  int rcr = fread(pDataRet, BS_IDX_LEN, pCnt, pFile);
  if (rcr != pCnt) {
    if (errno == 0) { errno = BSE_READ_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Write BS_CHAR_T into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bschar(BS_CHAR_T *pData, FILE *pFile) {
  int cnt = 1;
  int wcr = fwrite(pData, BS_CHAR_LEN, cnt, pFile);
  if (wcr != cnt) {
    if (errno == 0) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Write BS_CHAR_T array into given file.</p>
 * @param pData - pointer to data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bschars(BS_CHAR_T *pData, int pCnt, FILE *pFile) {
  int wcr = fwrite(pData, BS_CHAR_LEN, pCnt, pFile);
  if (wcr != pCnt) {
    if (errno == 0) { errno = BSE_WRITE_FILE; }
    BSLOG_ERR
  }
}

/**
 * <p>Write BS_WHAR_TYPE char into given file N times.</p>
 * @param pChrp - poiner to char
 * @param pCnt - times count
 * @param pFile - file
 * @set errno if error.
 **/
void
  bsfwrite_bscharn (BS_CHAR_T *pChrp, int pCnt, FILE *pFile)
{
  int wrc, i;
  for ( i = 0; i < pCnt; i++ )
  {
    wrc = fwrite (&pChrp, BS_CHAR_LEN, 1, pFile);
    if ( wrc != 1 )
    {
      if ( errno == 0 ) { errno = BSE_WRITE_FILE; }
      BSLOG_ERR
    }
  }
}
