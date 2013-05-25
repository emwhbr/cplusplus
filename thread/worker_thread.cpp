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

#include <unistd.h>
#include <iostream>

#include "worker_thread.h"

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

worker_thread::worker_thread(string thread_name,
			     unsigned work_id,
			     unsigned long data_base_addr) : thread(thread_name)
{
#ifdef DEBUG_PRINTS
  cout << "worker_thread::worker_thread = " + get_name() << endl;
#endif

  m_work_id        = work_id;
  m_data_base_addr = data_base_addr;
}

////////////////////////////////////////////////////////////////

worker_thread::~worker_thread(void)
{
#ifdef DEBUG_PRINTS
  cout << "worker_thread::~worker_thread = " + get_name() << endl;
#endif
}

/////////////////////////////////////////////////////////////////////////////
//               Protected member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

long worker_thread::setup(void)
{
#ifdef DEBUG_PRINTS
  cout << "worker_thread::setup = " + get_name() << endl;
#endif

  return THREAD_SUCCESS;
}

////////////////////////////////////////////////////////////////

long worker_thread::execute(void *arg)
{
#ifdef DEBUG_PRINTS
  cout << "worker_thread::execute = " + get_name() << endl;
#endif

  // Make GCC happy (-Wextra)
  if (arg) {
    return THREAD_INTERNAL_ERROR;
  }

  // Get pointer to thread's data
  volatile WORKER_DATA *data =
    (WORKER_DATA *) (m_data_base_addr + (m_work_id) * sizeof(WORKER_DATA));

  *data = 0;

  // Produce data
  while ( !is_stopped() ) {

    *data += 1;

    update_exe_cnt();

    sleep(1);
  }

  return THREAD_SUCCESS;
}

////////////////////////////////////////////////////////////////

long worker_thread::cleanup(void)
{
#ifdef DEBUG_PRINTS
  cout << "worker_thread::cleanup = " + get_name() << endl;
#endif
  
  return THREAD_SUCCESS;
}
