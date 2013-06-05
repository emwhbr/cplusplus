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
#include <stdint.h>
#include <pthread.h>
#include <strings.h>
#include <memory>

#include "fifo_queue.h"

using namespace std;

// The macros
#define FQ1_MSG_MAX_LEN  65536
#define FQ2_STR_LEN      50

#define TEST_LOOPS  500

// Fifo queue message types
typedef struct {
  uint16_t data_index;
  uint8_t  data[FQ1_MSG_MAX_LEN];
} FQ1_MESSAGE; // Master -> Slave

typedef struct {
  char message[FQ2_STR_LEN];
} FQ2_MESSAGE; // Slave -> Master

// Message sent to thread functions
typedef struct {
  char info[50];
  auto_ptr<fifo_queue> *in_fq;
  auto_ptr<fifo_queue> *out_fq;
} THREAD_FUNC_MSG;

// Thread function prototypes
static void *master_thread_func(void *ptr);
static void *slave_thread_func(void *ptr);


////////////////////////////////////////////////////////////////

static void *master_thread_func(void *ptr)
{
  THREAD_FUNC_MSG *msg = (THREAD_FUNC_MSG *) ptr;

  printf("%s : fifo queue(IN)=%s, fifo queue(OUT)=%s\n",
	 msg->info,
	 msg->in_fq[0]->get_name().c_str(),
	 msg->out_fq[0]->get_name().c_str());

  // Extract IN / OUT fifo queues
  auto_ptr<fifo_queue> *in_fq  = msg->in_fq;
  auto_ptr<fifo_queue> *out_fq = msg->out_fq;

  FQ1_MESSAGE out_msg;
  FQ2_MESSAGE in_msg;

  bool would_block = false;

  ///////////////////////////
  // Check status of queues
  /////////////////////////// 
  (*out_fq)->put_would_block(would_block);
  printf("%s : Put would block = %s\n",
	 msg->info, (would_block ? "TRUE" : "FALSE"));

  (*in_fq)->get_would_block(would_block);
  printf("%s : Get would block = %s\n",
	 msg->info, (would_block ? "TRUE" : "FALSE"));

  ///////////////////////////////////////////////
  // Produce and consume data using fifo queues
  ///////////////////////////////////////////////
  for (unsigned i=0; i < TEST_LOOPS; i++) {

    bzero((void *) &in_msg, sizeof(in_msg));
    bzero((void *) &out_msg, sizeof(out_msg));

    // Produce data to slave
    out_msg.data_index = i;
    out_msg.data[i]    = i;

    if ( (*out_fq)->put(&out_msg, 0) != FIFO_QUEUE_SUCCESS ) {
      printf("%s : *** ERROR (put)\n", msg->info);
      return NULL;
    }

    // Consume data from slave
    if ( (*in_fq)->get(&in_msg, 0) != FIFO_QUEUE_SUCCESS ) {
      printf("%s : *** ERROR (put)\n", msg->info);
      return NULL;
    }

    printf("%s : From slave => \"%s\"\n",
	   msg->info, in_msg.message);
  }

  ////////////////////////////
  // Check status of queues
  ////////////////////////////  
  (*out_fq)->put_would_block(would_block);  
  printf("%s : Put would block = %s\n",
	 msg->info, (would_block ? "TRUE" : "FALSE"));

  (*in_fq)->get_would_block(would_block); 
  printf("%s : Get would block = %s\n",
	 msg->info, (would_block ? "TRUE" : "FALSE"));
  
  ///////////////////////////////
  // Try non-blocking operation
  ///////////////////////////////

  // This should block
  if ( (*in_fq)->get(&in_msg, FIFO_QUEUE_NONBLOCK) != FIFO_QUEUE_WOULD_BLOCK ) {
    printf("%s : *** ERROR (put)\n", msg->info);
    return NULL;
  }
  
  return NULL;
}

////////////////////////////////////////////////////////////////

static void *slave_thread_func(void *ptr)
{
  THREAD_FUNC_MSG *msg = (THREAD_FUNC_MSG *) ptr;

  printf("%s : fifo queue(IN)=%s, fifo queue(OUT)=%s\n",
	 msg->info,
	 msg->in_fq[0]->get_name().c_str(),
	 msg->out_fq[0]->get_name().c_str());

  // Extract IN / OUT fifo queues
  auto_ptr<fifo_queue> *in_fq  = msg->in_fq;
  auto_ptr<fifo_queue> *out_fq = msg->out_fq;

  FQ1_MESSAGE in_msg;
  FQ2_MESSAGE out_msg;

  ///////////////////////////////////////////////
  // Produce and consume data using fifo queues
  ///////////////////////////////////////////////
  for (unsigned i=0; i < TEST_LOOPS; i++) {

    bzero((void *) &in_msg, sizeof(in_msg));
    bzero((void *) &out_msg, sizeof(out_msg));

    // Consume data from master
    if ( (*in_fq)->get(&in_msg, 0) != FIFO_QUEUE_SUCCESS ) {
      printf("%s : *** ERROR (put)\n", msg->info);
      return NULL;
    }

    // Produce data to master
    sprintf(out_msg.message, "index(%05d) : %#x",
	    in_msg.data_index, in_msg.data[in_msg.data_index]);

    if ( (*out_fq)->put(&out_msg, 0) != FIFO_QUEUE_SUCCESS ) {
      printf("%s : *** ERROR (put)\n", msg->info);
      return NULL;
    }
  }

  ///////////////////////////////
  // Try non-blocking operation
  ///////////////////////////////

  // This should block
  if ( (*in_fq)->get(&in_msg, FIFO_QUEUE_NONBLOCK) != FIFO_QUEUE_WOULD_BLOCK ) {
    printf("%s : *** ERROR (put)\n", msg->info);
    return NULL;
  }

  return NULL;
}

////////////////////////////////////////////////////////////////

int main(void)
{
  int rc;

  auto_ptr<fifo_queue> fq1_auto;
  auto_ptr<fifo_queue> fq2_auto;

  printf("MAIN : Test start\n");

  // Create the fifo queues with garbage collector
  fifo_queue *fq1 = new fifo_queue("FQ1", sizeof(FQ1_MESSAGE), 10);
  fq1_auto = auto_ptr<fifo_queue>(fq1);

  fifo_queue *fq2 = new fifo_queue("FQ2", sizeof(FQ2_MESSAGE), 20);
  fq2_auto = auto_ptr<fifo_queue>(fq2);

  // Create the threads
  THREAD_FUNC_MSG master_msg = {"MASTER-thread",
				&fq2_auto, // IN
				&fq1_auto  // OUT
  };
  THREAD_FUNC_MSG slave_msg = {"SLAVE-thread",
			       &fq1_auto, // IN
			       &fq2_auto  // OUT
  };

  pthread_t master_thread;
  pthread_t slave_thread;

  rc = pthread_create(&master_thread,
		      NULL,
		      master_thread_func, (void*)&master_msg );
  if (rc) {
    perror("*** ERROR : pthread_create(MASTER)");
    return 1;
  }

  rc = pthread_create(&slave_thread,
		      NULL,
		      slave_thread_func, (void*)&slave_msg );
  if (rc) {
    perror("*** ERROR : pthread_create(SLAVE)");
    return 1;
  }

  // Wait for threads to complete
  rc = pthread_join(master_thread, NULL);
  if (rc) {
    perror("*** ERROR : pthread_join(MASTER)");
    return 1;
  }
  rc = pthread_join(slave_thread, NULL);
  if (rc) {
    perror("*** ERROR : pthread_join(SLAVE)");
    return 1;
  }

  printf("MAIN : Test done\n");

  return 0;
}
