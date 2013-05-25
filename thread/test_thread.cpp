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
#include <stdint.h>
#include <sched.h>
#include <unistd.h>
#include <strings.h>

#include "worker_thread.h"

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////

WORKER_DATA g_worker_data[WORKER_MAX_THREADS];

////////////////////////////////////////////////////////////////

void print_thread_info(worker_thread *wt)
{
  printf("Thread name: %s, PID: %u, TID: %u\n",
	 wt->get_name().c_str(),
	 wt->get_pid(),
	 wt->get_tid());
}

////////////////////////////////////////////////////////////////

int main(void)
{
  const unsigned work_id = 3;

  bzero(g_worker_data, sizeof(g_worker_data));

  // Create thread
  worker_thread *wt = new worker_thread("TEST-WORKER",
					work_id,
					(unsigned long)&g_worker_data);
  // Start thread
  if ( wt->start(NULL) != THREAD_SUCCESS ) {
    printf("*** Error : start thread\n");
    goto error;
  }

  // Wait for thread to complete setup
  do {
    if ( wt->get_status() != THREAD_STATUS_OK ) {
      break;
    }
    sched_yield();
  } while ( wt->get_state() != THREAD_STATE_SETUP_DONE );

  if ( wt->get_status() != THREAD_STATUS_OK ) {
    printf("*** Error : thread status not OK: 0x%x, state:%u\n",
	   wt->get_status(), wt->get_state());
    goto error;
  }

  print_thread_info(wt);

  // Release thread
  if ( wt->release() != THREAD_SUCCESS ) {
    printf("*** Error : release thread\n");
    goto error;
  }

  // Take it easy, let thread work
  printf("About to sleep, let thread work\n");
  sleep(10);

  // Check that thread is working
  if ( wt->get_state() != THREAD_STATE_EXECUTING) {
    printf("*** Error : thread is not working\n");
    goto error;
  }

  // Stop thread
  if ( wt->stop() ) {
    printf("*** Error : stop thread\n");
    goto error;
  }

  // Wait for thread to complete
  if ( wt->wait() != THREAD_SUCCESS) {
    printf("*** Error : wait thread\n");
    goto error;
  }

  // Check that thread is completed
  if ( wt->get_state() != THREAD_STATE_NOT_STARTED) {
    printf("*** Error : thread is not done\n");
    goto error;
  }

  // Get thread status
  if ( wt->get_status() != THREAD_STATUS_OK ) {
    printf("*** Error : thread status not OK: 0x%x, state:%u\n",
	   wt->get_status(), wt->get_state());
    goto error;
  }

  // Check data produced by thread
  printf("\n");
  for (int i=0; i < WORKER_MAX_THREADS; i++) {
    printf("g_worker_data[%d] = %u\n",
	   i, g_worker_data[i]);
  }

  printf("\nthread exe count = %u\n", wt->get_exe_cnt());

  if (wt->get_exe_cnt() != g_worker_data[work_id]) {
    printf("*** Error : thread created unexpected result\n");
  }

  // Delete thread
  delete wt;

  return 0;

 error:
  if (wt) {
    delete wt;
  }
  return 1;
}
