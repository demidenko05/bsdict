/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ logger library. A file is the best alternative to output logs.
 * If a log file exceed max size, then it will be overwritten to avoid "out of order" cause huge log files.
 * It is designed for using multiply log-files. The first log file will be used as main log when client didn't choose file.</p>
 * This is fault-tolerated log, so wrong initialized log will print into stdout/err.
 * @author Yury Demidenko
 **/

#include "time.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "pthread.h"

#include "BsLog.h"
#include "BsError.h"

//Constants:
static const char *s_log_levels[] = { "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "TEST" };
//State:

/* Clients often require different files,
 * so this log file with path:
 */

//main program should initialize it and pass file's indexes to clients
static BsLogFiles *s_log_files;

static BsLogDbgRanges *s_dranges = NULL;

//They do not need to synchronization, beside they have almost atomic-behavior:
static int s_debug_level_floor = 0;

static int s_debug_level_ceiling = 0;

static BS_FOFST_T s_maximum_size = 1048576L; //default 1MB

//fprintf locks stream itself but changing file that excess maximum size must be locked
static pthread_mutex_t sMutex;
static bool sMutexOk = false;

//Lib:

/**
 * <p>Destructor.</p>
 * @param p_drange range, maybe NULL
 * @return always NULL
 **/
BsLogDbgRange *bslogdbgrange_free(BsLogDbgRange *p_drange) {
  if (p_drange != NULL) {
    free(p_drange);
  }
  return NULL;
}

/**
 * <p>Constructor.</p>
 * @param p_floor floor
 * @param p_ceiling ceiling
 * @return range or NULL when OOM
  **/
BsLogDbgRange *bslogdbgrange_new(int p_floor, int p_ceiling) {
  BsLogDbgRange *obj = malloc(sizeof(BsLogDbgRange));
  if (obj == NULL) {
    BSLOG_LOG(BSLWARN, "Out of memory!\n");
  } else {
    obj->floor = p_floor;
    obj->ceiling = p_ceiling;
  }
  return obj;
}

/**
 * <p>Destructor.</p>
 * @param pSet - collection
 * @return always NULL
 **/
BsLogDbgRanges *bslogdbgranges_free(BsLogDbgRanges *pSet) {
  if (pSet != NULL) {
    if (pSet->vals != NULL) {
      for (int l = 0; l < pSet->bsize; l++) {
        if (pSet->vals[l] != NULL) {
          bslogdbgrange_free(pSet->vals[l]);
        }
      }
      free(pSet->vals);
    }
    free(pSet);
  }
  return NULL;
}

/**
 * <p>Constructor.</p>
 * @param pBufSz buffer size, must be more than 0
 * @return object or NULL when OOM
 **/
BsLogDbgRanges *bslogdbgranges_new(int pBufSz) {
  if (pBufSz <= 0) {
    pBufSz = 10;
  }
  BsLogDbgRanges *obj = malloc(sizeof(BsLogDbgRanges));
  if (obj != NULL) {
    obj->vals = malloc(pBufSz * sizeof(void*));
    if (obj->vals == NULL) {
      obj = bslogdbgranges_free(obj);
    } else {
      obj->bsize = pBufSz;
      obj->size = 0;
      for (int l = 0; l < obj->bsize; l++) {
       obj->vals[l] = NULL;
      }
    }
  }
  if (obj == NULL) {
    BSLOG_LOG(BSLWARN, "Out of memory!\n");
  }
  return obj;
}

/**
 * <p>Add object to the first null cell of collection.
 * It increases collection if it's full-filled.</p>
 * @param pSet - collection
 * @param pObj - object
 * @param pInc - how much realloc when adding into
 *   full-filled collection, if 0 or less then error
 * @set errno - BSE_ARR_SET_NULL, BSE_ARR_OUT_MAX_SIZE, BSE_ARR_WPINCSIZE or ENOMEM
 **/
void bslogdbgranges_add_inc(BsLogDbgRanges *pSet, BsLogDbgRange *pObj, int pInc) {
  if (pObj == NULL) {
    return;
  }
  if (pSet->size == pSet->bsize) {
    int newsz;
    if (pInc <= 0) {
      newsz = pSet->bsize + 10;
    } else {
      newsz = pSet->bsize + pInc;
    }
    if (newsz <= pSet->bsize || newsz < 0) { //overflow
      BSLOG_LOG(BSLWARN, "Overflow!\n");
      return;
    }
    pSet->vals = realloc(pSet->vals, newsz * sizeof(void*));
    if (pSet->vals == NULL) {
      BSLOG_LOG(BSLWARN, "Out of memory!\n");
      pSet->bsize = 0;
      pSet->size = 0;
      return;
    }
    pSet->bsize = newsz;
    for (int l = pSet->size; l < pSet->bsize; l++) {
      pSet->vals[l] = NULL;
    }
  }
  pSet->vals[pSet->size] = pObj;
  pSet->size++;
}
  //intermediate public testable:
/**
 * <p>Make log message to given file with given index.
 * If wrong initialized, then print into stdout/err.</p>
 * @param p_file_idx - index of log file
 * @param p_tag TAG maybe NULL
 * @param p_levomsg Log level or print only message without date, thread#, tag and level
 * @param p_fmt message or format
 * @param p_vl var list
 **/
void bslogtst_log_to(const int p_file_idx, const char *p_tag, BsELogLevOmsg p_levomsg, char *p_fmt, va_list p_vl) {
  if (s_log_files != NULL && s_log_files->files[p_file_idx]->file != NULL) {
    if (sMutexOk) { pthread_mutex_lock(&sMutex); }
    BS_FOFST_T ofst = ftell(s_log_files->files[p_file_idx]->file);
    if (ofst > s_maximum_size) {
      fclose(s_log_files->files[p_file_idx]->file);
      s_log_files->files[p_file_idx]->file = NULL;
      int len = strlen(s_log_files->files[p_file_idx]->pth);
      if (strstr(s_log_files->files[p_file_idx]->pth + len - 3, BSLOG_EXT)) {
        strcpy(s_log_files->files[p_file_idx]->pth + len - 3, BSLOG_EXT_SECOND);
      } else {
        strcpy(s_log_files->files[p_file_idx]->pth + len - 3, BSLOG_EXT);
      }
      s_log_files->files[p_file_idx]->file = fopen(s_log_files->files[p_file_idx]->pth, "w");
    }
  }
  if (s_log_files == NULL || s_log_files->files[p_file_idx]->file == NULL) {
    if (p_fmt != NULL) {
      if (p_levomsg != BSLONLYMSG) {
        fprintf(stdout, "%s: ", s_log_levels[p_levomsg]);
      }
      if (p_tag != NULL) {
        fprintf(stdout, "%s: ", p_tag);
      }
      vfprintf(stdout, p_fmt, p_vl);
    }
    if (errno != 0) {
      fprintf(stderr, "ERROR! %s\n", bserror_to_str(errno));
    }
    return;
  }
  if (p_levomsg != BSLONLYMSG) {
    struct timespec tsp;
    int trz = timespec_get(&tsp, TIME_UTC);
    if (trz != 0) {
      long msec = (tsp.tv_nsec / 1000000LL) % 1000L;
      struct tm *tmsct = localtime(&tsp.tv_sec);
      if (tmsct != NULL) {
        char tms[20];
        strftime(tms, 20, "%d/%m/%g %H:%M:%S", tmsct);
        fprintf(s_log_files->files[p_file_idx]->file, "%s.%ld", tms, msec);
      } else {
        char *tms = ctime(&tsp.tv_sec);
        char *tmsnl = strchr(tms, '\n');
        if (tmsnl != NULL) {
          tmsnl[0] = 0;
        }
        fprintf(s_log_files->files[p_file_idx]->file, "%s.%ld", tms, msec);
      }
    }
    pthread_t slptr = pthread_self();
    fprintf(s_log_files->files[p_file_idx]->file, " thread#%ld", slptr);
    if (p_tag != NULL) {
      fprintf(s_log_files->files[p_file_idx]->file, " %s: %s:", p_tag, s_log_levels[p_levomsg]);
    } else {
      fprintf(s_log_files->files[p_file_idx]->file, " %s:", s_log_levels[p_levomsg]);
    }
    if (errno != 0) {
      fprintf(s_log_files->files[p_file_idx]->file, " %s", bserror_to_str(errno));
    }
  }
  if (p_fmt != NULL) {
    vfprintf(s_log_files->files[p_file_idx]->file, p_fmt, p_vl);
  }
  fflush(s_log_files->files[p_file_idx]->file);
  if (sMutexOk) { pthread_mutex_unlock(&sMutex); }
}

  //Lib:
/**
 * <p>Make log message into the first log file.</p>
 * @param p_levomsg Log level or print only message without date, thread#, tag and level
 * @param p_fmt message or format
 **/
void bslog_log(BsELogLevOmsg p_levomsg, char *p_fmt, ...) {
  va_list vl;
  va_start(vl, p_fmt);
  bslogtst_log_to(0, NULL, p_levomsg, p_fmt, vl);
  va_end(vl);
}

/**
 * <p>Make log message.</p>
 * @param pLgCnst pointer to log constant data
 * @param p_levomsg Log level or print only message without date, thread#, tag and level
 * @param p_fmt message or format
 **/
void bslog_log_to(BsLogConst *pLgCnst, BsELogLevOmsg p_levomsg, char *p_fmt, ...) {
  va_list vl;
  va_start(vl, p_fmt);
  if (pLgCnst == NULL) {
    bslogtst_log_to(0, pLgCnst->tag, p_levomsg, p_fmt, vl);
  } else {
    bslogtst_log_to(pLgCnst->file_index, pLgCnst->tag, p_levomsg, p_fmt, vl);
  }
  va_end(vl);
}

/**
 * <p>Try to open log file with respect to maximum size.</p>
 * @param p_log_file log file
 * @set errno if error.
 **/
void bslogtst_file_init_open(BsLogFile *p_log_file) {
  char *initstr = "BS-LOG try init...\n";
  p_log_file->file = fopen(p_log_file->pth, "a");
  if (p_log_file->file == NULL) {
    errno = BSE_LOG_INIT_A_FILE;
    perror(p_log_file->pth);
    return;
  } else if (fprintf(p_log_file->file, initstr) < 0) {
    errno = BSE_LOG_INIT_WRITE;
    perror(p_log_file->pth);
    return;
  }
  BS_FOFST_T ofst = ftell(p_log_file->file);
  if (ofst > s_maximum_size) {
    fclose(p_log_file->file);
    p_log_file->file = NULL;
    int len = strlen(p_log_file->pth);
    strcpy(p_log_file->pth + len - 3, BSLOG_EXT_SECOND);
    p_log_file->file = fopen(p_log_file->pth, "a");
    if (p_log_file->file == NULL) {
      errno = BSE_LOG_INIT_A_FILE2;
      perror(p_log_file->pth);
      return;
    } else if (fprintf(p_log_file->file, initstr) < 0) {
      errno = BSE_LOG_INIT_WRITE2;
      perror(p_log_file->pth);
      return;
    }
    ofst = ftell(p_log_file->file);
    if (ofst > s_maximum_size) {
      //rewrite second:
      fclose(p_log_file->file);
      p_log_file->file = NULL;
      p_log_file->file = fopen(p_log_file->pth, "w");
      if (p_log_file->file == NULL) {
        errno = BSE_LOG_INIT_W_FILE2;
        perror(p_log_file->pth);
      } else if (fprintf(p_log_file->file, initstr) < 0) {
        errno = BSE_LOG_INIT_WRITE2;
        perror(p_log_file->pth);
      }
    }
  }
}

  //1.Setting:
/**
 * <p>Get registered debug ranges.
 * Usually for test purposes.</p>
 * @return registered debug ranges, maybe NULL
 **/
BsLogDbgRanges *bslog_get_dbgranges() {
  return s_dranges;
}

/**
 * <p>Add debug range. Non-synchronized!!!</p>
 * @param p_floor floor
 * @param p_ceiling ceiling
 **/
void bslog_add_dbgrange(int p_floor, int p_ceiling) {
  if (s_dranges == NULL) {
    s_dranges = bslogdbgranges_new(10);
    if (s_dranges == NULL) {
      return;
    }
  }
  BsLogDbgRange *drange = bslogdbgrange_new(p_floor, p_ceiling);
  if (drange == NULL) {
    return;
  }
  bslogdbgranges_add_inc(s_dranges, drange, 10);
}

/**
 * <p>Set main range debug level floor.</p>
 * @param p_levomsg - level
 **/
void bslog_set_debug_floor(int p_levomsg) {
  s_debug_level_floor = p_levomsg;
}

/**
 * <p>Get main range debug level floor.</p>
 * @return level floor
 **/
int bslog_get_debug_floor() {
  return s_debug_level_floor;
}

/**
 * <p>Set main range debug level ceiling.</p>
 * @param p_levomsg - level
 **/
void bslog_set_debug_ceiling(int p_levomsg) {
  s_debug_level_ceiling = p_levomsg;
}

/**
 * <p>Get main range debug level ceiling.</p>
 * @return debug level ceiling.
 **/
int bslog_get_debug_ceiling() {
  return s_debug_level_ceiling;
}

/**
 * <p>Set maximum log file size.</p>
 * @param pSize - size
 **/
void bslog_set_maximum_size(BS_FOFST_T pSize) {
  s_maximum_size = pSize;
}

/**
 * <p>Get maximum log file size.</p>
 * @return maximum log file size.
 **/
BS_FOFST_T bslog_get_maximum_size() {
  return s_maximum_size;
}

  //2.Initialization:
/**
 * <p>Invoked by main client constructor.</p>
 * @param pSize size of array
 * @return Log files array or NULL.
 * @set errno if error.
 **/
BsLogFiles*
  bslogfiles_new (int pSize)
{
  if ( pSize < 1 )
  {
    errno = BSE_ARR_WPSIZE;
    fprintf (stderr, "%s %s\n", __func__, bserror_to_str(errno));
    return NULL;
  }
  BsLogFiles *obj = malloc (sizeof (BsLogFiles));
  if ( obj == NULL )
  {
    if ( errno == 0 ) { errno = ENOMEM; }
    perror (__func__);
    return NULL;
  }
  obj->files = malloc (pSize * sizeof (BsLogFile*));
  if ( obj->files == NULL )
  {
    obj = bslogfiles_free(obj);
    goto out;
  }
  obj->size = pSize;
  int i;
  for ( i = 0; i < obj->size; i++ )
              { obj->files[i] = NULL; }

  for ( i = 0; i < obj->size; i++ )
  {
    obj->files[i] = malloc (sizeof(BsLogFile));
    if ( obj->files[i] == NULL )
    {
      obj = bslogfiles_free (obj);
      break;
    }
    obj->files[i]->file = NULL;
    obj->files[i]->pth = NULL;
  }
out:
  if ( obj == NULL )
  {
    if ( errno == 0 ) { errno = ENOMEM; }
    perror(__func__);
  }
  return obj;
}

/**
 * <p>Invoked by main client initializer of file path in given array's file.</p>
 * @param pLogFls - files array
 * @param pIdx - file index
 * @param p_path - file path
 * @set errno if error.
 **/
void bslog_files_set_path(BsLogFiles *pLogFls, int pIdx, char *p_path) {
  if (pIdx < 0 || pIdx >= pLogFls->size) {
    errno = BSE_ARR_OUT_OF_BOUNDS;
    fprintf(stderr, "%s %s\n", __func__, bserror_to_str(errno));
    return;
  }
  if (pLogFls == NULL || p_path == NULL
    || (pLogFls->files != NULL && pLogFls->files[pIdx] != NULL && pLogFls->files[pIdx]->pth != NULL)) {
    errno = BSE_LOG_WPPATH;
    fprintf(stderr, "%s %s\n", __func__, bserror_to_str(errno));
    return;
  }
  int pth_len = strlen(p_path) + 1;
  pLogFls->files[pIdx]->pth = malloc(pth_len * sizeof(char));
  if (pLogFls->files[pIdx]->pth == NULL) {
    if (errno == 0) {
      errno = ENOMEM;
    }
    perror("bslog_files_set_path");
    return;
  }
  strcpy(pLogFls->files[pIdx]->pth, p_path);
}

/**
 * <p>Main program initializes log files
 * and passes file's indexes to clients.</p>
 * @param pLogFls dynamically allocated and opened log files
 * @set errno if error.
 **/
void bslog_init(BsLogFiles *pLogFls) {
  if (s_log_files != NULL) {
    errno = BSE_LOG_ALREADY_INITIALIZED;
    fprintf(stderr, "%s %s\n", __func__, bserror_to_str(errno));
    return;
  }
  if (pLogFls == NULL || pLogFls->files == NULL) {
    errno = BSE_LOG_WRONG_INIT_PARAMS;
    fprintf(stderr, "%s %s\n", __func__, bserror_to_str(errno));
    return;
  }
  int i;
  for (i = 0; i < pLogFls->size; i++) {
    if (pLogFls->files[i]->pth == NULL) {
      errno = BSE_LOG_WRONG_INIT_PARAMS;
      fprintf(stderr, "%s %s\n", __func__, bserror_to_str(errno));
      return;
    }
    if (pLogFls->files[i]->file != NULL) {
      errno = BSE_LOG_WRONG_INIT_PARAMS;
      fprintf(stderr, "%s %s\n", __func__, bserror_to_str(errno));
      return;
    }
  }
  for (i = 0; i < pLogFls->size; i++) {
    bslogtst_file_init_open(pLogFls->files[i]);
    if (errno != 0) {
      return;
    }
  }
  if (pthread_mutex_init(&sMutex, NULL) != 0) {
    sMutexOk = false;
    errno = BSE_LOG_INIT_MUTEX;
    fprintf(stderr, "%s %s\n", __func__, bserror_to_str(errno));
    return;
  }
  sMutexOk = true;
  s_log_files = pLogFls;
}

/**
 * <p>Check log constant after logging initializing.</p>
 * @param pLgCnst pointer to log constant data
 * @set errno if error.
 **/
void bslogconst_check(BsLogConst *pLgCnst) {
  if (s_log_files == NULL) {
    errno = BSE_LOG_WRONG_INITIALIZED;
    fprintf(stderr, "%s %s\n", __func__, bserror_to_str(errno));
    return;
  }
  if (pLgCnst == NULL || pLgCnst->tag == NULL) {
    errno = BSE_LOG_WRONG_PARAMS;
    fprintf(stderr, "%s %s\n", __func__, bserror_to_str(errno));
    return;
  }
  if (pLgCnst->file_index >= s_log_files->size) {
    errno = BSE_LOG_WRONG_PARAMS;
    fprintf(stderr, "%s %s\n", __func__, bserror_to_str(errno));
    return;
  }
}

  //3.Destroying:

/**
 * <p>Destructor.</p>
 * @param pLogFls - files array
 * @return always NULL
 **/
BsLogFiles*
  bslogfiles_free (BsLogFiles *pLogFls)
{
  if ( pLogFls != NULL )
  {
    if ( pLogFls->files != NULL )
    {
      for (int i = 0; i < pLogFls->size; i++) {
        if (pLogFls->files[i]->file != NULL) {
          fprintf(pLogFls->files[i]->file, "BS-LOG try to close...\n");
          fclose(pLogFls->files[i]->file);
        }
        if (pLogFls->files[i]->pth != NULL) {
          free(pLogFls->files[i]->pth);
        }
        free(pLogFls->files[i]);
      }
      free(pLogFls->files);
    }
    free(pLogFls);
  }
  bslogdbgranges_free(s_dranges);
  return NULL;
}

/**
 * <p>Free memory, closing files on main program exit.</p>
 **/
void
  bslog_destroy(void)
{
  if (s_log_files != NULL)
          { s_log_files = bslogfiles_free(s_log_files);  }

  if (sMutexOk)
  {
    pthread_mutex_destroy(&sMutex);
    sMutexOk = false;
  }
}

/**
 * <p>Checker if show debug messages for given level.
 * This is for high performance and best filtering of debug messages.
 * Example:
 * <pre>
 * if (bslog_is_debug(12786) {
 *   //a lot of job to make message:
 *   ...
 *   bslog_log_to(s_lconst, BSLDEBUG, msg);
 * }
 * </pre>
 * </p>
 * @param p_levomsg int - debug level
 * @return if debug.
 **/
bool bslog_is_debug(int p_levomsg) {
  bool isDbg = p_levomsg >= s_debug_level_floor && p_levomsg <= s_debug_level_ceiling;
  if (!isDbg && s_dranges != NULL) {
    for (int l = 0; l < s_dranges->size; l++) {
      if (s_dranges->vals[l] == NULL) {
        break;
      }
      isDbg = p_levomsg >= s_dranges->vals[l]->floor && p_levomsg <= s_dranges->vals[l]->ceiling;
      if (isDbg) {
        break;
      }
    }
  }
  return isDbg;
}
