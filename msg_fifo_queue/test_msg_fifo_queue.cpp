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
#include "item_queue.h"
#include "item.h"
#include "extended_item.h"

using namespace std;

// Message sent to thread functions
typedef struct {
  char info[50];
  int commands;
  cmd_queue *cmd_fifo;
} CMD_THREAD_FUNC_MSG;

typedef struct {
  char info[50];
  int items;
  item_queue *item_fifo;
} ITEM_THREAD_FUNC_MSG;

// Global variables
static bool g_cmd_thread_done = false;

// Thread function prototypes
static void *cmd_thread_func(void *ptr);
static void *item_thread_func(void *ptr);

////////////////////////////////////////////////////////////////

static void *cmd_thread_func(void *ptr)
{
  CMD_THREAD_FUNC_MSG *msg = (CMD_THREAD_FUNC_MSG *) ptr;

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

static void *item_thread_func(void *ptr)
{
  ITEM_THREAD_FUNC_MSG *msg = (ITEM_THREAD_FUNC_MSG *) ptr;

  item *the_item = NULL;
  extended_item *the_extended_item = NULL;
  item_queue *item_fifo = msg->item_fifo;
  unsigned fifo_size;

  printf("%s - Started, expected items=%d\n",
	 msg->info, msg->items);

  for (int i=0; i < msg->items; i++) {
    item_fifo->size(fifo_size);
    printf("size=%u\n", fifo_size);
    item_fifo->recv(the_item, true);
    printf("id=%u\n", the_item->id);

    the_extended_item = dynamic_cast<extended_item*>(the_item);
    if (the_extended_item) {
      printf("EXTENDED : %u\n", the_extended_item->extended_id);
    }

    delete the_item;
  }

  return NULL;
}

////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  int commands = 5;
  int items = 5;
  unsigned fifo_size;
  long rc;

  // Check arguments
  if (argc == 2) {
    commands = atoi(argv[1]);
    items = commands;
  }

  /////////////////////////////////////////////////
  //  PART 1: FIFO QUEUE OF COPY OF OBJECTS
  /////////////////////////////////////////////////

  printf("-------------- PART 1 -------------------\n");

  CMD_QUEUE_ITEM cmd_item;
  cmd_queue *cmd_fifo = new cmd_queue();  

  // Add dummy commands to fifo
  printf("Add dummy commands to fifo\n");
  cmd_item.cmd   = "apa";
  cmd_item.value = 123;
  cmd_fifo->send(cmd_item);

  cmd_item.cmd   = "bepa";
  cmd_item.value = 456;
  cmd_fifo->send(cmd_item);

  cmd_fifo->size(fifo_size);
  printf("fifo size=%u\n", fifo_size);

  // Clear fifo
  printf("clear fifo...\n");
  cmd_fifo->clear();
  cmd_fifo->size(fifo_size);
  printf("fifo size=%u\n", fifo_size);

  // Add initial commands to fifo
  printf("Add initial commands to fifo\n");
  cmd_item.cmd   = "aaaa";
  cmd_item.value = 1111;
  cmd_fifo->send(cmd_item);

  cmd_item.cmd   = "bbbb";
  cmd_item.value = 2222;
  cmd_fifo->send(cmd_item);

  cmd_fifo->size(fifo_size);
  printf("fifo size=%u\n", fifo_size);

  // Create the command thread (fifo consumer)
  CMD_THREAD_FUNC_MSG cmd_thread_msg = {"COMMAND_thread",
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
    cmd_item.cmd   = cmd_name;
    cmd_item.value = i;

    cmd_fifo->send(cmd_item);

    sleep(1);
  }

  // Wait for command thread to process all commands
  while (!g_cmd_thread_done) {
    sleep(1);
  }
  cmd_item.cmd   = "cccc";
  cmd_item.value = 3333;
  cmd_fifo->send(cmd_item);

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

  if (cmd_fifo->recv(cmd_item,false) == CMD_QUEUE_WOULD_BLOCK) {
    printf("FIFO empty\n");
  }
  else {
    printf("*** FIFO not empty\n");
  }

  delete cmd_fifo;

  /////////////////////////////////////////////////
  //  PART 2: FIFO QUEUE OF OBJECT POINTERS
  /////////////////////////////////////////////////

  printf("-------------- PART 2 -------------------\n");

  item *item_ptr = NULL;
  extended_item *extended_item_ptr = NULL;
  item_queue *item_fifo = new item_queue();

  // Create the item thread (fifo consumer)
  ITEM_THREAD_FUNC_MSG item_thread_msg = {"ITEM_thread",
					  items * 2, // Two items created in each loop
					  item_fifo
  };

  pthread_t item_thread;
  
  rc = pthread_create(&item_thread,
		      NULL,
		      item_thread_func,
		      (void*)&item_thread_msg );
  if (rc) {
    perror("*** ERROR : pthread_create(ITEM)");
    delete item_fifo;
    return 1;
  }

  // Add items to fifo
  for (int i=0; i < items; i++) {
    
    // Base class
    item_ptr = new item();
    item_ptr->id = i + 100;
    item_fifo->send(item_ptr);

    // Inherited class
    extended_item_ptr = new extended_item();
    extended_item_ptr->id = i + 200;
    extended_item_ptr->extended_id = i + 300;
    item_fifo->send(extended_item_ptr);

    sleep(1);
  }

  // Wait for item thread to complete
  rc = pthread_join(item_thread, NULL);
  if (rc) {
    perror("*** ERROR : pthread_join(ITEM)");
    delete item_fifo;
    return 1;
  }

  // Add dummy items to fifo
  printf("Add dummy items to fifo\n");

  item_ptr = new item();
  item_ptr->id = 1000;
  item_fifo->send(item_ptr);
  
  extended_item_ptr = new extended_item();
  extended_item_ptr->id = 2000;
  extended_item_ptr->extended_id = 3000;
  item_fifo->send(extended_item_ptr);

  item_fifo->size(fifo_size);
  printf("fifo size=%u\n", fifo_size);

  // Clear fifo
  printf("clear fifo...\n");
  item_fifo->clear();
  item_fifo->size(fifo_size);
  printf("fifo size=%u\n", fifo_size);

  delete item_fifo;

  return 0;
}
