/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ errors codes and string mapping.</p>
 * @author Yury Demidenko
 **/

#include "string.h"
#include "BsError.h"

char *bserror_to_str(int pCode) {
  if (pCode < 10000) {
    return strerror(pCode);
  }
  if (pCode == BSE_ERR) {
    return "Generic error";
  }
  if (pCode == BSE_ALG_ERR) {
    return "Algorithm error";
  }
  if (pCode == BSE_VALIDATE_ERR) {
    return "Validation data error";
  }
  if (pCode == BSE_UNIMPLEMENTED) {
    return "This use case is not implemented";
  }
  if (pCode == BSE_WRITE_FILE) {
    return "Fail fwrite to file";
  }
  if (pCode == BSE_TEST_ERR) {
    return "Test error";
  }
  if (pCode ==  BSE_READ_FILE) {
    return "Fail read from file";
  }
  if (pCode ==  BSE_WRONG_FDATA) {
    return "Wrong file data";
  }
  if (pCode == BSE_OPEN_FILE) {
    return "Fail fopen file";
  }
  if (pCode == BSE_SEEK_FILE) {
    return "Fail fseek file";
  }
  if (pCode == BSE_OUT_BUFFER_SIZE) {
    return "Out of buffer size";
  }
  if (pCode == BSE_INTEGER_OVERFLOW) {
    return "Integer casting overflow, e.g. int=257->char";
  }
  if (pCode == BSE_WRONG_PARAMS) {
    return "Passed parameters are wrong";
  }
  if (pCode == BSE_ARR_OUT_OF_BOUNDS) {
    return "Out of bounds";
  }
  if (pCode == BSE_ARR_SET_NULL) {
    return "Try to add/set NULL object";
  }
  if (pCode == BSE_ARR_OUT_MAX_SIZE) {
    return "Out of maximum size";
  }
  if (pCode == BSE_ARR_WPSIZE) {
    return "Parameter size <= 0";
  }
  if (pCode == BSE_ARR_WPINCSIZE) {
    return "Parameter increase size <= 0";
  }
  if (pCode == BSE_LOG_FATAL_OPEN_W_FILE) {
    return "BSLOG-FATAL can't open file to rewrite";
  }
  if (pCode == BSE_LOG_INIT_A_FILE) {
    return "BSLOG-INIT can't open file#1 to append";
  }
  if (pCode == BSE_LOG_INIT_A_FILE2) {
    return "BSLOG-INIT can't open file#2 to append";
  }
  if (pCode == BSE_LOG_INIT_W_FILE2) {
    return "BSLOG-INIT can't open file#2 to rewrite";
  }
  if (pCode == BSE_LOG_INIT_WRITE) {
    return "BSLOG-INIT can't write file#1";
  }
  if (pCode == BSE_LOG_INIT_WRITE2) {
    return "BSLOG-INIT can't write file#2";
  }
  if (pCode == BSE_LOG_WPPATH) {
    return "BSLOG wrong parameters for path";
  }
  if (pCode == BSE_LOG_ALREADY_INITIALIZED) {
    return "BSLOG already initialized";
  }
  if (pCode == BSE_LOG_WRONG_INIT_PARAMS) {
    return "BSLOG wrong init params";
  }
  if (pCode == BSE_LOG_WRONG_PARAMS) {
    return "BSLOG wrong log param";
  }
  if (pCode == BSE_LOG_WRONG_INITIALIZED) {
    return "BSLOG wrong initialized";
  }
  if (pCode == BSE_LOG_INIT_MUTEX) {
    return "BSLOG error init mutex";
  }
  return "UNKNOWN ERROR";
}
