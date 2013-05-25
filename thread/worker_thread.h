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

#ifndef __WORKER_THREAD_H__
#define __WORKER_THREAD_H__

#include <stdint.h>

#include "thread.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////

#define WORKER_MAX_THREADS 5

typedef uint32_t WORKER_DATA;

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class worker_thread : public thread {

 public:
  worker_thread(string thread_name,
		unsigned work_id,
		unsigned long data_base_addr);
  ~worker_thread(void);

 protected:
  long setup(void);        // Implements pure virtual functions from base class
  long execute(void *arg); // Implements pure virtual functions from base class
  long cleanup(void);      // Implements pure virtual functions from base class
    
 private:
  unsigned m_work_id;
  unsigned long m_data_base_addr;
};

#endif // __WORKER_THREAD_H__
