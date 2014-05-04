/************************************************************************
 *                                                                      *
 * Copyright (C) 2014 Bonden i Nol (hakanbrolin@hotmail.com)            *
 *                                                                      *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/select.h>

#include "joy.h"

#ifdef DEBUG_PRINTS
#define debug_test_printf(fmt, args...)  printf("DBG - "); printf(fmt, ##args); fflush(stdout)
#else
#define debug_test_printf(fmt, args...) 
#endif /* DEBUG_PRINTS */

/*
 * ---------------------------------
 *       Macros
 * ---------------------------------
 */
#define TEST_LIBJOY_ERROR_MSG "*** ERROR : test_libjoy\n"

#define C_PI 3.14159265358979323846

/*
 * ---------------------------------
 *       Types
 * ---------------------------------
 */

/*
 * ---------------------------------
 *       Global variables
 * ---------------------------------
 */

/*
 * ---------------------------------
 *       Function prototypes
 * ---------------------------------
 */
static int kbhit(void);
static JOY_DEV get_dev_from_user(void);
static void get_prod_info(void);
static void get_last_error(void);
static void initialize(void);
static void finalize(void);
static void get_joystick_info(void);
static void get_joystick_events(void);
static void get_pos_unit_circle(void);
static void get_joystick_positions(void);
static void print_menu(void);
static void do_test_libjoy(void);

/*****************************************************************/

int kbhit(void)
{
  struct timeval tv;
  fd_set fds;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds); // Standard input

  select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);

  return FD_ISSET(0, &fds);
}

/*****************************************************************/

static JOY_DEV get_dev_from_user(void)
{
  int rc;
  unsigned dev_value;
  JOY_DEV dev = JOY_DEV_0;

  /* User input */
  do {
    printf("Enter joystick device[0..1]: ");
    rc = scanf("%u", &dev_value);
    if (rc == 1) {
      switch (dev_value) {
      case 0:
	dev = JOY_DEV_0;
	break;
      case 1:
	dev = JOY_DEV_1;
	break;
      }
    }
  } while (dev_value > 1);

  return dev;
}

/*****************************************************************/

static void get_prod_info(void)
{
  JOY_LIBJOY_PROD_INFO prod_info;

  if (joy_test_get_libjoy_prod_info(&prod_info)) {
    printf(TEST_LIBJOY_ERROR_MSG);
    return;
  }
  printf("LIBJOY prod num: %s\n", prod_info.prod_num);
  printf("LIBJOY rstate  : %s\n", prod_info.rstate);
}

/*****************************************************************/

static void get_last_error(void)
{
  JOY_LIB_STATUS status;
  JOY_ERROR_STRING error_string;

  if (joy_get_last_error(&status)) {
    printf(TEST_LIBJOY_ERROR_MSG);
    return;
  }

  if (joy_get_error_string(status.error_code, error_string)) {
    printf(TEST_LIBJOY_ERROR_MSG);
    return;
  }

  switch (status.error_source) {
  case JOY_INTERNAL_ERROR:
    printf("LIBJOY error source : JOY_INTERNAL_ERROR\n");
    break;
  case JOY_LINUX_ERROR:
    printf("LIBJOY error source : JOY_LINUX_ERROR\n");
    break;
  default:
    printf("LIBJOY error source : *** UNKNOWN\n");
  }
  printf("LIBJOY error code   : %ld\n", status.error_code);
  printf("LIBJOY error string : %s\n",  error_string);
}

/*****************************************************************/

static void initialize(void)
{
  int rc;
  JOY_DEV dev;
  unsigned non_block_value;
  bool non_block;

  /* User input */
  dev = get_dev_from_user();

  printf("Non-block[0=false, 1=true]: ");
  rc = scanf("%u", &non_block_value);
  if (rc != 1) {
    return;    
  }
  non_block = (non_block_value ? true : false);

  /* Do initialization */
  if (joy_initialize(dev, non_block) != JOY_SUCCESS) {
    printf(TEST_LIBJOY_ERROR_MSG);
    return;
  }
}

/*****************************************************************/

static void finalize(void)
{
  JOY_DEV dev;

  /* User input */
  dev = get_dev_from_user();

  /* Do finalization */
  if (joy_finalize(dev) != JOY_SUCCESS) {
    printf(TEST_LIBJOY_ERROR_MSG);
  }
}

/*****************************************************************/

static void get_joystick_info(void)
{
  JOY_DEV dev;
  int axis, buttons;
  char name[300];

  /* User input */
  dev = get_dev_from_user();

  /* Get joystick info */
  if (joy_get_name(dev, name, 300) != JOY_SUCCESS) {
    printf(TEST_LIBJOY_ERROR_MSG);
    return;
  }
  if (joy_get_axis(dev, &axis) != JOY_SUCCESS) {
    printf(TEST_LIBJOY_ERROR_MSG);
    return;
  }
  if (joy_get_buttons(dev, &buttons) != JOY_SUCCESS) {
    printf(TEST_LIBJOY_ERROR_MSG);
    return;
  }

  printf("Name:%s, Axis:%d, Buttons:%d\n", name, axis, buttons);
}

