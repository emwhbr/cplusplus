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

#include "io.h"

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
#define TEST_LIBIO_ERROR_MSG "*** ERROR : test_libio\n"

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
static void get_prod_info(void);
static void get_last_error(void);
static void initialize(void);
static void finalize(void);
static void reset_dev1(void);
static void reset_dev2(void);
static void print_menu(void);
static void do_test_libio(void);

/*****************************************************************/

static void get_prod_info(void)
{
  IO_LIBIO_PROD_INFO prod_info;

  if (io_test_get_libio_prod_info(&prod_info)) {
    printf(TEST_LIBIO_ERROR_MSG);
    return;
  }
  printf("LIBIO prod num: %s\n", prod_info.prod_num);
  printf("LIBIO rstate  : %s\n", prod_info.rstate);
}

/*****************************************************************/

static void get_last_error(void)
{
  IO_LIB_STATUS status;
  IO_ERROR_STRING error_string;

  if (io_get_last_error(&status)) {
    printf(TEST_LIBIO_ERROR_MSG);
    return;
  }

  if (io_get_error_string(status.error_code, error_string)) {
    printf(TEST_LIBIO_ERROR_MSG);
    return;
  }

  switch (status.error_source) {
  case IO_INTERNAL_ERROR:
    printf("LIBIO error source : IO_INTERNAL_ERROR\n");
    break;
  case IO_LINUX_ERROR:
    printf("LIBIO error source : IO_LINUX_ERROR\n");
    break;
  default:
    printf("LIBIO error source : *** UNKNOWN\n");
  }
  printf("LIBIO error code   : %ld\n", status.error_code);
  printf("LIBIO error string : %s\n",  error_string);
}

/*****************************************************************/

static void initialize(void)
{
  if (io_initialize() != IO_SUCCESS) {
    printf(TEST_LIBIO_ERROR_MSG);
    return;
  }
}

/*****************************************************************/

static void finalize(void)
{
  if (io_finalize() != IO_SUCCESS) {
    printf(TEST_LIBIO_ERROR_MSG);
  }
}

/*****************************************************************/

static void reset_dev1(void)
{
  if (io_reset_dev1() != IO_SUCCESS) {
    printf(TEST_LIBIO_ERROR_MSG);
  }
}

/*****************************************************************/

static void reset_dev2(void)
{
  if (io_reset_dev2() != IO_SUCCESS) {
    printf(TEST_LIBIO_ERROR_MSG);
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
  printf("  5. reset DEV1\n");
  printf("  6. reset DEV2\n");
  printf("100. Exit\n\n");
}

/*****************************************************************/

static void do_test_libio(void)
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
      reset_dev1();
      break;
    case 6:
      reset_dev2();
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
  do_test_libio();

  printf("Goodbye!\n");
  return 0;
}
