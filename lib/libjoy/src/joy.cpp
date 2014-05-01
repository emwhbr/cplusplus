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

#include "joy.h"
#include "joy_core.h"

////////////////////////////
// Module global variables 
////////////////////////////
static joy_core g_object;

////////////////////////////////////////////////////////////////

long joy_get_last_error(JOY_LIB_STATUS *status)
{
  return g_object.get_last_error(status);
}

////////////////////////////////////////////////////////////////

long joy_get_error_string(long error_code,
			  JOY_ERROR_STRING error_string)
{
  return g_object.get_error_string(error_code, error_string);  
}

////////////////////////////////////////////////////////////////

long joy_initialize(JOY_DEV dev,
		    bool non_block)
{
  return g_object.init(dev, non_block);
}

////////////////////////////////////////////////////////////////

long joy_finalize(JOY_DEV dev)
{
  return g_object.finalize(dev);  
}

////////////////////////////////////////////////////////////////

long joy_get_name(JOY_DEV dev,
		  char *name,
		  unsigned len)

{
  return g_object.get_name(dev, name, len);
}

////////////////////////////////////////////////////////////////

long joy_get_axis(JOY_DEV dev,
		  int *axis)
{
  return g_object.get_axis(dev, axis);
}

////////////////////////////////////////////////////////////////

long joy_get_buttons(JOY_DEV dev,
		     int *buttons)
{
  return g_object.get_buttons(dev, buttons);
}

////////////////////////////////////////////////////////////////

long joy_get_event(JOY_DEV dev,
		   JOY_EVENT *event)
{
  return g_object.get_event(dev, event);
}

////////////////////////////////////////////////////////////////

long joy_get_pos_unit_circle(int16_t x_value,
			     int16_t y_value,
			     JOY_POS_UNIT_CIRCLE *pos)
{
  return g_object.get_pos_unit_circle(x_value,
				      y_value,
				      pos);
}

////////////////////////////////////////////////////////////////

long joy_test_get_libjoy_prod_info(JOY_LIBJOY_PROD_INFO *prod_info)
{
  return g_object.test_get_libjoy_prod_info(prod_info);
}
