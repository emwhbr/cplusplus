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

#ifndef __BST_H__
#define __BST_H__

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Class support types
/////////////////////////////////////////////////////////////////////////////
struct bst_node;
typedef struct bst_node BST_NODE;

typedef struct bst_node {
  int      key;
  int      data; 
  BST_NODE *left; 
  BST_NODE *right; 
} BST_NODE;

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class bst {

 public:
  bst(void);
  ~bst(void);

  void insert(int key, int data);

  bool search_data(int key, int &data);

  int size(void);

  void dump(void);

  void erase(void);

 private:
  BST_NODE *m_root_node;

  BST_NODE* new_bst_node(int key, const int *data);

  BST_NODE* insert_bst(BST_NODE *node, int key, const int *data);

  const BST_NODE* search_bst(const BST_NODE *node, int key);

  int nodes_bst(const BST_NODE *node);

  void dump_bst(const BST_NODE *node);

  void erase_bst(BST_NODE **node);
};

#endif // __BST_H__
