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

#ifndef __ITEM_QUEUE_H__
#define __ITEM_QUEUE_H__

#include <string>

#include "msg_fifo_queue.h"
#include "item.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definitions of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define ITEM_QUEUE_SUCCESS       0
#define ITEM_QUEUE_FAILURE      -1
#define ITEM_QUEUE_WOULD_BLOCK  -2
#define ITEM_QUEUE_TIMEDOUT     -3

/////////////////////////////////////////////////////////////////////////////
//               Class support types
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class item_queue
{
 public:
  item_queue(void);
  ~item_queue(void);

  long send(const item *ptr);

  long recv(item *&ptr,
	    bool wait);

  long recv(item *&ptr,
	    double timeout_in_sec);

  long size(unsigned &value);

  long clear(void);

 private:
  msg_fifo_queue<item *> m_queue;
};

#endif // __ITEM_QUEUE_H__
