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
#include <iostream>
#include <sstream>
#include <iomanip>

#include "excep.h"

using namespace std;

////////////////////////////////////////////////////////////////

void generate_exception(long source,
			long code,
			string info)
{
  THROW_EXP(source, code, info.c_str());

  cout << "*** Error: This was not supposed to happen\n";
}

////////////////////////////////////////////////////////////////

void test_exception(void)
{
  ostringstream oss_msg;
  int cnt = 1971;

  oss_msg << "Test with cnt=" << ++cnt;
  generate_exception(1, 11, oss_msg.str());
}

////////////////////////////////////////////////////////////////

int main(void)
{
  excep rxp;

  ///////////////////////////////////////////////
  //  Generate an exception
  ///////////////////////////////////////////////
  try {
    test_exception();
  }
  catch (excep &exp) {
    cout << exp.what();
    
    // Save exception
    rxp = exp;
  }
  catch (...) {
    cout << "*** Error: Unexpected exception\n";
    return 1;
  }

  ///////////////////////////////////////////////
  //  Examine the exception
  ///////////////////////////////////////////////

  // Get the stack trace
  STACK_FRAMES frames;
  rxp.get_stack_frames(frames);

  ostringstream oss_msg;
  char buffer[18];

  oss_msg << "\nstack frames:" << (int) frames.active_frames << "\n";

  for (unsigned i=0; i < frames.active_frames; i++) {
    sprintf(buffer, "0x%016lx", frames.frames[i]);
    oss_msg << "\tframe:" << dec << setw(2) << setfill('0') << i
	    << "  addr:" << buffer << "\n";
  }

  // Get info from predefined macros
  oss_msg << "\tViolator: " << rxp.get_file() 
	  << ":" << rxp.get_line()
	  << ", " << rxp.get_function() << "\n";

  // Get the internal info
  oss_msg << "\tSource: " << rxp.get_source()
	  << ", Code: " << rxp.get_code() << "\n";

  oss_msg << "\tInfo: " << rxp.get_info() << "\n";

  // Print all info
  cout << oss_msg.str().c_str() << endl;

  return 0;
}
