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
#include <iostream>
#include <string>

#include "shell_cmd.h"

using namespace std;

////////////////////////////////////////////////////////////////

int main(void)
{
  shell_cmd sc;

  string cmd;
  string output;
  int exit_status;
  bool found;

  // This command should be found
  cmd.clear();
  cmd  = "ping";
  cout << "+++++++++++++++++++++++++++++++++++++++\n";
  cout << "Command: " << cmd << endl;
  if ( sc.check_command(cmd, found) != SHELL_CMD_SUCCESS) {
    cout << "*** Error 1\n";
    return 1;
  }
  printf("Found: %s\n", (found ? "YES" : "NO"));

  // This command should not be found
  cmd.clear();
  cmd  = "/usr/bin/this_is_a_non_existing_command";
  cout << "+++++++++++++++++++++++++++++++++++++++\n";
  cout << "Command: " << cmd << endl;
  if ( sc.check_command(cmd, found) != SHELL_CMD_SUCCESS) {
    cout << "*** Error 2\n";
    return 1;
  }
  printf("Found: %s\n", (found ? "YES" : "NO"));

  // This should pass
  cmd.clear();
  output.clear();
  cmd  = "ping -c 3 8.8.8.8";
  cout << "+++++++++++++++++++++++++++++++++++++++\n";
  cout << "Command: " << cmd << endl;
  if ( sc.execute(cmd, output) != SHELL_CMD_SUCCESS) {
    cout << "*** Error 3\n";
    return 1;
  }
  cout << "Output:\n" << output << endl;

  // This should fail (visible in return code)
  cmd.clear();
  output.clear();
  cmd  = "/usr/bin/this_is_a_non_existing_command a b c";
  cout << "+++++++++++++++++++++++++++++++++++++++\n";
  cout << "Command: " << cmd << endl;
  if ( sc.execute(cmd, output) != SHELL_CMD_NOT_FOUND) {
    cout << "*** Error 4\n";
    return 1;
  }

  // This should pass
  cmd.clear();
  cmd  = "ping -c 3 8.8.8.8";
  cout << "+++++++++++++++++++++++++++++++++++++++\n";
  cout << "Command: " << cmd << endl;
  if ( sc.execute(cmd, exit_status) != SHELL_CMD_SUCCESS) {
    cout << "*** Error 5\n";
    return 1;
  }
  cout << "Exit status: " << exit_status << endl;

  // This should fail (visible in return code)
  cmd.clear();
  cmd  = "/usr/bin/this_is_a_non_existing_command a b c";
  cout << "+++++++++++++++++++++++++++++++++++++++\n";
  cout << "Command: " << cmd << endl;
  if ( sc.execute(cmd, exit_status) != SHELL_CMD_NOT_FOUND) {
    cout << "*** Error 6\n";
    return 1;
  }

  // This should fail (visible in no output)
  cmd.clear();
  output.clear();
  cmd  = "ls /non-existing-dir";
  cout << "+++++++++++++++++++++++++++++++++++++++\n";
  cout << "Command: " << cmd << endl;
  if ( sc.execute(cmd, output) != SHELL_CMD_SUCCESS) {
    cout << "*** Error 7\n";
    return 1;
  }
  cout << "Output:\n" << output << endl;

  // This should fail (visible in exit status)
  cmd.clear();
  cmd  = "ls /non-existing-dir";
  cout << "+++++++++++++++++++++++++++++++++++++++\n";
  cout << "Command: " << cmd << endl;
  if ( sc.execute(cmd, exit_status) != SHELL_CMD_SUCCESS) {
    cout << "*** Error 8\n";
    return 1;
  }
  cout << "Exit status: " << exit_status << endl;

  return 0;
}
