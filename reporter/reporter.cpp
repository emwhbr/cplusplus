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

#include <memory>

#include "reporter.h"

reporter* reporter::m_instance = NULL;

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

reporter::~reporter(void)
{
}

////////////////////////////////////////////////////////////////

reporter* reporter::instance(void)
{
  if (!m_instance) {
    m_instance = new reporter;
    static auto_ptr<reporter> m_auto = auto_ptr<reporter>(m_instance);
  }
  return m_instance;
}

////////////////////////////////////////////////////////////////

void reporter::set_debug(bool on)
{
  m_debug = on;
}

////////////////////////////////////////////////////////////////

void reporter::put_line(string str)
{
  if (m_debug) {
    put_line_always(str);
  }
}

////////////////////////////////////////////////////////////////

void reporter::put(string str)
{
  if (m_debug) {
    put_always(str);
  }
}

////////////////////////////////////////////////////////////////

void reporter::put_line_always(string str)
{
  put_always(str + "\n");
}

////////////////////////////////////////////////////////////////

void reporter::put_always(string str)
{
  cout << str;
}
