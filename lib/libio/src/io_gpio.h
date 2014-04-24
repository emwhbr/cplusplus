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

#ifndef __IO_GPIO_H__
#define __IO_GPIO_H__

#include <semaphore.h>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definitions of macros
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class io_gpio {

public:
  io_gpio(void);
  ~io_gpio(void);

  void init(void);

  void reset_dev1(void);

  void reset_dev2(void);

  void finalize(void);

protected:

private:
  sem_t *m_dev1_sem;
  sem_t *m_dev2_sem;

  void init_members(void);

  void create_reset_pulse(double time_high,
			  double time_low,
			  int signal);

  void claim(sem_t *sem, 
	     const char *sem_name);
  
  void release(sem_t *sem,
	       const char *sem_name);
};

#endif // __IO_GPIO_H__
