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

#ifndef __SHELL_CMD_H__
#define __SHELL_CMD_H__

#include <string>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
#define SHELL_CMD_SUCCESS   0
#define SHELL_CMD_FAILURE  -1

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class shell_cmd {

 public:
  shell_cmd(void);
  ~shell_cmd(void);
  
  // No output on stdout
  long execute(const string cmd,
	       string &output);

  // Output on stdout
  long execute(const string cmd,
	       int &exit_status);

 private:
  void init_members(void);

  string get_cmd_no_args(const string cmd);

  long check_cmd_found(const string cmd,
		       bool &found);

  long execute_shell_cmd(const string cmd,
			 int &exit_status);
};

#endif // __SHELL_CMD_H__
