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

#include <sstream>

#include "reporter.h"


////////////////////////////////////////////////////////////////

int main(void)
{
  ostringstream oss_msg;
  int cnt = 0;

  // Default is debug ON, with output enabled
  oss_msg << "PUT_LINE: cnt=" << ++cnt;
  PUT_LINE(oss_msg.str().c_str());
  oss_msg.str("");;

  oss_msg << "PUT: cnt=" << ++cnt;
  PUT(oss_msg.str().c_str());
  oss_msg.str("");;

  oss_msg << ", PUT: cnt=" << ++cnt;
  PUT(oss_msg.str().c_str());
  oss_msg.str("");;

  PUT_LINE("");

  // Set debug OFF, no output
  SET_DEBUG(false);

  std::cout << "... skipping output from reporter ..." << std::endl;

  // No output  
  oss_msg << "PUT_LINE: cnt=" << ++cnt;
  PUT_LINE(oss_msg.str().c_str());
  oss_msg.str("");;

  oss_msg << "PUT: cnt=" << ++cnt;
  PUT(oss_msg.str().c_str());
  oss_msg.str("");;

  oss_msg << ", PUT: cnt=" << ++cnt;
  PUT(oss_msg.str().c_str());
  oss_msg.str("");;

  PUT_LINE("");

  // Force output
  oss_msg << "PUT_LINE_ALWAYS: cnt=" << ++cnt;
  PUT_LINE_ALWAYS(oss_msg.str().c_str());
  oss_msg.str("");;

  oss_msg << "PUT_ALWAYS: cnt=" << ++cnt;
  PUT_ALWAYS(oss_msg.str().c_str());
  oss_msg.str("");;

  oss_msg << ", PUT_ALWAYS: cnt=" << ++cnt;
  PUT_ALWAYS(oss_msg.str().c_str());
  oss_msg.str("");;

  PUT_LINE_ALWAYS("");

  std::cout << "cnt=" << cnt << ", expected=9" << std::endl;
}
