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

#ifndef __FIFO_QUEUE_H__
#define __FIFO_QUEUE_H__

#include <stdint.h>
#include <semaphore.h>
#include <string>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Declaration of macros
/////////////////////////////////////////////////////////////////////////////

// Return codes
#define FIFO_QUEUE_SUCCESS      0
#define FIFO_QUEUE_FAILURE     -1
#define FIFO_QUEUE_CLOSED      -2
#define FIFO_QUEUE_WOULD_BLOCK -3

// Flag values
#define FIFO_QUEUE_NONBLOCK  0x01

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class fifo_queue {

 public:
  fifo_queue(string queue_name,
	     unsigned element_size,
	     unsigned max_elements);
  ~fifo_queue(void);
  
  long put(const void *data, unsigned flag); // Shall only be called by 
                                             // producer thread

  long get(void *data, unsigned flag); // Shall only be called by 
                                       // consumer thread

  long put_would_block(bool &value); // Called by producer to check if room
                                     // for new data in queue (avoid blocking)

  long get_would_block(bool &value); // Called by consumer to check if
                                     // any data in queue (avoid blocking)

  long close(void); // Queue is closed and any consumer/producer
                    // blocking on queue will be unblocked.

  string get_name(void);

 private:
  string   m_queue_name;
  unsigned m_element_size;
  unsigned m_max_elements;

  uint8_t   **m_queue; // The queue
  unsigned  m_w_pos;   // Current WRITE position
  unsigned  m_r_pos;   // Current READ position
  bool      m_closed;

  sem_t  m_sem_queue;    // Protects access to queue
  sem_t  m_sem_consumer; // Released by consumer, claimed by producer
  sem_t  m_sem_producer; // Released by producer, claimed by consumer  

  void init_members(void);
};

#endif // FIFO_QUEUE
