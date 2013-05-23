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
#include <unistd.h>

#include "timer.h"

////////////////////////////////////////////////////////////////

int main(void)
{
  timer t1;
  timer t2;

  // Reset timers
  if ( t1.reset() != TIMER_SUCCESS) {
    printf("*** Error 1\n");
  }
  if ( t2.reset() != TIMER_SUCCESS) {
    printf("*** Error 2\n");
  }

  // Sleep for a while, timer1 and timer2 are running
  sleep(1);

  // Pause timer2
  if ( t2.pause() != TIMER_SUCCESS) {
    printf("*** Error 3\n");
  }

  // Check timer state
  if ( ! t1.is_running() ) {
    printf("*** Error 4\n");
  }
  if ( t2.is_running() ) {
    printf("*** Error 5\n");
  }  

  // Sleep for a while, only timer1 is running
  sleep(1);

  // Resume timer2
  if ( t2.resume() != TIMER_SUCCESS) {
    printf("*** Error 6\n");
  }

  // Sleep for a while, timer1 and timer2 are running
  sleep(1);

  // Get times
  double elapsed_t1 = t1.get_elapsed_time();
  double running_t1 = t1.get_running_time();
  
  double elapsed_t2 = t2.get_elapsed_time();
  double running_t2 = t2.get_running_time();

  printf("Timer 1, Elapsed: %f\n", elapsed_t1);
  printf("         Running: %f\n", running_t1);
  printf("\n");
  printf("Timer 2, Elapsed: %f\n", elapsed_t2);
  printf("         Running: %f\n", running_t2);

  // Test of polling timer
  timer t3;
  unsigned cnt = 0;

  t3.reset();
  while (t3.get_elapsed_time() < 5.0) {
    sleep(1);
    cnt++;
  }

  printf("\nSleep poll was activated %u times\n", cnt);
}
