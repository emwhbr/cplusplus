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

#include "cmd_queue.h"

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

cmd_queue::cmd_queue(void)
{
}

////////////////////////////////////////////////////////////////

cmd_queue::~cmd_queue(void)
{
}

////////////////////////////////////////////////////////////////

long cmd_queue::send(const CMD_QUEUE_ITEM &item)
{
  if (m_queue.push(item) != MFQ_SUCCESS) {
    return CMD_QUEUE_FAILURE;
  }

  return CMD_QUEUE_SUCCESS;
}

////////////////////////////////////////////////////////////////

long cmd_queue::recv(CMD_QUEUE_ITEM &item,
		     bool wait)
{
  long rc;
  unsigned flag = (wait ? 0 : MFQ_FLAG_NONBLOCK);

  rc = m_queue.pop(item, flag);
  if (rc != MFQ_SUCCESS) {
    if (rc == MFQ_WOULD_BLOCK) { 
      return CMD_QUEUE_WOULD_BLOCK;
    }
    else {
      return CMD_QUEUE_FAILURE;
    }
  }

  return CMD_QUEUE_SUCCESS;
}

////////////////////////////////////////////////////////////////

long cmd_queue::recv(CMD_QUEUE_ITEM &item,
		     double timeout_in_sec)
{
  long rc;

  rc = m_queue.pop_timed(item, timeout_in_sec);
  if (rc != MFQ_SUCCESS) {
    if (rc == MFQ_TIMEDOUT) { 
      return CMD_QUEUE_TIMEDOUT;
    }
    else {
      return CMD_QUEUE_FAILURE;
    }
  }

  return CMD_QUEUE_SUCCESS;
}

////////////////////////////////////////////////////////////////

long cmd_queue::size(unsigned &value)
{
  if (m_queue.nr_elements(value) != MFQ_SUCCESS) {
    return CMD_QUEUE_FAILURE;
  }

  return CMD_QUEUE_SUCCESS;
}

////////////////////////////////////////////////////////////////

long cmd_queue::clear(void)
{
  if (m_queue.clear() != MFQ_SUCCESS) {
    return CMD_QUEUE_FAILURE;
  }

  return CMD_QUEUE_SUCCESS;
}
