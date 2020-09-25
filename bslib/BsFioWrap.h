/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ stdio file read/write type-safe wrappers.</p>
 * @author Yury Demidenko
 **/

#ifndef BSFIOWRAP_H

#include "stdio.h"

#include "BsStrings.h"

#define BSFIOWRAP_H 1

/**
 * <p>Write unsigned char into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_uchar(unsigned char *pData, FILE *pFile);

/**
 * <p>Write int into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_int(int *pData, FILE *pFile);

/**
 * <p>Write bool into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bool(bool *pData, FILE *pFile);

/**
 * <p>Write unsigned int into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_uint(unsigned int *pData, FILE *pFile);

/**
 * <p>Goto file position (SEEK_SET).</p>
 * @param pFile - file
 * @param pOfst - offset to go
 * @set errno if error.
 **/
void bsfseek_goto(FILE *pFile, BS_FOFST_T pOfst);

/**
 * <p>Read unsigned char from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_uchar(unsigned char *pDataRet, FILE *pFile);

/**
 * <p>Read char from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_char(char *pDataRet, FILE *pFile);

/**
 * <p>Read int from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_int(int *pDataRet, FILE *pFile);

/**
 * <p>Read bool from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bool(bool *pDataRet, FILE *pFile);

/**
 * <p>Read unsigned int from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_uint(unsigned int *pDataRet, FILE *pFile);

/**
 * <p>Read BS_CHAR_T from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bschar(BS_CHAR_T *pDataRet, FILE *pFile);

/**
 * <p>Read BS_FOFST_T from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bsfoffset(BS_FOFST_T *pDataRet, FILE *pFile);

/**
 * <p>Read BS_IDX_T from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bsindex(BS_IDX_T *pDataRet, FILE *pFile);

/**
 * <p>Read BS_SMALL_T from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bssmall(BS_SMALL_T *pDataRet, FILE *pFile);

/**
 * <p>Read enum from given file.</p>
 * @param pFile - file
 * @return enum value
 * @set errno if error.
 **/
int bsfread_enum(FILE *pFile);

/**
 * <p>Write enum into given file.</p>
 * @param pEnumVl - enum constant
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_enum(int pEnumVl, FILE *pFile);

/**
 * <p>Write long into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_long(long *pData, FILE *pFile);

/**
 * <p>Write BS_SMALL_T into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bssmall(BS_SMALL_T *pData, FILE *pFile);

/**
 * <p>Write BS_IDX_T into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bsindex(BS_IDX_T *pData, FILE *pFile);

/**
 * <p>Write BS_IDX_T array into given file.</p>
 * @param pData - pointer to data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bsindexs(BS_IDX_T *pData, int pCnt, FILE *pFile);

/**
 * <p>Write BS_FOFST_T into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bsfoffset(BS_FOFST_T *pData, FILE *pFile);

/**
 * <p>Write BS_WCHAR_T into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bswchar(BS_WCHAR_T *pData, FILE *pFile);

//TODO all arrays/strings make as objects with size member,
//i.e. instead of bsfwrite_bswchars(arr, 12, file) it must be bsfwrite_bswstring(wstr, file)
/**
 * <p>Write BS_WCHAR_T array into given file.</p>
 * @param pData - pointer to data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bswchars(BS_WCHAR_T *pData, int pCnt, FILE *pFile);

/**
 * <p>Read BS_WCHAR_T array from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bswchars(BS_WCHAR_T *pDataRet, int pCnt, FILE *pFile);

/**
 * <p>Read BS_CHAR_T array from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bschars(BS_CHAR_T *pDataRet, int pCnt, FILE *pFile);

/**
 * <p>Read char array from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_chars (char *pDataRet, int pCnt, FILE *pFile);

/**
 * <p>Read float size string without 0 terminator from given file,
 * string started with uchar length.</p>
 * @param pFile - file
 * @return read string or NULL if error
 * @set errno if error.
 **/
BsString *bsfread_bsstr_lenuchar (FILE *pFile);

/**
 * <p>Write float size string without 0 terminator into given file,
 * string started with uchar length.</p>
 * @param pStr - string
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bsstr_lenuchar (BsString *pStr, FILE *pFile);

/**
 * <p>Read string from given file.</p>
 * @param pStr - string buffer
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bsstring (BsString *pStr, FILE *pFile);

/**
 * <p>Read BS_IDX_T array from given file.</p>
 * @param pDataRet - pointer to return data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfread_bsindexes(BS_IDX_T *pDataRet, int pCnt, FILE *pFile);

/**
 * <p>Write BS_CHAR_T into given file.</p>
 * @param pData - pointer to data
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bschar (BS_CHAR_T *pData, FILE *pFile);

/**
 * <p>Write BS_WHAR_TYPE array into given file.</p>
 * @param pData - pointer to data
 * @param pCnt - chars count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bschars (BS_CHAR_T *pData, int pCnt, FILE *pFile);

/**
 * <p>Write BS_WHAR_TYPE char into given file N times.</p>
 * @param pChr - pointer to char
 * @param pCnt - times count
 * @param pFile - file
 * @set errno if error.
 **/
void bsfwrite_bscharn (BS_CHAR_T *pChrp, int pCnt, FILE *pFile);

#endif
