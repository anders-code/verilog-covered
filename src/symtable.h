#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

/*!
 \file     symtable.h
 \author   Trevor Williams  (trevorw@charter.net)
 \date     1/3/2002
 \brief    Contains functions for manipulating a symtable structure.
*/

#include "defines.h"


//! Creates a new symtable entry and adds it to the specified symbol table.
void symtable_add( char* sym, signal* sig, symtable** symtab );

//! Finds specified symbol in specified symbol table and returns pointer to signal.
bool symtable_find( char* sym, symtable* symtab, signal** sig, int skip );

//! Deallocates all symtable entries for specified symbol table.
void symtable_dealloc( symtable* symtab );

/* $Log$
/* Revision 1.3  2002/07/03 03:31:11  phase1geo
/* Adding RCS Log strings in files that were missing them so that file version
/* information is contained in every source and header file.  Reordering src
/* Makefile to be alphabetical.  Adding mult1.v diagnostic to regression suite.
/* */

#endif

