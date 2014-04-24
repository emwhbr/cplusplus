/************************************************************************
 *                                                                      *
 * Copyright (C) 2014 Bonden i Nol (hakanbrolin@hotmail.com)            *
 *                                                                      *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 ************************************************************************/

#ifndef __IO_H__
#define __IO_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * LIBIO return codes
 */
#define IO_SUCCESS               0
#define IO_FAILURE              -1
#define IO_ERROR_MUTEX_FAILURE  -2

/*
 * LIBIO Internal error codes
 */
#define IO_NO_ERROR                      0
#define IO_NOT_INITIALIZED               1
#define IO_ALREADY_INITIALIZED           2
#define IO_BAD_ARGUMENT                  3
#define IO_MUTEX_LOCK_FAILED             4
#define IO_MUTEX_UNLOCK_FAILED           5
#define IO_TIME_ERROR                    6
#define IO_SEM_OPERATION_FAILED          7
#define IO_UNEXPECTED_EXCEPTION          8

/*
 * Error source values
 */
typedef enum {IO_INTERNAL_ERROR, 
	      IO_LINUX_ERROR} IO_ERROR_SOURCE;
/*
 * Basic API support types
 */

/*
 * API types
 */

typedef char IO_ERROR_STRING[256];

typedef struct {
  char prod_num[20];
  char rstate[10];
} IO_LIBIO_PROD_INFO;

typedef struct {
  IO_ERROR_SOURCE error_source;
  long            error_code;
} IO_LIB_STATUS;

/****************************************************************************
*
* Name io_get_last_error
*
* Description Returns the error information held by LIBIO, when a LIBIO 
*             call returns unsuccessful completion. 
*             LIBIO clears its internal error information after it has been 
*             read by the calling application.
*
* Parameters status  IN/OUT  pointer to a buffer to hold the error information
*
* Error handling Returns IO_SUCCESS if successful
*                otherwise IO_FAILURE or IO_ERROR_MUTEX_FAILURE
*
****************************************************************************/
 extern long io_get_last_error(IO_LIB_STATUS *status);

/****************************************************************************
*
* Name io_get_error_string
*
* Description Returns the error string corresponding to the provided
*             internal error code.
*
* Parameters error_code    IN      actual error code
*            error_string  IN/OUT  pointer to a buffer to hold the error string
*
* Error handling Returns always IO_SUCCESS
*
****************************************************************************/
extern long io_get_error_string(long error_code, 
				IO_ERROR_STRING error_string);

/****************************************************************************
*
* Name io_initialize
*
* Description Allocates necessary cluster wide resources.
*             This function shall be called once to make LIBIO operational.
*             This function can be called again after finalization.
*
* Parameters None
*
* Error handling Returns IO_SUCCESS if successful
*                otherwise IO_FAILURE or IO_ERROR_MUTEX_FAILURE
*
****************************************************************************/
extern long io_initialize(void);

/****************************************************************************
*
* Name io_finalize
*
* Description Deallocates cluster wide resources created during initialization.
*
* Parameters None 
*
* Error handling Returns IO_SUCCESS if successful
*                otherwise IO_FAILURE or IO_ERROR_MUTEX_FAILURE
*
****************************************************************************/
extern long io_finalize(void);

/****************************************************************************
*
* Name io_reset_dev1
*
* Description Resets the DEV1 board.
*
* Parameters None
*
* Error handling Returns IO_SUCCESS if successful
*                otherwise IO_FAILURE or IO_ERROR_MUTEX_FAILURE
*
****************************************************************************/
extern long io_reset_dev1(void);

/****************************************************************************
*
* Name io_reset_dev2
*
* Description Resets the DEV2 board.
*
* Parameters None
*
* Error handling Returns IO_SUCCESS if successful
*                otherwise IO_FAILURE or IO_ERROR_MUTEX_FAILURE
*
****************************************************************************/
extern long io_reset_dev2(void);

/****************************************************************************
*
* Name io_test_get_libio_prod_info
*
* Description Returns the product number and the RState of LIBIO.
*
* Parameters prod_info  IN/OUT  pointer to a buffer to hold the product number
*                               and the RState.
*
* Error handling Returns always IO_SUCCESS.
*
****************************************************************************/
extern long io_test_get_libio_prod_info(IO_LIBIO_PROD_INFO *prod_info);

#ifdef  __cplusplus
}
#endif

#endif /* __IO_H__ */
