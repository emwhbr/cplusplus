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

#ifndef __TEST_CFG_FILE_H__
#define __TEST_CFG_FILE_H__

#include "cfg_file.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class test_cfg_file : public cfg_file {

 public:
  test_cfg_file(string file_name);
  ~test_cfg_file(void);

  long get_bool(bool &value);
  long get_string(string &value);
  long get_double(double &value);
  long get_int_hex(int &value);
  long get_int_dec(int &value);
};

#endif // __TEST_CFG_FILE_H__
