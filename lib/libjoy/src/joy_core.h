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

#ifndef __JOY_CORE_H__
#define __JOY_CORE_H__

#include <memory>

#include "joy.h"
#include "joy_exception.h"
#include "joy_io.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definitions of macros
/////////////////////////////////////////////////////////////////////////////
#define NR_JOY_DEVICES  2   // We support two joysticks in the system:
                            //  /dev/input/js0  (JOY_DEV_0)
                            //  /dev/input/js1  (JOY_DEV_1)

/////////////////////////////////////////////////////////////////////////////
//               Class support types
/////////////////////////////////////////////////////////////////////////////
typedef struct {
  bool             initialized;  // Keep track of initialization
  pthread_mutex_t  init_mutex;   // Keep track of initialization
  string           device;       // Joystick device file
  auto_ptr<joy_io> joy_io_auto;  // Joystick I/O object
} JOY_DEV_INFO;

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class joy_core {

public:
  joy_core(void);
  ~joy_core(void);

  long get_last_error(JOY_LIB_STATUS *status);

  long get_error_string(long error_code,
			JOY_ERROR_STRING error_string);

  long init(JOY_DEV dev,
	    bool non_block);

  long finalize(JOY_DEV dev);

  long get_name(JOY_DEV dev,
		char *name,
		unsigned len);

  long get_axis(JOY_DEV dev,
		int *value);

  long get_buttons(JOY_DEV dev,
		   int *value);

  long get_event(JOY_DEV dev,
		 JOY_EVENT *event);

  long get_pos_unit_circle(int16_t x_value,
			   int16_t y_value,
			   JOY_POS_UNIT_CIRCLE *pos);

  long test_get_libjoy_prod_info(JOY_LIBJOY_PROD_INFO *prod_info);

private:
  // Error handling information
  JOY_ERROR_SOURCE m_error_source;
  long             m_error_code;
  bool             m_last_error_read;
  pthread_mutex_t  m_error_mutex;

  // Joystick devices
  JOY_DEV_INFO m_joy_dev_info[NR_JOY_DEVICES];

  // Private member functions
  long set_error(joy_exception exp);

  long update_error(joy_exception exp);

  long internal_get_error_string(long error_code,
				 JOY_ERROR_STRING error_string);

  void internal_init(JOY_DEV dev,
		     bool non_block);

  void internal_finalize(JOY_DEV dev);

  long internal_get_pos_unit_circle(int16_t x_value,
				    int16_t y_value,
				    JOY_POS_UNIT_CIRCLE *pos);

  long internal_test_get_libjoy_prod_info(JOY_LIBJOY_PROD_INFO *prod_info);
};

#endif // __JOY_CORE_H__
