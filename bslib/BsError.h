/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */

/**
 * <p>Beigesoft™ errors codes and string mapping.</p>
 * @author Yury Demidenko
 **/
#ifndef BSE_ARR_OUT_OF_BOUNDS

//Generic errors range: 10000-10799
#define BSE_ERR 10000 //Generic error, e.g. for tests
#define BSE_WRONG_PARAMS 10001 //Passed parameters are wrong
#define BSE_ALG_ERR 10002 //Algorithm error
#define BSE_VALIDATE_ERR 10003 //Validation data error
#define BSE_UNIMPLEMENTED 10004 //This use case is not implemented
#define BSE_WRITE_FILE 10005 //Fail fwrite to file
#define BSE_READ_FILE 10006 //Fail read from file
#define BSE_OPEN_FILE 10007 //Fail fopen file
#define BSE_SEEK_FILE 10008 //Fail fseek file
#define BSE_OUT_BUFFER_SIZE 10009 //Out of buffer size
#define BSE_INTEGER_OVERFLOW 10010 //Integer casting overflow, e.g. int=257->char
#define BSE_WRONG_FDATA 10011 //Wrong file data
//Test errors 10800-10899
#define BSE_TEST_ERR 10800 //Test error
//Logger range 10900-10999:
#define BSE_LOG_WRONG_INITIALIZED 10900 
#define BSE_LOG_ALREADY_INITIALIZED 10901
#define BSE_LOG_WRONG_INIT_PARAMS 10902
#define BSE_LOG_INIT_WRITE 10903
#define BSE_LOG_INIT_WRITE2 10904
#define BSE_LOG_INIT_A_FILE 10905
#define BSE_LOG_INIT_A_FILE2 10906
#define BSE_LOG_INIT_W_FILE2 10907
#define BSE_LOG_FATAL_OPEN_W_FILE 10908
#define BSE_LOG_WPPATH 10909
#define BSE_LOG_INIT_MUTEX 10910
#define BSE_LOG_WRONG_PARAMS 10911
//Arrays range 11000-11499
#define BSE_ARR_OUT_OF_BOUNDS 11000
#define BSE_ARR_SET_NULL 11001 //Try to add/set NULL object
#define BSE_ARR_OUT_MAX_SIZE 11002 //Out of maximum size
#define BSE_ARR_WPINCSIZE 11003 //Parameter increase size <= 0
#define BSE_ARR_WPSIZE 11004 //Parameter size <= 0

char *bserror_to_str(int pCode);
#endif
