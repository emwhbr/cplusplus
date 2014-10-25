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
#include <string.h>

#include "bst.h"

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

bst::bst(void)
{
  m_root_node = 0;
}

////////////////////////////////////////////////////////////////

bst::~bst(void)
{
  this->erase();
}

////////////////////////////////////////////////////////////////

void bst::insert(int key, int data)
{
  m_root_node = insert_bst(m_root_node, key, &data);
}

////////////////////////////////////////////////////////////////

bool bst::get_data(int key, int &data)
{
  const BST_NODE *node = search_bst(m_root_node, key);

  // Check if key found
  if (node) {
    data = node->data;
    return true;
  }
  else {
    return false;
  }
}

////////////////////////////////////////////////////////////////

bool bst::get_min_key(int &key)
{
  const BST_NODE *node = min_key_bst(m_root_node);

  // Check if key found
  if (node) {
    key = node->key;
    return true;
  }
  else {
    return false;
  }
}

////////////////////////////////////////////////////////////////

bool bst::get_in_order_successor(int key, int &succ_key)
{
  const BST_NODE *node = search_bst(m_root_node, key);

  // Check if key found
  if (node) {

    const BST_NODE *succ_node = in_order_successor_bst(m_root_node, node);

    // Check if successor key found
    if (succ_node) {
      succ_key = succ_node->key;
      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

////////////////////////////////////////////////////////////////

int bst::size(void)
{
  return nodes_bst(m_root_node);
}

////////////////////////////////////////////////////////////////

void bst::dump(void)
{
  dump_bst(m_root_node);
}

////////////////////////////////////////////////////////////////

void bst::erase(void)
{
  erase_bst(&m_root_node);
}

/////////////////////////////////////////////////////////////////////////////
//               Private member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

BST_NODE* bst::new_bst_node(int key, const int *data)
{
  BST_NODE *node = new BST_NODE;

  node->key = key;
  memcpy(&node->data, data, sizeof(*data));
 
  node->left  = 0;
  node->right = 0;

  return node;
}

////////////////////////////////////////////////////////////////

BST_NODE* bst::insert_bst(BST_NODE *node, int key, const int *data)
{
  // 1. If the tree is empty, return a new single node
  if (!node) {
    return new_bst_node(key, data);
  }
  else {
    // 2. Otherwise, recurse down the tree
    if (key <= node->key) {
      node->left = insert_bst(node->left, key, data);
    }
    else {
      node->right = insert_bst(node->right, key, data);
    }

    return node; // return the (unchanged) node pointer
  }
}

////////////////////////////////////////////////////////////////

const BST_NODE* bst::search_bst(const BST_NODE *node, int key)
{
  // 1. Base case is empty tree
  // target is not found so return false
  if (!node) { 
    return 0;
  }
  else {
    // 2. see if found here 
    if (key == node->key) {
      return node;
    }
    else {
      // 3. otherwise recurse down the correct subtree
      if (key < node->key) {
	return search_bst(node->left, key);
      }
      else {
	return search_bst(node->right, key);
      }
    }
  }
}

////////////////////////////////////////////////////////////////

BST_NODE* bst::min_key_bst(const BST_NODE *node)
{
  BST_NODE *current = (BST_NODE *)node;
  
  // Loop down to find the leftmost node
  while (current->left) {
    current = current->left;
  }
  return current;
}

////////////////////////////////////////////////////////////////

BST_NODE* bst::in_order_successor_bst(BST_NODE *root,
				      const BST_NODE *node)
{
  // If right subtree of node is not NULL, then succ lies
  // in right subtree. Go to right subtree and return the
  // node with minimum key value in right subtree.
  if (node->right) {
    return min_key_bst(node->right);
  }
  
  BST_NODE *succ = 0;
  
  // Start from root and search for successor down the tree.
  // If a node’s data is greater than root’s data then go
  // right side, otherwise go to left side.
  while (root) {
    if (node->key < root->key) {
      succ = root;
      root = root->left;
    }
    else if (node->key > root->key) {
      root = root->right;
    }
    else {
      break;
    }
  }
  
  return succ;
}

////////////////////////////////////////////////////////////////

int bst::nodes_bst(const BST_NODE *node)
{
  if (!node) {
    return 0; // No nodes in tree
  } else {
    // Compute the number of nodes in a tree
    return (nodes_bst(node->left) + 1 + nodes_bst(node->right));
  }
}

////////////////////////////////////////////////////////////////

void bst::dump_bst(const BST_NODE *node)
{
  if (!node) {
    return;
  }
	
  dump_bst(node->left);
  
  printf("key=%d, data=%d\n",
	 node->key, node->data);
  
  dump_bst(node->right);
}

////////////////////////////////////////////////////////////////

void bst::erase_bst(BST_NODE **node)
{
  if (*node) {
    erase_bst(&(*node)->left);
    erase_bst(&(*node)->right);
		
    printf("delete key=%d, data=%d\n",
	   (*node)->key, (*node)->data);
		
    delete *node;

    *node = 0;
  }
}
