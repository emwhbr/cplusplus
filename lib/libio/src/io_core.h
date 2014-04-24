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

#ifndef __IO_CORE_H__
#define __IO_CORE_H__

#include <memory>

#include "io_exception.h"
#include "io_gpio.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class io_core {

public:
  io_core(void);
  ~io_core(void);

  long get_last_error(IO_LIB_STATUS *status);

  long get_error_string(long error_code,
			IO_ERROR_STRING error_string);

  long init(void);

  long finalize(void);

  long reset_dev1(void);

  long reset_dev2(void);

  long test_get_libio_prod_info(IO_LIBIO_PROD_INFO *prod_info);

private:
  // Error handling information
  IO_ERROR_SOURCE  m_error_source;
  long             m_error_code;
  bool             m_last_error_read;
  pthread_mutex_t  m_error_mutex;

  // Keep track of initialization
  bool             m_initialized;
  pthread_mutex_t  m_init_mutex;

  // GPIO
  auto_ptr<io_gpio> m_io_gpio_auto;

  // Private member functions
  long set_error(io_exception ixp);

  long update_error(io_exception ixp);

  long internal_get_error_string(long error_code,
				 IO_ERROR_STRING error_string);

  void internal_init(void);

  void internal_finalize(void);

  long internal_test_get_libio_prod_info(IO_LIBIO_PROD_INFO *prod_info);
};

#endif // __IO_CORE_H__
