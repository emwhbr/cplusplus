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

#include <stdlib.h>
#include <iostream>

#include "cmd_line.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//            The global variables for command line arguments
/////////////////////////////////////////////////////////////////////////////

static string   g_remote_ip_addr;
static unsigned g_local_port;
static unsigned g_remote_port;

static float g_delay;

static bool g_verbose;


////////////////////////////////////////////////////////////////

static void print_usage(void)
{
  cout << "\nUsage: test_cmd_line <options>\n" << endl;
  cout << 
    "   Where <options> are according to:\n\n"

    "    -tcp <remote_ip_addr> <remote_port> <local_port>\n"
    "        <remote_ip_addr>\n"
    "            Remote Ip address\n"
    "            Default address is 192.168.100.17\n"
    "        <remote_port>\n"
    "            Remote TCP port number\n"
    "            Default port is 24000\n"
    "        <local_port>\n"
    "            Local TCP port number\n"
    "            Default port is 14000\n\n"

    "    -delay <interval>\n"
    "        Delay interval\n"
    "        Default delay interval is 0.1 seconds\n\n"
    
    "    -v\n"
    "        Verbose mode, prints additional debug\n"
    "        information during program execution\n\n"

    "    -h\n"
    "        Print this help information\n\n";
}

////////////////////////////////////////////////////////////////

static bool parse_arguments(int argc, char **argv)
{    
  CCmdLine cmd_line;
    
  // Parse argc, argv 
  if (cmd_line.SplitLine(argc, argv) < 1) {

    // No switches were given on the command line
  }
  
  // Test if 'help' switch is present
  if (cmd_line.HasSwitch("-h")) {
      print_usage();
      return false;
  }

  // -tcp
  g_remote_ip_addr = cmd_line.GetSafeArgument(      "-tcp", 0, "192.168.100.17");
  g_remote_port    = atoi( cmd_line.GetSafeArgument("-tcp", 1, "24000").c_str() );
  g_local_port     = atoi( cmd_line.GetSafeArgument("-tcp", 2, "14000").c_str() );

  // -delay
  g_delay = atof( cmd_line.GetSafeArgument("-delay", 0, "0.1").c_str() );

  // -v
  g_verbose = cmd_line.HasSwitch("-v");

  // Assume OK
  return true;
}

////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  // Handle command line arguments
  if ( ! parse_arguments(argc, argv) ) {
    return 1;
  }

  // Print command line arguments
  cout << "------------- COMMAND LINE ARGUMENTS ------------\n\n";

  cout << "g_remote_ip_addr : " << g_remote_ip_addr << endl
       << "g_remote_port    : " << g_remote_port    << endl
       << "g_local_port     : " << g_local_port     << endl
       << "g_delay          : " << g_delay          << endl
       << "g_verbose        : " << g_verbose        << endl;
  
  cout << "\n-------------------------------------------------\n\n";

  return 0;
}
