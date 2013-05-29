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

#include <stdio.h>
#include <sched.h>
#include <unistd.h>

#include "worker_cyclic_thread.h"

////////////////////////////////////////////////////////////////

void print_thread_info(worker_cyclic_thread *wct)
{
  printf("Thread name: %s, PID: %u, TID: %u\n",
	 wct->get_name().c_str(),
	 wct->get_pid(),
	 wct->get_tid());
}

////////////////////////////////////////////////////////////////

int main(void)
{
  const double thread_frequency = 50.0; // Hz

  // Create thread
  worker_cyclic_thread *wct = new worker_cyclic_thread("TEST-CYCLIC-WORKER",
						       thread_frequency);
  // Start thread
  if ( wct->start(NULL) != THREAD_SUCCESS ) {
    printf("*** Error : start thread\n");
    goto error;
  }

  // Wait for thread to complete setup
  do {
    if ( wct->get_status() != THREAD_STATUS_OK ) {
      break;
    }
    sched_yield();
  } while ( wct->get_state() != THREAD_STATE_SETUP_DONE );

  if ( wct->get_status() != THREAD_STATUS_OK ) {
    printf("*** Error : thread status not OK: 0x%x, state:%u\n",
	   wct->get_status(), wct->get_state());
    goto error;
  }

  print_thread_info(wct);

  // Release thread
  if ( wct->release() != THREAD_SUCCESS ) {
    printf("*** Error : release thread\n");
    goto error;
  }

  // Take it easy, let thread work
  printf("About to sleep, let cyclic thread work\n");
  sleep(60);

  // Check that thread is working
  if ( wct->get_state() != THREAD_STATE_EXECUTING) {
    printf("*** Error : thread is not working\n");
    goto error;
  }

  // Stop thread
  if ( wct->stop() ) {
    printf("*** Error : stop thread\n");
    goto error;
  }

  // Wait for thread to complete
  if ( wct->wait() != THREAD_SUCCESS) {
    printf("*** Error : wait thread\n");
    goto error;
  }

  // Check that thread is completed
  if ( wct->get_state() != THREAD_STATE_NOT_STARTED) {
    printf("*** Error : thread is not done\n");
    goto error;
  }

  // Get thread status
  if ( wct->get_status() != THREAD_STATUS_OK ) {
    printf("*** Error : thread status not OK: 0x%x, state:%u\n",
	   wct->get_status(), wct->get_state());
    goto error;
  }

  // Check data produced by thread
  printf("\nthread min period = %f\n", wct->get_min_period());
  printf("thread max period = %f\n",   wct->get_max_period());
  printf("thread avg period = %f\n",   wct->get_avg_period());
  printf("thread exe count  = %u\n",   wct->get_exe_cnt());

  printf("\nthread expected frequency : %f Hz\n", thread_frequency);
  printf("thread expected period    : %f s\n", (1.0 / thread_frequency));

  // Delete thread
  delete wct;

  return 0;

 error:
  if (wct) {
    delete wct;
  }
  return 1;
}
