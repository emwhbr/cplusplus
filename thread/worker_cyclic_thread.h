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

#ifndef __WORKER_CYCLIC_THREAD_H__
#define __WORKER_CYCLIC_THREAD_H__

#include "cyclic_thread.h"
#include "timer.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class worker_cyclic_thread : public cyclic_thread {

 public:
  worker_cyclic_thread(string thread_name,
		       double frequency);
  ~worker_cyclic_thread(void);

  double get_min_period(void) {return m_min_period;}
  double get_max_period(void) {return m_max_period;}
  double get_avg_period(void) {return m_avg_period;}

 protected:
  virtual long setup(void);   // Implements pure virtual function from base class
  virtual long cleanup(void); // Implements pure virtual function from base class

  virtual long cyclic_execute(void); // Implements pure virtual function from base class
    
 private:
  timer m_period_timer;

  double m_min_period;
  double m_max_period;
  double m_avg_period;
  double m_tot_period;
  bool   m_first_cycle;

  void init_members(void);
};

#endif // __WORKER_CYCLIC_THREAD_H__
