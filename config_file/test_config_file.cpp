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

#include "test_cfg_file.h"

////////////////////////////////////////////////////////////////

static int print_config_values(const char *file)
{
  long rc;
  test_cfg_file *cfg_f = new test_cfg_file(file);

  // Parse configuration file
  rc = cfg_f->parse();
  if ( (rc != CFG_FILE_SUCCESS) &&         // Use values from file
       (rc != CFG_FILE_FILE_NOT_FOUND) ) { // Use default values
    delete cfg_f;
    switch(rc) {
    case CFG_FILE_FILE_IO_ERROR:
      printf("I/O error parsing config file %s\n", file);
      break;
    case CFG_FILE_BAD_FILE_FORMAT:
      printf("Bad file format parsing config file %s\n", file);
      break;
    case CFG_FILE_BAD_VALUE_FORMAT:
      printf("Bad value format parsing config file %s\n", file);
      break;
    default:
      printf("Unexpected error(%ld) parsing config file %s\n", rc, file);
    }
    return 0;
  }

  if (rc == CFG_FILE_SUCCESS) {
    printf("File '%s' found, values:\n", file);
  }
  else {
    printf("File '%s' NOT found, using default values:\n", file);
  }

  // Get configuration values
  bool cfg_ok = true;
  bool the_bool;
  rc = cfg_f->get_bool(the_bool);
  if (rc != CFG_FILE_SUCCESS) {
    cfg_ok = false;
    printf("Unexpected error(%ld) get_bool\n", rc);
  }
  string the_string;
  rc = cfg_f->get_string(the_string);
  if (rc != CFG_FILE_SUCCESS) {
    cfg_ok = false;
    printf("Unexpected error(%ld) get_string\n", rc);
  }
  double the_double;
  rc = cfg_f->get_double(the_double);
  if (rc != CFG_FILE_SUCCESS) {
    cfg_ok = false;
    printf("Unexpected error(%ld) get_double\n", rc);
  }
  int the_int_hex;
  rc = cfg_f->get_int_hex(the_int_hex);
  if (rc != CFG_FILE_SUCCESS) {
    cfg_ok = false;
    printf("Unexpected error(%ld) get_int_hex\n", rc);
  }
  int the_int_dec;
  rc = cfg_f->get_int_dec(the_int_dec);
  if (rc != CFG_FILE_SUCCESS) {
    cfg_ok = false;
    printf("Unexpected error(%ld) get_int_dex\n", rc);
  }  
  if (!cfg_ok) {
    delete cfg_f;
    return 0;
  }
  
  // Print configuration values
  printf("\tbool_test   :%s\n", (the_bool ? "TRUE" : "FALSE"));
  printf("\tstring_test :%s\n", the_string.c_str());
  printf("\tdouble_test :%lf\n", the_double);
  printf("\tint_hex_test:0x%x\n", the_int_hex);
  printf("\tint_dec_test:%d\n", the_int_dec);

  return 1;
}

////////////////////////////////////////////////////////////////

int main(void)
{
  printf("Trying existing file\n");
  print_config_values("test.cfg");

  printf("\nTrying non-existing file\n");
  print_config_values("non-exisiting-test.cfg");

  return 0;
}
