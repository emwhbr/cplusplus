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

#include <stdio.h>
#include <unistd.h>

#include "sys_stat.h"

////////////////////////////////////////////////////////////////

int main(void)
{
  sys_stat ss;
  float cpu_load;
  unsigned irq;
  unsigned mem_used;
  unsigned uptime;

  while (1) {

    // Reset CPU load interval
    if ( ss.reset_interval_cpu_load() != SYS_STAT_SUCCESS) {
      printf("*** Error 1-1\n");
    }

     // Reset IRQ interval
    if ( ss.reset_interval_irq() != SYS_STAT_SUCCESS) {
      printf("*** Error 1-2\n");
    }

    // Sleep for a while
    sleep(1);
    
    // Check CPU load for interval
    if ( ss.get_interval_cpu_load(cpu_load) != SYS_STAT_SUCCESS) {
      printf("*** Error 2-1\n");
    }

    // Check IRQ for interval
    if ( ss.get_interval_irq(irq) != SYS_STAT_SUCCESS) {
      printf("*** Error 2-2\n");
    }

    // Check MEM used
    if ( ss.get_mem_used_kb(mem_used) != SYS_STAT_SUCCESS) {
      printf("*** Error 2-3\n");
    }

    // Check UPTIME
    if ( ss.get_uptime_sec(uptime) != SYS_STAT_SUCCESS) {
      printf("*** Error 2-4\n");
    }

    printf("CPU load [%%]   = %f\n", cpu_load);
    printf("IRQ      [-]   = %u\n", irq);
    printf("Mem used [KB]  = %u\n", mem_used);
    printf("Uptime   [sec] = %u\n\n", uptime);
  }
}
