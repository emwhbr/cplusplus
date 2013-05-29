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

#include "cyclic_thread.h"
#include "delay.h"

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

cyclic_thread::cyclic_thread(string thread_name,
			     double frequency) : thread(thread_name)
{
  m_frequency = frequency;
}

////////////////////////////////////////////////////////////////

cyclic_thread::~cyclic_thread(void)
{
}

/////////////////////////////////////////////////////////////////////////////
//               Protected member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

long cyclic_thread::execute(void *arg)
{
  const float delay_interval = 1.0 / m_frequency;
  const clockid_t the_clock = get_clock_id();

  struct timespec t1;
  struct timespec t2; 

  // Make GCC happy (-Wextra)
  if (arg) {
    return THREAD_INTERNAL_ERROR;
  }

  // Prepare first run
  clock_gettime(the_clock, &t1);
  get_new_time(&t1, delay_interval, &t2);
  delay_until(&t2);

  while ( !is_stopped() ) {

    // Do cyclic work
    if ( cyclic_execute() != THREAD_SUCCESS ) {
      return THREAD_INTERNAL_ERROR;
    }

    // Calculate next interval
    get_new_time(&t2, delay_interval, &t2);
    if ( delay_until(&t2) != DELAY_SUCCESS) {
      return THREAD_TIME_ERROR;
    }

    update_exe_cnt();
  }

  return THREAD_SUCCESS;
}
