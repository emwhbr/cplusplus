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
#include <string.h>
#include <errno.h>

#include "fifo_queue.h"

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

fifo_queue::fifo_queue(string queue_name,
		       unsigned element_size,
		       unsigned max_elements)
{
  m_queue_name   = queue_name;
  m_element_size = element_size;
  m_max_elements = max_elements;

#ifdef DEBUG_PRINTS
  printf("Constructor (%s)\n", m_queue_name.c_str());
#endif

  init_members(); 
}

////////////////////////////////////////////////////////////////

fifo_queue::~fifo_queue(void)
{
#ifdef DEBUG_PRINTS
  printf("Destructor (%s)\n", m_queue_name.c_str());
#endif

  // Release semaphore that protect access to queue
  sem_destroy(&m_sem_queue);

  // Release semaphore (consumer & producer)
  sem_destroy(&m_sem_consumer);
  sem_destroy(&m_sem_producer);

  // Free the queue
  if (m_queue) {
    for (unsigned i=0; i < m_max_elements; i++) {
      delete [] m_queue[i];
    }
    delete [] m_queue;
  }
}

////////////////////////////////////////////////////////////////

long fifo_queue::put(const void *data, unsigned flag)
{
  int rc;

  // Check if closed
  if (m_closed) {
    return FIFO_QUEUE_CLOSED;
  }

  // Get access to queue
  rc = sem_wait(&m_sem_queue);
  if (rc) {
    return FIFO_QUEUE_FAILURE;
  }

  rc = sem_trywait(&m_sem_consumer);
  if (rc) {
    if (errno == EAGAIN) {      
      // Release access to queue
      rc = sem_post(&m_sem_queue);
      if (rc) {
	return FIFO_QUEUE_FAILURE;
      }
      // Check if non-blocking operation
      if (flag & FIFO_QUEUE_NONBLOCK) {
	return FIFO_QUEUE_WOULD_BLOCK;
      }
      // Wait for consumer
      rc = sem_wait(&m_sem_consumer);
      if (rc) {
	return FIFO_QUEUE_FAILURE;
      }
      // Get access to queue
      rc = sem_wait(&m_sem_queue);
      if (rc) {
	return FIFO_QUEUE_FAILURE;
      }
    } else {
      return FIFO_QUEUE_FAILURE;
    }
  }

  // Copy data into the queue
  memcpy(m_queue[m_w_pos], data, m_element_size);

  // Update next position to WRITE to
  if (m_w_pos == m_max_elements - 1) {
    m_w_pos = 0;
  } else {
    m_w_pos++;
  }

  // Signal consumer that data is available
  rc = sem_post(&m_sem_producer);
  if (rc) {
    return FIFO_QUEUE_FAILURE;
  }

  // Release access to queue
  rc = sem_post(&m_sem_queue);
  if (rc) {
    return FIFO_QUEUE_FAILURE;
  }

  return FIFO_QUEUE_SUCCESS;
}

////////////////////////////////////////////////////////////////

long fifo_queue::get(void *data, unsigned flag)
{
  int rc;

  // Check if closed
  if (m_closed) {
    return FIFO_QUEUE_CLOSED;
  }
  
  // Get access to queue
  rc = sem_wait(&m_sem_queue);
  if (rc) {
    return FIFO_QUEUE_FAILURE;
  }

  rc = sem_trywait(&m_sem_producer);
  if (rc) {
    if (errno == EAGAIN) {      
      // Release access to queue
      rc = sem_post(&m_sem_queue);
      if (rc) {
	return FIFO_QUEUE_FAILURE;
      }
      // Check if non-blocking operation
      if (flag & FIFO_QUEUE_NONBLOCK) {
	return FIFO_QUEUE_WOULD_BLOCK;
      }
      // Wait for producer
      rc = sem_wait(&m_sem_producer);
      if (rc) {
	return FIFO_QUEUE_FAILURE;
      }
      // Get access to queue
      rc = sem_wait(&m_sem_queue);
      if (rc) {
	return FIFO_QUEUE_FAILURE;
      }
    } else {
      return FIFO_QUEUE_FAILURE;
    }
  }

  // Copy data from the queue to caller
  memcpy(data, m_queue[m_r_pos], m_element_size);

  // Update next position to READ from
  if (m_r_pos == m_max_elements - 1) {
    m_r_pos = 0;
  } else {
    m_r_pos++;
  }

  // Signal producer that data has been consumed
  rc = sem_post(&m_sem_consumer);
  if (rc) {
    return FIFO_QUEUE_FAILURE;
  }

  // Release access to queue
  rc = sem_post(&m_sem_queue);
  if (rc) {
    return FIFO_QUEUE_FAILURE;
  }

  return FIFO_QUEUE_SUCCESS;
}

////////////////////////////////////////////////////////////////

long fifo_queue::put_would_block(bool &value)
{
  int rc;
  int sem_val;

  // Check if data has been consumed
  rc = sem_getvalue(&m_sem_consumer, &sem_val);
  if (rc) {
    return FIFO_QUEUE_FAILURE;
  }

value = (sem_val == 0);

  return FIFO_QUEUE_SUCCESS;
}

////////////////////////////////////////////////////////////////

long fifo_queue::get_would_block(bool &value)
{
  int rc;
  int sem_val;

  // Check if data has been produced
  rc = sem_getvalue(&m_sem_producer, &sem_val);
  if (rc) {
    return FIFO_QUEUE_FAILURE;
  }

  value = (sem_val == 0);

  return FIFO_QUEUE_SUCCESS;
}

////////////////////////////////////////////////////////////////

long fifo_queue::close(void)
{
  int rc;

  m_closed = true;

  // Release access to queue
  rc = sem_post(&m_sem_queue);
  if (rc) {
    return FIFO_QUEUE_FAILURE;
  }
  rc = sem_post(&m_sem_queue);
  if (rc) {
    return FIFO_QUEUE_FAILURE;
  }

  // Release producer
  for (unsigned i=0; i < m_max_elements; i++) {
    rc = sem_post(&m_sem_consumer);
    if (rc) {
      return FIFO_QUEUE_FAILURE;
    }
    
    // Release consumer
    rc = sem_post(&m_sem_producer);
    if (rc) {
      return FIFO_QUEUE_FAILURE;
    }
  }

  return FIFO_QUEUE_SUCCESS;
}

////////////////////////////////////////////////////////////////

string fifo_queue::get_name(void)
{
  return m_queue_name;
}

/////////////////////////////////////////////////////////////////////////////
//               Private member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

void fifo_queue::init_members(void)
{
  m_w_pos  = 0;
  m_r_pos  = 0;
  m_closed = false;

  // Init semaphore that protect access to queue
  sem_init(&m_sem_queue, 0, 1);

  // Init semaphore (consumer)
  sem_init(&m_sem_consumer, 0, m_max_elements);

  // Init semaphore (producer)
  sem_init(&m_sem_producer, 0, 0);

  // Allocate queue (all elements)
  m_queue = new uint8_t * [m_max_elements];
  for (unsigned i=0; i < m_max_elements; i++) {
    m_queue[i] = new uint8_t[m_element_size];
  }
}
