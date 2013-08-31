// ************************************************************************
// *                                                                      *
// * Copyright (C) 2013 Bonden i Nol (hakanbrolin@hotmail.com)            *
// *                                                                      *
// * This program is free software; you can redistribute it and/or modify *
// * it under the terms of the GNU General Public License as published by *
// * the Free Software Foundation; either version 2 of the License, or    *
// * (at your option) any later version.                                  *
// *                                                                      *
// ************************************************************************

#include "test_cfg_file.h"

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////

// Valid item names
#define BOOL_TEST      "bool_test"
#define STRING_TEST    "string_test"
#define DOUBLE_TEST    "double_test"
#define INT_HEX_TEST   "int_hex_test"
#define INT_DEC_TEST   "int_dec_test"

// Default configuration values
#define DEF_BOOL_TEST     true
#define DEF_STRING_TEST   "/usr/include/syslog.h"
#define DEF_DOUBLE_TEST   123.456
#define DEF_INT_HEX_TEST  0xff
#define DEF_INT_DEC_TEST  1972

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

test_cfg_file::test_cfg_file(string file_name) : cfg_file(file_name)
{
  // Define default values for all items that
  // can be found in a config file
  // Note!
  // - Using anything but 'dec' on double will lead to error
  // - Specifier on string is not used and has no effect.
  set_default_item_value(BOOL_TEST,    bool(DEF_BOOL_TEST),     boolalpha);
  set_default_item_value(STRING_TEST,  string(DEF_STRING_TEST), left);
  set_default_item_value(DOUBLE_TEST,  double(DEF_DOUBLE_TEST), dec);
  set_default_item_value(INT_HEX_TEST, int(DEF_INT_HEX_TEST),   hex);
  set_default_item_value(INT_DEC_TEST, int(DEF_INT_DEC_TEST),   dec);
}

////////////////////////////////////////////////////////////////

test_cfg_file::~test_cfg_file(void)
{
}

////////////////////////////////////////////////////////////////

long test_cfg_file::get_bool(bool &value)
{
  return get_item_value(BOOL_TEST, value);
}

////////////////////////////////////////////////////////////////

long test_cfg_file::get_string(string &value)
{
  return get_item_value(STRING_TEST, value);
}

////////////////////////////////////////////////////////////////

long test_cfg_file::get_double(double &value)
{
  return get_item_value(DOUBLE_TEST, value);
}

////////////////////////////////////////////////////////////////

long test_cfg_file::get_int_hex(int &value)
{
  return get_item_value(INT_HEX_TEST, value);
}

////////////////////////////////////////////////////////////////

long test_cfg_file::get_int_dec(int &value)
{
  return get_item_value(INT_DEC_TEST, value);
}
