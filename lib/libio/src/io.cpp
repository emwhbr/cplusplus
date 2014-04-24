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

#include "io.h"
#include "io_core.h"

////////////////////////////
// Module global variables 
////////////////////////////
static io_core g_object;

////////////////////////////////////////////////////////////////

long io_get_last_error(IO_LIB_STATUS *status)
{
  return g_object.get_last_error(status);
}

////////////////////////////////////////////////////////////////

long io_get_error_string(long error_code,
			 IO_ERROR_STRING error_string)
{
  return g_object.get_error_string(error_code, error_string);  
}

////////////////////////////////////////////////////////////////

long io_initialize(void)
{
  return g_object.init();
}

////////////////////////////////////////////////////////////////

long io_finalize(void)
{
  return g_object.finalize();  
}

////////////////////////////////////////////////////////////////

long io_reset_dev1(void)
{
  return g_object.reset_dev1();
}

////////////////////////////////////////////////////////////////

long io_reset_dev2(void)
{
  return g_object.reset_dev2();
}

////////////////////////////////////////////////////////////////

long io_test_get_libio_prod_info(IO_LIBIO_PROD_INFO *prod_info)
{
  return g_object.test_get_libio_prod_info(prod_info);
}
