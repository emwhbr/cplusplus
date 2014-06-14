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

#ifndef __MSG_FIFO_QUEUE_H__
#define __MSG_FIFO_QUEUE_H__

#include <pthread.h>
#include <queue>
#include <deque>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define MFQ_SUCCESS        0
#define MFQ_MUTEX_ERROR   -1
#define MFQ_SIGNAL_ERROR  -2
#define MFQ_WOULD_BLOCK   -3

// Flag values
#define MFQ_FLAG_NONBLOCK  0x01

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

template <class T> 
class msg_fifo_queue
{
 public:
  msg_fifo_queue(void);
  ~msg_fifo_queue(void);

  long push(const T &msg);

  long pop(T &msg, unsigned flag);

  long nr_elements(unsigned &value);

 private:
  queue<T, deque<T> > m_queue; // Underlaying container is deque
  pthread_mutex_t     m_queue_mutex;
  pthread_cond_t      m_queue_condv; 
};

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

template <class T>
msg_fifo_queue<T>::msg_fifo_queue(void)
{
  pthread_mutex_init(&m_queue_mutex, NULL); // Use default mutex attributes
  pthread_cond_init(&m_queue_condv, NULL);  // Use default condition attributes
}

////////////////////////////////////////////////////////////////

template <class T>
msg_fifo_queue<T>::~msg_fifo_queue(void)
{
  pthread_mutex_destroy(&m_queue_mutex);
  pthread_cond_destroy(&m_queue_condv);
}

////////////////////////////////////////////////////////////////

template <class T>
long msg_fifo_queue<T>::push(const T &msg)
{
  // Lockdown queue
  if (pthread_mutex_lock(&m_queue_mutex)) {
    return MFQ_MUTEX_ERROR;
  }

  // Copy element to end of queue,
  // oldest element is first in queue
  m_queue.push(msg);

  // Notify waiting thread
  if (pthread_cond_signal(&m_queue_condv)) {
    pthread_mutex_unlock(&m_queue_mutex);
    return MFQ_SIGNAL_ERROR;
  }

  // Lockup queue
  if (pthread_mutex_unlock(&m_queue_mutex)) {
    return MFQ_MUTEX_ERROR;
  }

  return MFQ_SUCCESS;
}

////////////////////////////////////////////////////////////////

template <class T>
long msg_fifo_queue<T>::pop(T &msg, unsigned flag)
{
  // Lockdown queue
  if (pthread_mutex_lock(&m_queue_mutex)) {
    return MFQ_MUTEX_ERROR;
  }

  // Check if queue is empty
  if ( m_queue.empty() && (flag & MFQ_FLAG_NONBLOCK) ) {  
    // Lockup queue
    if (pthread_mutex_unlock(&m_queue_mutex)) {
      return MFQ_MUTEX_ERROR;
    }
    return MFQ_WOULD_BLOCK;
  }

  // While queue is empty, make calling thread wait
  while ( m_queue.empty() ) {
    if (pthread_cond_wait(&m_queue_condv, &m_queue_mutex)) {
      pthread_mutex_unlock(&m_queue_mutex);
      return MFQ_SIGNAL_ERROR;
    }
  }

  // Copy element from beginning of queue (oldest element)
  msg = m_queue.front();

  // Remove this element from queue
  m_queue.pop();

  // Lockup queue
  if (pthread_mutex_unlock(&m_queue_mutex)) {
    return MFQ_MUTEX_ERROR;
  }

  return MFQ_SUCCESS;
}

////////////////////////////////////////////////////////////////

template <class T>
long msg_fifo_queue<T>::nr_elements(unsigned &value)
{
  // Lockdown queue
  if (pthread_mutex_lock(&m_queue_mutex)) {
    return MFQ_MUTEX_ERROR;
  }

  value = m_queue.size();

  // Lockup queue
  if (pthread_mutex_unlock(&m_queue_mutex)) {
    return MFQ_MUTEX_ERROR;
  }

  return MFQ_SUCCESS;
}

#endif // __MSG_FIFO_QUEUE_H__
