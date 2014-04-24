// ************************************************************************
// *                                                                      *
// * Copyright (C) 2014 Bonden i Nol (hakanbrolin@hotmail.com)            *
// *                                                                      *
// * This program is free software; you can redistribute it and/or modify *
// * it under the terms of the GNU General Public License as published by *
// * the Free Software Foundation; either version 2 of the License, or    *
// * (at your option) any later version.                                  *
// *                                                                      *
// ************************************************************************

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <error.h>
#include <sstream>
#include <iomanip>

#include "io_core.h"
#include "io_utility.h"

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////

#define PRODUCT_NUMBER   "TBD"
#define RSTATE           "R1A01"

#define ERROR_MUTEX_LOCK(mutex)				\
  ( __extension__ ( { if (pthread_mutex_lock(&mutex)) {	\
	return IO_ERROR_MUTEX_FAILURE;		\
      } }))

#define ERROR_MUTEX_UNLOCK(mutex)			  \
  ( __extension__ ( { if (pthread_mutex_unlock(&mutex)) { \
	return IO_ERROR_MUTEX_FAILURE;		  \
      } }))

#ifdef DEBUG_PRINTS
// 
// Notes!
// Macro 'debug_printf' can be used anywhere in LIBIO.
// The other macros can only be used in function 'update_error'.
// 
#define debug_printf(fmt, args...)  printf("LIBIO - "); \
                                    printf(fmt, ##args); \
				    fflush(stdout)

#define debug_linux_error()         printf("LIBIO LINUX ERROR - "); \
                                    error(0, errno, NULL); \
				    fflush(stdout)

#define debug_internal_error()      printf("LIBIO INTERNAL ERROR\n"); \
				    fflush(stdout)
#else
#define debug_printf(fmt, args...) 
#define debug_linux_error()
#define debug_internal_error()
#endif // DEBUG_PRINTS

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

io_core::io_core(void)
{
  m_error_source    = IO_INTERNAL_ERROR;
  m_error_code      = 0;
  m_last_error_read = true;
  pthread_mutex_init(&m_error_mutex, NULL); // Use default mutex attributes

  m_initialized = false;
  pthread_mutex_init(&m_init_mutex, NULL); // Use default mutex attributes
}

////////////////////////////////////////////////////////////////

io_core::~io_core(void)
{
  pthread_mutex_destroy(&m_error_mutex);
  pthread_mutex_destroy(&m_init_mutex);
}

////////////////////////////////////////////////////////////////

long io_core::get_last_error(IO_LIB_STATUS *status)
{
  try {
    ERROR_MUTEX_LOCK(m_error_mutex);
    status->error_source = m_error_source;
    status->error_code   = m_error_code;
    
    // Clear internal error information
    m_error_source    = IO_INTERNAL_ERROR;
    m_error_code      = IO_NO_ERROR;
    m_last_error_read = true;
    ERROR_MUTEX_UNLOCK(m_error_mutex);
    return IO_SUCCESS;
  }
  catch (...) {
    return set_error(EXP(IO_INTERNAL_ERROR, IO_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long io_core::get_error_string(long error_code, 
			       IO_ERROR_STRING error_string)
{
  try {
    // Check input values
    if (!error_string) {
      THROW_EXP(IO_INTERNAL_ERROR, IO_BAD_ARGUMENT,
		"error_string is null pointer", NULL);
    }

    // Do the actual work
    return internal_get_error_string(error_code, error_string);
  }
  catch (io_exception &ixp) {
    return set_error(ixp);
  }
  catch (...) {
    return set_error(EXP(IO_INTERNAL_ERROR, IO_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long io_core::init(void)
{
  try {
    if (io_do_mutex_lock(&m_init_mutex) != IO_SUCCESS) {
      THROW_EXP(IO_LINUX_ERROR, IO_MUTEX_LOCK_FAILED,
		"Mutex lock failed", NULL);
    }

    // Check if already initialized
    if (m_initialized) {
       THROW_EXP(IO_INTERNAL_ERROR, IO_ALREADY_INITIALIZED,
		 "Already initialized", NULL);
    }

    // Do the actual initialization
    internal_init();

    // Initialization completed    
    m_initialized = true;

    if (io_do_mutex_unlock(&m_init_mutex) != IO_SUCCESS) {
      THROW_EXP(IO_LINUX_ERROR, IO_MUTEX_UNLOCK_FAILED,
		"Mutex unlock failed", NULL);
    }

    return IO_SUCCESS;
  }
  catch (io_exception &ixp) {
    io_do_mutex_unlock(&m_init_mutex);
    return set_error(ixp);
  }
  catch (...) {
    io_do_mutex_unlock(&m_init_mutex);
    return set_error(EXP(IO_INTERNAL_ERROR, IO_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long io_core::finalize(void)
{
  try {
    if (io_do_mutex_lock(&m_init_mutex) != IO_SUCCESS) {
      THROW_EXP(IO_LINUX_ERROR, IO_MUTEX_LOCK_FAILED,
		"Mutex lock failed", NULL);
    }

    // Check if not initialized
    if (!m_initialized) {
      THROW_EXP(IO_INTERNAL_ERROR, IO_NOT_INITIALIZED,
		"Not initialized", NULL);
    }   

    // Do the actual finalization
    internal_finalize();

    // Finalization completed   
    m_initialized = false;

    if (io_do_mutex_unlock(&m_init_mutex) != IO_SUCCESS) {
      THROW_EXP(IO_LINUX_ERROR, IO_MUTEX_UNLOCK_FAILED,
		"Mutex unlock failed", NULL);
    }

    return IO_SUCCESS;
  }
  catch (io_exception &ixp) {
    io_do_mutex_unlock(&m_init_mutex);
    return set_error(ixp);
  }
  catch (...) {
    io_do_mutex_unlock(&m_init_mutex);
    return set_error(EXP(IO_INTERNAL_ERROR, IO_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long io_core::reset_dev1(void)
{
  try {
    // Check if not initialized
    if (!m_initialized) {
      THROW_EXP(IO_INTERNAL_ERROR, IO_NOT_INITIALIZED,
                "Not initialized", NULL);
    }
    
    // Do the actual work
    m_io_gpio_auto->reset_dev1();    

    return IO_SUCCESS;
  }
  catch (io_exception &ixp) {
    return set_error(ixp);
  }
  catch (...) {
    return set_error(EXP(IO_INTERNAL_ERROR, IO_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long io_core::reset_dev2(void)
{
  try {
    // Check if not initialized
    if (!m_initialized) {
      THROW_EXP(IO_INTERNAL_ERROR, IO_NOT_INITIALIZED,
                "Not initialized", NULL);
    }
    
    // Do the actual work
    m_io_gpio_auto->reset_dev2();

    return IO_SUCCESS;
  }
  catch (io_exception &ixp) {
    return set_error(ixp);
  }
  catch (...) {
    return set_error(EXP(IO_INTERNAL_ERROR, IO_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long io_core::test_get_libio_prod_info(IO_LIBIO_PROD_INFO *prod_info)
{
  try {
    // Check input values
    if (!prod_info) {
      THROW_EXP(IO_INTERNAL_ERROR, IO_BAD_ARGUMENT,
		"prod_info is null pointer", NULL);
    }

    // Do the actual work
    return internal_test_get_libio_prod_info(prod_info);
  }
  catch (io_exception &ixp) {
    return set_error(ixp);
  }
  catch (...) {
    return set_error(EXP(IO_INTERNAL_ERROR, IO_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

/////////////////////////////////////////////////////////////////////////////
//               Private member functions
/////////////////////////////////////////////////////////////////////////////

long io_core::set_error(io_exception ixp)
{
#ifdef DEBUG_PRINTS

  // Get the stack trace
  STACK_FRAMES frames;
  ixp.get_stack_frames(frames);

  ostringstream oss_msg;
  char buffer[18];

  oss_msg << "stack frames:" << (int) frames.active_frames << "\n";

  for (unsigned i=0; i < frames.active_frames; i++) {
    sprintf(buffer, "0x%016lx", frames.frames[i]);
    oss_msg << "\tframe:" << dec << setw(2) << setfill('0') << i
	    << "  addr:" << buffer << "\n";
  }

  // Get identification
  oss_msg << "\tPid: " << ixp.get_process_id() 
	  << ", Tid: " << ixp.get_thread_id() << "\n";

  // Get info from predefined macros
  oss_msg << "\tViolator: " << ixp.get_file() 
	  << ":" << ixp.get_line()
	  << ", " << ixp.get_function() << "\n";

  // Get the internal info
  oss_msg << "\tSource: " << ixp.get_source()
	  << ", Code: " << ixp.get_code() << "\n";

  oss_msg << "\tInfo: " << ixp.get_info() << "\n";

  // Print all info
  debug_printf(oss_msg.str().c_str(), NULL);

#endif // DEBUG_PRINTS

  // Update internal error information
  return update_error(ixp);
}

////////////////////////////////////////////////////////////////

long io_core::update_error(io_exception ixp)
{
  ERROR_MUTEX_LOCK(m_error_mutex);
  if (m_last_error_read) {
    m_error_source    = ixp.get_source();
    m_error_code      = ixp.get_code();
    m_last_error_read = false; /* Latch last error until read */
  }
  ERROR_MUTEX_UNLOCK(m_error_mutex);

#ifdef DEBUG_PRINTS 
  switch(ixp.get_source()) {
  case IO_INTERNAL_ERROR:
    debug_internal_error();
    break;
  case IO_LINUX_ERROR:
    debug_linux_error();
    break;
  }
#endif

  return IO_FAILURE;
}

////////////////////////////////////////////////////////////////

long io_core::internal_get_error_string(long error_code,
					IO_ERROR_STRING error_string)
{
  size_t str_len = sizeof(IO_ERROR_STRING);

  switch (error_code) {
  case IO_NO_ERROR:
    strncpy(error_string, "No error", str_len);
    break;
  case IO_NOT_INITIALIZED:
    strncpy(error_string, "Not initialized", str_len);
    break;
  case IO_ALREADY_INITIALIZED:
    strncpy(error_string, "Already initialized", str_len);
    break;
  case IO_BAD_ARGUMENT:
    strncpy(error_string, "Bad argument", str_len);
    break;
  case IO_MUTEX_LOCK_FAILED:
    strncpy(error_string, "Mutex lock failed", str_len);
    break;
  case IO_MUTEX_UNLOCK_FAILED:
    strncpy(error_string, "Mutex unlock failed", str_len);
    break;
  case IO_TIME_ERROR:
    strncpy(error_string, "Time operation failed", str_len);
    break;
  case IO_SEM_OPERATION_FAILED:
    strncpy(error_string, "Semaphore operation failed", str_len);
    break;
  case IO_UNEXPECTED_EXCEPTION:
    strncpy(error_string, "Unexpected exception", str_len);
    break;
  default: 
    strncpy(error_string, "Undefined error", str_len);
  }

  return IO_SUCCESS;
}

////////////////////////////////////////////////////////////////

void io_core::internal_init(void)
{
  // Create the GPIO object with garbage collector
  m_io_gpio_auto = auto_ptr<io_gpio>(new io_gpio());
  m_io_gpio_auto->init();
}

////////////////////////////////////////////////////////////////

void io_core::internal_finalize(void)
{
  m_io_gpio_auto->finalize();

  IO_LIB_STATUS status; 
  get_last_error(&status);  
}

////////////////////////////////////////////////////////////////

long io_core::internal_test_get_libio_prod_info(IO_LIBIO_PROD_INFO *prod_info)
{
  strncpy(prod_info->prod_num, 
	  PRODUCT_NUMBER, 
	  sizeof(((IO_LIBIO_PROD_INFO *)0)->prod_num));

  strncpy(prod_info->rstate, 
	  RSTATE, 
	  sizeof(((IO_LIBIO_PROD_INFO *)0)->rstate));

  return IO_SUCCESS;
}
