/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ basic constants and types.</p>
 * @author Yury Demidenko
 **/
#ifndef BS_FOFST_T

#include "limits.h"
#include "stddef.h"

//Common types:

/* Widely used delegator type */
typedef void BsVoid_Method(void);

/* Comparator of objects - -1 less 0 equal 1 greater */
typedef int Bs_Compare(void*, void*);

/* Generic destructor */
typedef void *Bs_Destruct(void*);

//detail logging on initial data evaluated, e.g. making IDX
#define BS_USE_LOG_DETAIL1 1

//basic level debug logging on/off, e.g. reporting info on start/exit/init/destroing
#define BS_DEBUG_BASIC_ENABLED 1

//constants:
  //ISO C 9899:1999 offset in file - int fseek(FILE *stream, long int offset, int whence);
    //record type in DWOLT:
  //TODO use fpos_t instead? but it seems it is not designed for arbitrary positioning, i.e. point to record%#N
  //it should be unsigned long
#define BS_FOFST_T long
#define BS_FOFST_LEN sizeof(BS_FOFST_T)
#define BS_FOFST_0 0L
#define BS_FOFST_NULL -1L
#define BS_FOFST_FMT "%ld"

#define BS_IDX_T long
#define BS_IDX_LEN sizeof(BS_IDX_T)
#define BS_IDX_0 0L
#define BS_IDX_MAX LONG_MAX
#define BS_IDX_NULL -1L
#define BS_IDX_1 1L
#define BS_IDX_2 2L
#define BS_IDX_10 10L
#define BS_IDX_100 100L
#define BS_IDX_200 200L
#define BS_IDX_300 300L
#define BS_IDX_1000 1000L
#define BS_IDX_10000 10000L
#define BS_IDX_FMT "%ld"
  //e.g. length of word:
#define BS_SMALL_T unsigned short
#define BS_SMALL_LEN sizeof(BS_CHAR_T)
#define BS_SMALL_NULL USHRT_MAX
  //char type, up to 65536 cause multi-lang dictionary:
#define BS_CHAR_T unsigned short
#define BS_CHAR_LEN sizeof(BS_CHAR_T)
#define BS_CHAR_MAX USHRT_MAX
#define BS_WCHAR_T wchar_t
#define BS_WCHAR_LEN sizeof(BS_WCHAR_T)

  //legacy BOOLEAN:
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

//Return results:
  //return BS_IDX_T
    //Current word is duplicate 
#define BSRL_INDEX_DUPLICATE -100L
    //Error
#define BSRL_ERR -101L

  //iterator-consumer: 2000-2099
#define BSR_OK_END 2000 //iterator went through end
#define BSR_OK_ENOUGH 2001 //e.g. stop iteration cause matched data is enough
#define BSR_OK_DEBUG 2002 //e.g. stop iteration cause tester receive enough data
#endif
