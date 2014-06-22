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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cmd_queue.h"

using namespace std;

// Message sent to thread functions
typedef struct {
  char info[50];
  int commands;
  cmd_queue *cmd_fifo;
} THREAD_FUNC_MSG;

// Global variables
static bool g_cmd_thread_done = false;

// Thread function prototypes
static void *cmd_thread_func(void *ptr);

////////////////////////////////////////////////////////////////

static void *cmd_thread_func(void *ptr)
{
  THREAD_FUNC_MSG *msg = (THREAD_FUNC_MSG *) ptr;

  CMD_QUEUE_ITEM item;
  cmd_queue *cmd_fifo = msg->cmd_fifo;
  unsigned fifo_size;

  printf("%s - Started, expected commands=%d\n",
	 msg->info, msg->commands);

  for (int i=0; i < msg->commands; i++) {
    cmd_fifo->size(fifo_size);
    printf("size=%u\n", fifo_size);
    cmd_fifo->recv(item, true);
    printf("cmd=%s, value=%d\n",
	   item.cmd.c_str(), item.value);  
  }

  // Test of timeout (shall timeout)
  printf("%s - All commands received, wait using timeout(5s)\n",
	 msg->info);
  if (cmd_fifo->recv(item, 5.0) == CMD_QUEUE_TIMEDOUT) {
    printf("Timeout occurred\n");
  } else {
    printf("*** No Timeout occurred\n");
  };
  printf("%s - All commands received, wait using timeout(2s)\n",
	 msg->info);
  if (cmd_fifo->recv(item, 2.0) == CMD_QUEUE_TIMEDOUT) {
    printf("Timeout occurred\n");
  } else {
    printf("*** No Timeout occurred\n");
  };

  g_cmd_thread_done = true; // Signal main

  // Test of timeout (shall NOT timeout)
  // Main is sending one last command
  printf("%s - All commands received, wait using timeout(5s)\n",
	 msg->info);
  if (cmd_fifo->recv(item, 5.0) == CMD_QUEUE_SUCCESS) {
    printf("cmd=%s, value=%d\n",
	   item.cmd.c_str(), item.value);
  } else {
    printf("*** Error receiving last command\n");
  };

  return NULL;
}

////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  int commands = 5;
  long rc;
  
  CMD_QUEUE_ITEM item;
  cmd_queue *cmd_fifo = new cmd_queue();
  unsigned fifo_size;

  // Check arguments
  if (argc == 2) {
    commands = atoi(argv[1]);
  }

  // Add dummy commands to fifo
  printf("Add dummy commands to fifo\n");
  item.cmd   = "apa";
  item.value = 123;
  cmd_fifo->send(item);

  item.cmd   = "bepa";
  item.value = 456;
  cmd_fifo->send(item);

  cmd_fifo->size(fifo_size);
  printf("fifo size=%u\n", fifo_size);

  // Clear fifo
  printf("clear fifo...\n");
  cmd_fifo->clear();
  cmd_fifo->size(fifo_size);
  printf("fifo size=%u\n", fifo_size);

  // Add initial commands to fifo
  printf("Add initial commands to fifo\n");
  item.cmd   = "aaaa";
  item.value = 1111;
  cmd_fifo->send(item);

  item.cmd   = "bbbb";
  item.value = 2222;
  cmd_fifo->send(item);

  cmd_fifo->size(fifo_size);
  printf("fifo size=%u\n", fifo_size);

  // Create the command thread (fifo consumer)
  THREAD_FUNC_MSG cmd_thread_msg = {"COMMAND_thread",
				    commands + (int)fifo_size,
				    cmd_fifo
  };

  pthread_t cmd_thread;

  rc = pthread_create(&cmd_thread,
		      NULL,
		      cmd_thread_func,
		      (void*)&cmd_thread_msg );
  if (rc) {
    perror("*** ERROR : pthread_create(COMMAND)");
    delete cmd_fifo;
    return 1;
  }

  // Add commands to fifo
  for (int i=0; i < commands; i++) {
    char cmd_name[50];
    sprintf(cmd_name, "command-%04d", i);
    item.cmd   = cmd_name;
    item.value = i;

    cmd_fifo->send(item);

    sleep(1);
  }

  // Wait for command thread to process all commands
  while (!g_cmd_thread_done) {
    sleep(1);
  }
  item.cmd   = "cccc";
  item.value = 3333;
  cmd_fifo->send(item);

  // Wait for command thread to complete
  rc = pthread_join(cmd_thread, NULL);
  if (rc) {
    perror("*** ERROR : pthread_join(COMMAND)");
    delete cmd_fifo;
    return 1;
  }

  // Fifo should be empty
  cmd_fifo->size(fifo_size);
  printf("fifo size=%u\n", fifo_size);

  if (cmd_fifo->recv(item,false) == CMD_QUEUE_WOULD_BLOCK) {
    printf("FIFO empty\n");
  }
  else {
    printf("*** FIFO not empty\n");
  }

  delete cmd_fifo;

  return 0;
}
