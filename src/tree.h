#ifndef __TREE_H__
#define __TREE_H__

/*!
 \file     tree.h
 \author   Trevor Williams  (trevorw@charter.net)
 \date     1/4/2003
 \brief    Contains functions for adding, finding, and removing nodes from binary tree.
*/

#include "defines.h"

/*! \brief Adds specified key/value pair to tree as a node. */
tnode* tree_add( const char* key, const char* value, bool override, tnode** root );

/*! \brief Returns pointer to tree node that matches specified key */
tnode* tree_find( const char* key, tnode* root );

/*! \brief Removes specified tree node from tree. */
void tree_remove( const char* key, tnode** root );

/*! \brief Deallocates entire tree from memory. */
void tree_dealloc( tnode* root );

/*
 $Log$
 Revision 1.1  2003/01/04 09:25:15  phase1geo
 Fixing file search algorithm to fix bug where unexpected module that was
 ignored cannot be found.  Added instance7.v diagnostic to verify appropriate
 handling of this problem.  Added tree.c and tree.h and removed define_t
 structure in lexer.

*/

#endif

