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

#include "bst.h"

/////////////////////////////////////////////////////////////////////////////
//               Types
/////////////////////////////////////////////////////////////////////////////
typedef struct {
  int key;
  int data;
} KEY_DATA;

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////
KEY_DATA g_key_data[] =
  {/* key  data */
    { 50,  0 },
    { 40,  1 },
    { 60,  2 },
    { 45,  3 },
    { 55,  4 },
    { 30,  5 },
    { 25,  6 },
    { 65,  7 },
    { 35,  8 },
    { 70,  9 }
  };

////////////////////////////////////////////////////////////////

int main(void)
{
  bst tree;
  int key;
  int data;

  printf("\nEmpty tree...\n");
  printf("Tree size : %d\n", tree.size());

  printf("\nPopulate tree...\n");
  for (unsigned i=0; i < sizeof(g_key_data)/sizeof(KEY_DATA); i++) {
    tree.insert(g_key_data[i].key,
		g_key_data[i].data);
  }
  printf("Tree size : %d\n", tree.size());

  printf("\nPrint tree...\n");
  tree.dump();

  printf("\nSearch tree...\n");
  key = 25;  // Existing key 
  if (tree.search_data(key, data)) {
    printf("Key(%d) found, data=%d\n", key, data);
  }
  else {
    printf("Key(%d) not found\n", key);
  }
  key = 1000; // Non-existing key
  if (tree.search_data(key, data)) {
    printf("Key(%d) found, data=%d\n", key, data);
  }
  else {
    printf("Key(%d) not found\n", key);
  }

  printf("\nErase tree...\n");
  tree.erase();
  printf("Tree size : %d\n", tree.size());

  printf("\nSearch empty tree...\n");
  key = 25;  // Non-Existing key (tree has been erased) 
  if (tree.search_data(key, data)) {
    printf("Key(%d) found, data=%d\n", key, data);
  }
  else {
    printf("Key(%d) not found\n", key);
  }
}
