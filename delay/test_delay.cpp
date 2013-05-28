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
#include <time.h>

#include "delay.h"

////////////////////////////////////////////////////////////////

double get_time_diff(timespec start, timespec end)
{
  timespec temp;

  // This function assumes end >= start, use with care

  if ( (end.tv_nsec - start.tv_nsec) < 0 ) {
    // Borrow one second
    temp.tv_sec  = end.tv_sec - start.tv_sec - 1;
    temp.tv_nsec = 1000000000L + end.tv_nsec - start.tv_nsec;
  } else {
    temp.tv_sec  = end.tv_sec - start.tv_sec;
    temp.tv_nsec = end.tv_nsec - start.tv_nsec;
  }

  return (double)temp.tv_sec +
         (double)(temp.tv_nsec) / 1000000000.0;
}

////////////////////////////////////////////////////////////////

int main(void)
{
  const clockid_t the_clock = get_clock_id();

  struct timespec t1;
  struct timespec t2; 

  double delay_interval;

  //////////////////////////////
  // Testing delay 1(2)
  //////////////////////////////
  delay_interval = 0.005;

  clock_gettime(the_clock, &t1);
  if ( delay(delay_interval) != DELAY_SUCCESS) {
    printf("*** Error1\n");
    return 1;
  }
  clock_gettime(the_clock, &t2);

  printf("Requested delay: %f, Actual: %f\n",
	 delay_interval, get_time_diff(t1, t2));

  //////////////////////////////
  // Testing delay 2(2)
  //////////////////////////////
  delay_interval = 0.005; // 5 ms

  // Run for five seconds
  clock_gettime(the_clock, &t1);
  for (int i=0; i < 1000; i++) {

    if ( delay(delay_interval) != DELAY_SUCCESS) {
      printf("*** Error2\n");
      return 1;
    }
  }
  clock_gettime(the_clock, &t2);

  printf("\nTotal time using delay: %f\n",
	 get_time_diff(t1, t2));

  ///////////////////////////
  // Testing delay_until
  ///////////////////////////
  delay_interval = 0.005; // 5 ms

  // Start after one second from now
  clock_gettime(the_clock, &t1);
  get_new_time(&t1, 1.0, &t2);
  delay_until(&t2);

  // Run for five seconds
  clock_gettime(the_clock, &t1);
  for (int i=0; i < 1000; i++) {

    // Do cyclic work here

    // Calculate next interval
    get_new_time(&t2, delay_interval, &t2);
    if ( delay_until(&t2) != DELAY_SUCCESS) {
      printf("*** Error3\n");
      return 1;
    }
  }
  clock_gettime(the_clock, &t2);

  printf("\nTotal time using delay_until: %f\n",
	 get_time_diff(t1, t2));

  return 0;
}
