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

#ifndef __REPORTER_H__
#define __REPORTER_H__

#include <iostream>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////

#define SET_DEBUG       reporter::instance()->set_debug
#define PUT_LINE        reporter::instance()->put_line
#define PUT_LINE_ALWAYS reporter::instance()->put_line_always
#define PUT             reporter::instance()->put
#define PUT_ALWAYS      reporter::instance()->put_always

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class reporter {

 public:
  ~reporter(void);
  static reporter* instance(void);

  void set_debug(bool on);
  void put_line(string str);
  void put(string str);
  void put_line_always(string str);
  void put_always(string str);

 private:
  reporter(void) {m_debug = true;};  // Private so it can't be called
  static reporter *m_instance;
  bool m_debug;
};

#endif // __REPORTER_H__
