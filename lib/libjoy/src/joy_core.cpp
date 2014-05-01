// ************************************************************************
// *                                                                      *
// * Copyright (C) 2014 Bonden i Nol (hakanbrolin@hotmail.com)            *
// *                                                                      *
// * This program is free software; you can redistribute it and/or modify *
// * it under the terms of thE GNU General Public License as published by *
// * the Free Software Foundation; either version 2 of the License, or    *
// * (at your option) any later version.                                  *
// *                                                                      *
// ************************************************************************

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <error.h>
#include <math.h>
#include <sstream>
#include <iomanip>

#include "joy_core.h"
#include "joy_hw.h"
#include "joy_utility.h"

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////

#define PRODUCT_NUMBER   "LIBJOY"
#define RSTATE           "R1A01"

#define ERROR_MUTEX_LOCK(mutex)				\
  ( __extension__ ( { if (pthread_mutex_lock(&mutex)) {	\
	return JOY_ERROR_MUTEX_FAILURE;		\
      } }))

#define ERROR_MUTEX_UNLOCK(mutex)			  \
  ( __extension__ ( { if (pthread_mutex_unlock(&mutex)) { \
	return JOY_ERROR_MUTEX_FAILURE;		  \
      } }))

#ifdef DEBUG_PRINTS
// 
// Notes!
// Macro 'debug_printf' can be used anywhere in LIBJOY.
// The other macros can only be used in function 'update_error'.
// 
#define debug_printf(fmt, args...)  printf("LIBJOY - "); \
                                    printf(fmt, ##args); \
				    fflush(stdout)

#define debug_linux_error()         printf("LIBJOY LINUX ERROR - "); \
                                    error(0, errno, NULL); \
				    fflush(stdout)

#define debug_internal_error()      printf("LIBJOY INTERNAL ERROR\n"); \
				    fflush(stdout)
#else
#define debug_printf(fmt, args...) 
#define debug_linux_error()
#define debug_internal_error()
#endif // DEBUG_PRINTS

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

joy_core::joy_core(void)
{
  m_error_source    = JOY_INTERNAL_ERROR;
  m_error_code      = 0;
  m_last_error_read = true;
  pthread_mutex_init(&m_error_mutex, NULL); // Use default mutex attributes

  // Initialize joystick device information
  m_joy_dev_info[JOY_DEV_0].initialized = false;
  pthread_mutex_init(&m_joy_dev_info[JOY_DEV_0].init_mutex,
		     NULL); // Use default mutex attributes
  m_joy_dev_info[JOY_DEV_0].device = JOY_DEV_0_FILE;
  m_joy_dev_info[JOY_DEV_0].joy_io_auto.reset();

  m_joy_dev_info[JOY_DEV_1].initialized = false;
  pthread_mutex_init(&m_joy_dev_info[JOY_DEV_1].init_mutex,
		     NULL); // Use default mutex attributes
  m_joy_dev_info[JOY_DEV_1].device = JOY_DEV_1_FILE;
  m_joy_dev_info[JOY_DEV_1].joy_io_auto.reset();
}

////////////////////////////////////////////////////////////////

joy_core::~joy_core(void)
{
  pthread_mutex_destroy(&m_error_mutex);
  pthread_mutex_destroy(&m_joy_dev_info[JOY_DEV_0].init_mutex);
  pthread_mutex_destroy(&m_joy_dev_info[JOY_DEV_1].init_mutex);
}

////////////////////////////////////////////////////////////////

long joy_core::get_last_error(JOY_LIB_STATUS *status)
{
  try {
    ERROR_MUTEX_LOCK(m_error_mutex);
    status->error_source = m_error_source;
    status->error_code   = m_error_code;
    
    // Clear internal error information
    m_error_source    = JOY_INTERNAL_ERROR;
    m_error_code      = JOY_NO_ERROR;
    m_last_error_read = true;
    ERROR_MUTEX_UNLOCK(m_error_mutex);
    return JOY_SUCCESS;
  }
  catch (...) {
    return set_error(EXP(JOY_INTERNAL_ERROR, JOY_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long joy_core::get_error_string(long error_code, 
				JOY_ERROR_STRING error_string)
{
  try {
    // Check input values
    if (!error_string) {
      THROW_EXP(JOY_INTERNAL_ERROR, JOY_BAD_ARGUMENT,
		"error_string is null pointer", NULL);
    }

    // Do the actual work
    return internal_get_error_string(error_code, error_string);
  }
  catch (joy_exception &exp) {
    return set_error(exp);
  }
  catch (...) {
    return set_error(EXP(JOY_INTERNAL_ERROR, JOY_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long joy_core::init(JOY_DEV dev,
		    bool non_block)
{
  try {
    if (joy_do_mutex_lock(&m_joy_dev_info[dev].init_mutex) != JOY_SUCCESS) {
      THROW_EXP(JOY_LINUX_ERROR, JOY_MUTEX_LOCK_FAILED,
		"Mutex lock failed", NULL);
    }

    // Check if already initialized
    if (m_joy_dev_info[dev].initialized) {
       THROW_EXP(JOY_INTERNAL_ERROR, JOY_ALREADY_INITIALIZED,
		 "Already initialized, dev=%d", dev);
    }

    // Do the actual initialization
    internal_init(dev, non_block);

    // Initialization completed    
    m_joy_dev_info[dev].initialized = true;

    if (joy_do_mutex_unlock(&m_joy_dev_info[dev].init_mutex) != JOY_SUCCESS) {
      THROW_EXP(JOY_LINUX_ERROR, JOY_MUTEX_UNLOCK_FAILED,
		"Mutex unlock failed", NULL);
    }

    return JOY_SUCCESS;
  }
  catch (joy_exception &exp) {
    joy_do_mutex_unlock(&m_joy_dev_info[dev].init_mutex);
    return set_error(exp);
  }
  catch (...) {
    joy_do_mutex_unlock(&m_joy_dev_info[dev].init_mutex);
    return set_error(EXP(JOY_INTERNAL_ERROR, JOY_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long joy_core::finalize(JOY_DEV dev)
{
  try {
    if (joy_do_mutex_lock(&m_joy_dev_info[dev].init_mutex) != JOY_SUCCESS) {
      THROW_EXP(JOY_LINUX_ERROR, JOY_MUTEX_LOCK_FAILED,
		"Mutex lock failed", NULL);
    }

    // Check if not initialized
    if (!m_joy_dev_info[dev].initialized) {
      THROW_EXP(JOY_INTERNAL_ERROR, JOY_NOT_INITIALIZED,
		"Not initialized", NULL);
    }   

    // Do the actual finalization
    internal_finalize(dev);

    // Finalization completed   
    m_joy_dev_info[dev].initialized = false;

    if (joy_do_mutex_unlock(&m_joy_dev_info[dev].init_mutex) != JOY_SUCCESS) {
      THROW_EXP(JOY_LINUX_ERROR, JOY_MUTEX_UNLOCK_FAILED,
		"Mutex unlock failed", NULL);
    }

    return JOY_SUCCESS;
  }
  catch (joy_exception &exp) {
    joy_do_mutex_unlock(&m_joy_dev_info[dev].init_mutex);
    return set_error(exp);
  }
  catch (...) {
    joy_do_mutex_unlock(&m_joy_dev_info[dev].init_mutex);
    return set_error(EXP(JOY_INTERNAL_ERROR, JOY_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long joy_core::get_name(JOY_DEV dev,
			char *name,
			unsigned len)
{
  try {
    // Check if not initialized
    if (!m_joy_dev_info[dev].initialized) {
      THROW_EXP(JOY_INTERNAL_ERROR, JOY_NOT_INITIALIZED,
                "Not initialized", NULL);
    }
    
    // Check parameters
    if (!name) {
      THROW_EXP(JOY_INTERNAL_ERROR, JOY_BAD_ARGUMENT,
                "name is null pointer", NULL);
    }

    if (len < 1) {
      THROW_EXP(JOY_INTERNAL_ERROR, JOY_BAD_ARGUMENT,
                "len is not valid", NULL);
    }

    // Do the actual work
    return m_joy_dev_info[dev].joy_io_auto->get_name(name, len);    
  }
  catch (joy_exception &exp) {
    return set_error(exp);
  }
  catch (...) {
    return set_error(EXP(JOY_INTERNAL_ERROR, JOY_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long joy_core::get_axis(JOY_DEV dev,
			int *value)
{
  try {
    // Check if not initialized
    if (!m_joy_dev_info[dev].initialized) {
      THROW_EXP(JOY_INTERNAL_ERROR, JOY_NOT_INITIALIZED,
                "Not initialized", NULL);
    }
    
    // Check parameters
    if (!value) {
      THROW_EXP(JOY_INTERNAL_ERROR, JOY_BAD_ARGUMENT,
                "value is null pointer", NULL);
    }

    // Do the actual work
    return m_joy_dev_info[dev].joy_io_auto->get_axis(value);    
  }
  catch (joy_exception &exp) {
    return set_error(exp);
  }
  catch (...) {
    return set_error(EXP(JOY_INTERNAL_ERROR, JOY_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long joy_core::get_buttons(JOY_DEV dev,
			   int *value)
{
  try {
    // Check if not initialized
    if (!m_joy_dev_info[dev].initialized) {
      THROW_EXP(JOY_INTERNAL_ERROR, JOY_NOT_INITIALIZED,
                "Not initialized", NULL);
    }
    
    // Check parameters
    if (!value) {
      THROW_EXP(JOY_INTERNAL_ERROR, JOY_BAD_ARGUMENT,
                "value is null pointer", NULL);
    }

    // Do the actual work
    return m_joy_dev_info[dev].joy_io_auto->get_buttons(value);    
  }
  catch (joy_exception &exp) {
    return set_error(exp);
  }
  catch (...) {
    return set_error(EXP(JOY_INTERNAL_ERROR, JOY_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long joy_core::get_event(JOY_DEV dev,
			 JOY_EVENT *event)
{
  try {
    // Check if not initialized
    if (!m_joy_dev_info[dev].initialized) {
      THROW_EXP(JOY_INTERNAL_ERROR, JOY_NOT_INITIALIZED,
                "Not initialized", NULL);
    }
    
    // Check parameters
    if (!event) {
      THROW_EXP(JOY_INTERNAL_ERROR, JOY_BAD_ARGUMENT,
                "event is null pointer", NULL);
    }

    // Do the actual work
    return m_joy_dev_info[dev].joy_io_auto->get_event(event);    
  }
  catch (joy_exception &exp) {
    return set_error(exp);
  }
  catch (...) {
    return set_error(EXP(JOY_INTERNAL_ERROR, JOY_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long joy_core::get_pos_unit_circle(int16_t x_value,
				   int16_t y_value,
				   JOY_POS_UNIT_CIRCLE *pos)
{
  try {
    // Check input values
    if (!pos) {
      THROW_EXP(JOY_INTERNAL_ERROR, JOY_BAD_ARGUMENT,
		"pos is null pointer", NULL);
    }

    // Do the actual work
    return internal_get_pos_unit_circle(x_value,
					y_value,
					pos);
  }
  catch (joy_exception &exp) {
    return set_error(exp);
  }
  catch (...) {
    return set_error(EXP(JOY_INTERNAL_ERROR, JOY_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

////////////////////////////////////////////////////////////////

long joy_core::test_get_libjoy_prod_info(JOY_LIBJOY_PROD_INFO *prod_info)
{
  try {
    // Check input values
    if (!prod_info) {
      THROW_EXP(JOY_INTERNAL_ERROR, JOY_BAD_ARGUMENT,
		"prod_info is null pointer", NULL);
    }

    // Do the actual work
    return internal_test_get_libjoy_prod_info(prod_info);
  }
  catch (joy_exception &exp) {
    return set_error(exp);
  }
  catch (...) {
    return set_error(EXP(JOY_INTERNAL_ERROR, JOY_UNEXPECTED_EXCEPTION, NULL, NULL));
  }
}

/////////////////////////////////////////////////////////////////////////////
//               Private member functions
/////////////////////////////////////////////////////////////////////////////

long joy_core::set_error(joy_exception exp)
{
#ifdef DEBUG_PRINTS

  // Get the stack trace
  STACK_FRAMES frames;
  exp.get_stack_frames(frames);

  ostringstream oss_msg;
  char buffer[18];

  oss_msg << "stack frames:" << (int) frames.active_frames << "\n";

  for (unsigned i=0; i < frames.active_frames; i++) {
    sprintf(buffer, "0x%016lx", frames.frames[i]);
    oss_msg << "\tframe:" << dec << setw(2) << setfill('0') << i
	    << "  addr:" << buffer << "\n";
  }

  // Get identification
  oss_msg << "\tPid: " << exp.get_process_id() 
	  << ", Tid: " << exp.get_thread_id() << "\n";

  // Get info from predefined macros
  oss_msg << "\tViolator: " << exp.get_file() 
	  << ":" << exp.get_line()
	  << ", " << exp.get_function() << "\n";

  // Get the internal info
  oss_msg << "\tSource: " << exp.get_source()
	  << ", Code: " << exp.get_code() << "\n";

  oss_msg << "\tInfo: " << exp.get_info() << "\n";

  // Print all info
  debug_printf(oss_msg.str().c_str(), NULL);

#endif // DEBUG_PRINTS

  // Update internal error information
  return update_error(exp);
}

////////////////////////////////////////////////////////////////

long joy_core::update_error(joy_exception exp)
{
  ERROR_MUTEX_LOCK(m_error_mutex);
  if (m_last_error_read) {
    m_error_source    = exp.get_source();
    m_error_code      = exp.get_code();
    m_last_error_read = false; /* Latch last error until read */
  }
  ERROR_MUTEX_UNLOCK(m_error_mutex);

#ifdef DEBUG_PRINTS 
  switch(exp.get_source()) {
  case JOY_INTERNAL_ERROR:
    debug_internal_error();
    break;
  case JOY_LINUX_ERROR:
    debug_linux_error();
    break;
  }
#endif

  return JOY_FAILURE;
}

////////////////////////////////////////////////////////////////

long joy_core::internal_get_error_string(long error_code,
					 JOY_ERROR_STRING error_string)
{
  size_t str_len = sizeof(JOY_ERROR_STRING);

  switch (error_code) {
  case JOY_NO_ERROR:
    strncpy(error_string, "No error", str_len);
    break;
  case JOY_NOT_INITIALIZED:
    strncpy(error_string, "Not initialized", str_len);
    break;
  case JOY_ALREADY_INITIALIZED:
    strncpy(error_string, "Already initialized", str_len);
    break;
  case JOY_BAD_ARGUMENT:
    strncpy(error_string, "Bad argument", str_len);
    break;
  case JOY_MUTEX_LOCK_FAILED:
    strncpy(error_string, "Mutex lock failed", str_len);
    break;
  case JOY_MUTEX_UNLOCK_FAILED:
    strncpy(error_string, "Mutex unlock failed", str_len);
    break;
  case JOY_FILE_OPERATION_FAILED:
    strncpy(error_string, "File operation failed", str_len);
    break;
  case JOY_DEVICE_OPERATION_FAILED:
    strncpy(error_string, "Device operation failed", str_len);
    break;
  case JOY_WRONG_EVENT_SIZE:
    strncpy(error_string, "Wrong event size", str_len);
    break;
  case JOY_UNEXPECTED_EXCEPTION:
    strncpy(error_string, "Unexpected exception", str_len);
    break;
  default: 
    strncpy(error_string, "Undefined error", str_len);
  }

  return JOY_SUCCESS;
}

////////////////////////////////////////////////////////////////

void joy_core::internal_init(JOY_DEV dev,
			     bool non_block)
{
  // Create the joystick I/O object with garbage collector
  joy_io *io_ptr = new joy_io(m_joy_dev_info[dev].device);
  m_joy_dev_info[dev].joy_io_auto = auto_ptr<joy_io>(io_ptr);

  // Initialize I/O object
  m_joy_dev_info[dev].joy_io_auto->init(non_block);
}

////////////////////////////////////////////////////////////////

void joy_core::internal_finalize(JOY_DEV dev)
{
   // Finalize the I/O object
  m_joy_dev_info[dev].joy_io_auto->finalize();

  JOY_LIB_STATUS status; 
  get_last_error(&status);  
}

////////////////////////////////////////////////////////////////

long joy_core::internal_get_pos_unit_circle(int16_t x_value,
					    int16_t y_value,
					    JOY_POS_UNIT_CIRCLE *pos)
{
  const float x0 = x_value / 32767.0f;
  const float y0 = -y_value / 32767.0f;

  float x  = x0 * sqrt(1 - pow(y0, 2)/2.0f);
  float y  = y0 * sqrt(1 - pow(x0, 2)/2.0f);
 
  pos->x = x0;
  pos->y = y0;
         
  pos->theta = atan2(y, x);
  pos->r = sqrt(pow(y, 2) + pow(x, 2));

  return JOY_SUCCESS;
}

////////////////////////////////////////////////////////////////

long joy_core::internal_test_get_libjoy_prod_info(JOY_LIBJOY_PROD_INFO *prod_info)
{
  strncpy(prod_info->prod_num, 
	  PRODUCT_NUMBER, 
	  sizeof(((JOY_LIBJOY_PROD_INFO *)0)->prod_num));

  strncpy(prod_info->rstate, 
	  RSTATE, 
	  sizeof(((JOY_LIBJOY_PROD_INFO *)0)->rstate));

  return JOY_SUCCESS;
}
