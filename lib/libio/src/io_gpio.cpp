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

#include <fcntl.h>
#include <sys/stat.h>

#include "io_gpio.h"
#include "io_exception.h"
#include "io_utility.h"
#include "io_hw.h"

/////////////////////////////////////////////////////////////////////////////
//               Definitions of macros
/////////////////////////////////////////////////////////////////////////////
#define RESET_PULSE_HI_TIME  0.1 // seconds
#define RESET_PULSE_LO_TIME  0.1 // seconds

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

io_gpio::io_gpio(void)
{
  init_members();
}

////////////////////////////////////////////////////////////////

io_gpio::~io_gpio(void)
{
}

////////////////////////////////////////////////////////////////

void io_gpio::init(void)
{
  m_dev1_sem = sem_open(DEV1_SEM_NAME,
			O_CREAT,
			S_IRWXU | S_IRWXG | S_IRWXO,
			1); // Semaphore is available

 if (m_dev1_sem == SEM_FAILED) {
    THROW_EXP(IO_LINUX_ERROR, IO_SEM_OPERATION_FAILED,
	      "sem_open for %s",
	      DEV1_SEM_NAME);
 }

  m_dev2_sem = sem_open(DEV2_SEM_NAME,
			O_CREAT,
			S_IRWXU | S_IRWXG | S_IRWXO,
			1); // Semaphore is available

 if (m_dev2_sem == SEM_FAILED) {
    THROW_EXP(IO_LINUX_ERROR, IO_SEM_OPERATION_FAILED,
	      "sem_open for %s",
	      DEV2_SEM_NAME);
 }
}

////////////////////////////////////////////////////////////////

void io_gpio::reset_dev1(void)
{
  // Lockdown DEV1
  claim(m_dev1_sem, DEV1_SEM_NAME);

  try {
    // Do reset of DEV1
    create_reset_pulse(RESET_PULSE_HI_TIME,
		       RESET_PULSE_LO_TIME,
		       DEV1_RESET_SIGNAL);
  }
  catch (...) {
    release(m_dev1_sem, DEV1_SEM_NAME);
    throw;
  }

   // Lockup DEV1
  release(m_dev1_sem, DEV1_SEM_NAME);
}

////////////////////////////////////////////////////////////////

void io_gpio::reset_dev2(void)
{
  // Lockdown DEV2
  claim(m_dev2_sem, DEV2_SEM_NAME);

  try {
    // Do reset of DEV2
    create_reset_pulse(RESET_PULSE_HI_TIME,
		       RESET_PULSE_LO_TIME,
		       DEV2_RESET_SIGNAL);
  }
  catch (...) {
    release(m_dev2_sem, DEV2_SEM_NAME);
    throw;
  }

  // Lockup DEV2
  release(m_dev2_sem, DEV2_SEM_NAME);
}

////////////////////////////////////////////////////////////////

void io_gpio::finalize(void)
{
  int rc;

  // Destroy the linux semaphore for DEV1 protection
  if (m_dev1_sem) {
    rc = sem_close(m_dev1_sem);
    if (rc) {
      THROW_EXP(IO_LINUX_ERROR, IO_SEM_OPERATION_FAILED,
		"sem_close for %s",
		DEV1_SEM_NAME);
    }
    rc = sem_unlink(DEV1_SEM_NAME);
    if (rc) {
      THROW_EXP(IO_LINUX_ERROR, IO_SEM_OPERATION_FAILED,
		"sem_unlink for %s",
		DEV1_SEM_NAME);      
    }
  }  

  // Destroy the linux semaphore for DEV2 protection
  if (m_dev2_sem) {
    rc = sem_close(m_dev2_sem);
    if (rc) {
      THROW_EXP(IO_LINUX_ERROR, IO_SEM_OPERATION_FAILED,
		"sem_close for %s",
		DEV2_SEM_NAME);
    }
    rc = sem_unlink(DEV2_SEM_NAME);
    if (rc) {
      THROW_EXP(IO_LINUX_ERROR, IO_SEM_OPERATION_FAILED,
		"sem_unlink for %s",
		DEV2_SEM_NAME);      
    }
  } 

  // Reset members
  init_members();
}

/////////////////////////////////////////////////////////////////////////////
//               Protected member functions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Private member functions
/////////////////////////////////////////////////////////////////////////////

void io_gpio::init_members(void)
{
  m_dev1_sem = NULL;
  m_dev2_sem = NULL;
}

////////////////////////////////////////////////////////////////

void io_gpio::create_reset_pulse(double time_high,
				 double time_low,
				 int signal)
{
  if (io_do_nanosleep(time_high) != IO_SUCCESS) {
    THROW_EXP(IO_INTERNAL_ERROR, IO_TIME_ERROR, 
	      "io_do_nanosleep (hi) failed for signal:%d", signal);
  }

  if (io_do_nanosleep(time_low) != IO_SUCCESS) {
    THROW_EXP(IO_INTERNAL_ERROR, IO_TIME_ERROR, 
	      "io_do_nanosleep (lo) failed for signal:%d", signal);
  }
}

////////////////////////////////////////////////////////////////

void io_gpio::claim(sem_t *sem, 
		    const char *sem_name)
{
  if ( sem_wait(sem) == -1 ) {
    THROW_EXP(IO_LINUX_ERROR, IO_SEM_OPERATION_FAILED,
	      "sem_wait failed for %s", sem_name);
  }
}

////////////////////////////////////////////////////////////////

void io_gpio::release(sem_t *sem,	       
		      const char *sem_name)
{
  if ( sem_post(sem) == -1 ) {
    THROW_EXP(IO_LINUX_ERROR, IO_SEM_OPERATION_FAILED,
	      "sem_post failed for %s)", sem_name);
  }
}