/*****************************************************************/

static void get_joystick_events(void)
{
  JOY_DEV dev;
  JOY_EVENT event;
  unsigned cnt = 0;

  /* User input */
  dev = get_dev_from_user();

  printf("Press ENTER to quit...\n");

  while (!kbhit()) {
    if (joy_get_event(dev, &event) != JOY_SUCCESS) {
      printf(TEST_LIBJOY_ERROR_MSG);
      return;
    }
    printf("%7u=>Type:%2x, Value:%6d, Number:%3d\n",
	   ++cnt,
	   event.type, event.value, event.number);
  }
}

/*****************************************************************/

static void get_pos_unit_circle(void)
{
  int rc;
  int x_value;
  int y_value;
  JOY_POS_UNIT_CIRCLE pos;

  /* User input */
  printf("x-value[-32767..32767]: ");
  rc = scanf("%d", &x_value);
  if (rc != 1) {
    return;
  }
  printf("y-value[-32767..32767]: ");
  rc= scanf("%d", &y_value);
  if (rc != 1) {
    return;
  }

  joy_get_pos_unit_circle((int16_t)x_value, (int16_t)y_value, &pos);

  printf("UNIT CIRCLE\n");
  printf("  x: %f\n", pos.x);
  printf("  y: %f\n", pos.y);
  printf("      r: %f\n", pos.r);
  printf("  theta: %f(rad), %f(deg)\n", pos.theta, pos.theta * 180.0f/C_PI);
}

/*****************************************************************/

static void get_joystick_positions(void)
{
  JOY_DEV dev;
  JOY_EVENT event;
  unsigned cnt = 0;

  int16_t x_value = 0;
  int16_t y_value = 0;

  JOY_POS_UNIT_CIRCLE pos;

  /* User input */
  dev = get_dev_from_user();

  printf("Press ENTER to quit...\n");

  while (!kbhit()) {

    // Get joystick event
    if (joy_get_event(dev, &event) != JOY_SUCCESS) {
      printf(TEST_LIBJOY_ERROR_MSG);
      return;
    }

    cnt++; // Another event has been read

    // Handle horizontal value
    if ( (event.type & JOY_EVENT_AXIS) &&
	 (event.number == JOY_X_AXIS) ) {
      x_value = event.value;
    }
    // Handle vertical value
    if ( (event.type & JOY_EVENT_AXIS) &&
	 (event.number == JOY_Y_AXIS) ) {
      y_value = event.value;
    }

    // Only print info about horizontal and vertical events
    if ( (event.type & JOY_EVENT_AXIS) &&
	 ( (event.number == JOY_X_AXIS) ||
	   (event.number == JOY_Y_AXIS) ) ) {

      // Convert to unit circle
      joy_get_pos_unit_circle(x_value, y_value, &pos);

      printf("%7u=> x=%.3f, y=%.3f, r=%.3f, theta: %4.1f\n",
	     cnt,
	     pos.x,
	     pos.y,
	     pos.r,
	     pos.theta * 180.0f/C_PI);
    }
  }
}
 
/*****************************************************************/

static void print_menu(void)
{
  printf("\n");
  printf("  1. (test) get product info\n");
  printf("  2. get last error + get error string\n");
  printf("  3. initialize\n");
  printf("  4. finalize\n");
  printf("  5. get joystick info\n");
  printf("  6. get joystick events\n");
  printf("  7. get pos unit circle\n");
  printf("  8. get joystick positions (unit circle)\n");
  printf("100. Exit\n\n");
}

/*****************************************************************/

static void do_test_libjoy(void)
{
  int rc;
  int value;

  do {
    print_menu();
    
    printf("Enter choice : ");
    rc=scanf("%d",&value);
    if (rc != 1) {
      printf("Illegal choice!\n");
      continue;
    }
    
    switch(value) {
    case 1:
      get_prod_info();
      break;
    case 2:
      get_last_error();
      break;
    case 3:
      initialize();
      break;
    case 4:
      finalize();
      break;
    case 5:
      get_joystick_info();
      break;
    case 6:
      get_joystick_events();
      break;
    case 7:
      get_pos_unit_circle();
      break;
    case 8:
      get_joystick_positions();
      break;
    case 100: /* Exit */
      break;    
    default:
      printf("Illegal choice!\n");
    }
  } while (value != 100);
}

/*****************************************************************/

int main(void)
{
  do_test_libjoy();

  printf("Goodbye!\n");
  return 0;
}
