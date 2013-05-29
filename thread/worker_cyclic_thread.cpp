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

#include <iostream>

#include "worker_cyclic_thread.h"

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

worker_cyclic_thread::worker_cyclic_thread(string thread_name,
					   double frequency) : cyclic_thread(thread_name,
									     frequency)
{
#ifdef DEBUG_PRINTS
  cout << "worker_cyclic_thread::worker_cyclic_thread = " + get_name() << endl;
#endif

  init_members();
}

////////////////////////////////////////////////////////////////

worker_cyclic_thread::~worker_cyclic_thread(void)
{
#ifdef DEBUG_PRINTS
  cout << "worker_cyclic_thread::~worker_cyclic_thread = " + get_name() << endl;
#endif
}

/////////////////////////////////////////////////////////////////////////////
//               Protected member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

long worker_cyclic_thread::setup(void)
{
#ifdef DEBUG_PRINTS
  cout << "worker_cyclic_thread::setup = " + get_name() << endl;
#endif

  init_members();

  return THREAD_SUCCESS;
}

////////////////////////////////////////////////////////////////

long worker_cyclic_thread::cleanup(void)
{
#ifdef DEBUG_PRINTS
  cout << "worker_cyclic_thread::cleanup = " + get_name() << endl;
#endif
  
  return THREAD_SUCCESS;
}

////////////////////////////////////////////////////////////////

long worker_cyclic_thread::cyclic_execute(void)
{
  double period = m_period_timer.get_elapsed_time();

  // Restart timer for next cycle
  m_period_timer.reset();

  // Get period time statistics
  if (m_first_cycle) {
    m_first_cycle = false;
  }
  else {    
    if (period < m_min_period) {
#ifdef DEBUG_PRINTS
      cout << "---" << get_name() 
	   << " (min) : " << period << " cycle : " << get_exe_cnt() << endl;
#endif
      m_min_period = period;
    }
    if (period > m_max_period) {
#ifdef DEBUG_PRINTS
      cout << "+++" << get_name() 
	   << " (max) : " << period << " cycle : " << get_exe_cnt() << endl;
#endif
      m_max_period = period;
    }
    // Note! exe_cnt starts at 0, updated AFTER each cycle
    m_tot_period += period;
    m_avg_period = m_tot_period / double(get_exe_cnt());
  }

   return THREAD_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//               Private member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

void worker_cyclic_thread::init_members(void)
{
  m_min_period  = 99999999.0; // Large enough
  m_max_period  = -1.0;       // Small enough
  m_avg_period  = 0.0;
  m_tot_period  = 0.0;
  m_first_cycle = true;
}
