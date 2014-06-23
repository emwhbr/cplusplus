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

#include "item_queue.h"

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

item_queue::item_queue(void)
{
}

////////////////////////////////////////////////////////////////

item_queue::~item_queue(void)
{
}

////////////////////////////////////////////////////////////////

long item_queue::send(const item *ptr)
{
  if (m_queue.push((item *)ptr) != MFQ_SUCCESS) {
    return ITEM_QUEUE_FAILURE;
  }

  return ITEM_QUEUE_SUCCESS;
}

////////////////////////////////////////////////////////////////

long item_queue::recv(item *&ptr,
		      bool wait)
{
  long rc;
  unsigned flag = (wait ? 0 : MFQ_FLAG_NONBLOCK);

  rc = m_queue.pop(ptr, flag);
  if (rc != MFQ_SUCCESS) {
    if (rc == MFQ_WOULD_BLOCK) { 
      return ITEM_QUEUE_WOULD_BLOCK;
    }
    else {
      return ITEM_QUEUE_FAILURE;
    }
  }

  return ITEM_QUEUE_SUCCESS;
}

////////////////////////////////////////////////////////////////

long item_queue::recv(item *&ptr,
		      double timeout_in_sec)
{
  long rc;

  rc = m_queue.pop_timed(ptr, timeout_in_sec);
  if (rc != MFQ_SUCCESS) {
    if (rc == MFQ_TIMEDOUT) { 
      return ITEM_QUEUE_TIMEDOUT;
    }
    else {
      return ITEM_QUEUE_FAILURE;
    }
  }

  return ITEM_QUEUE_SUCCESS;
}

////////////////////////////////////////////////////////////////

long item_queue::size(unsigned &value)
{
  if (m_queue.nr_elements(value) != MFQ_SUCCESS) {
    return ITEM_QUEUE_FAILURE;
  }

  return ITEM_QUEUE_SUCCESS;
}

////////////////////////////////////////////////////////////////

long item_queue::clear(void)
{
  unsigned queue_size;
  item *item_ptr = NULL;

  // Free all memory, pointed to by pointers in queue
  if (this->size(queue_size) != ITEM_QUEUE_SUCCESS) {
    return ITEM_QUEUE_FAILURE;
  }
  while (queue_size)  {    
    if (this->recv(item_ptr, true) != ITEM_QUEUE_SUCCESS) {
      return ITEM_QUEUE_FAILURE;
    }

    delete item_ptr; // Free memory pointed to by this elements

    if (this->size(queue_size) != ITEM_QUEUE_SUCCESS) {
      return ITEM_QUEUE_FAILURE;
    }
  }

  return ITEM_QUEUE_SUCCESS;
}
