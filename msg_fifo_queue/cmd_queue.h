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

#ifndef __CMD_QUEUE_H__
#define __CMD_QUEUE_H__

#include <string>

#include "msg_fifo_queue.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definitions of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define CMD_QUEUE_SUCCESS       0
#define CMD_QUEUE_FAILURE      -1
#define CMD_QUEUE_WOULD_BLOCK  -2

/////////////////////////////////////////////////////////////////////////////
//               Class support types
/////////////////////////////////////////////////////////////////////////////
typedef struct {
  string cmd;
  int    value;
} CMD_QUEUE_ITEM;

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class cmd_queue
{
 public:
  cmd_queue(void);
  ~cmd_queue(void);

  long send(const CMD_QUEUE_ITEM &item);

  long recv(CMD_QUEUE_ITEM &item,
	    bool wait);

  long size(unsigned &value);

 private:
  msg_fifo_queue<CMD_QUEUE_ITEM> m_queue;
};

#endif // __CMD_QUEUE_H__
