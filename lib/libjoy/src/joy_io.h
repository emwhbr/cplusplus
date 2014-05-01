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

#ifndef __JOY_IO_H__
#define __JOY_IO_H__

#include <stdint.h>
#include <string>

#include "joy.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definitions of macros
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class joy_io {

public:
  joy_io(string joy_dev);
  ~joy_io(void);

  void init(bool non_block);

  void finalize(void);

  long get_name(char *name,
		unsigned len);

  long get_axis(int *value);

  long get_buttons(int *value);

  long get_event(JOY_EVENT *event);

protected:

private:
  string m_joy_dev;
  int    m_joy_fd;

  bool m_non_block;

  char    m_name[256];
  uint8_t m_axis;
  uint8_t m_buttons;  

  void init_members(void);
};

#endif // __JOY_IO_H__
