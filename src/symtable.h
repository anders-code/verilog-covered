#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

/*!
 \file     symtable.h
 \author   Trevor Williams  (trevorw@charter.net)
 \date     1/3/2002
 \brief    Contains functions for manipulating a symtable structure.
*/

#include "defines.h"


/*! \brief Creates a new symtable entry and adds it to the specified symbol table. */
symtable* symtable_add( char* sym, signal* sig, symtable** symtab );

/*! \brief Finds specified symbol in specified symbol table and returns pointer to signal. */
symtable* symtable_find( char* sym, symtable* symtab, int skip );

/*! \brief Assigns stored values to all associated signals stored in specified symbol table. */
void symtable_assign( symtable* symtab );

/*! \brief Deallocates all symtable entries for specified symbol table. */
void symtable_dealloc( symtable* symtab );


/*
 $Log$
 Revision 1.7  2002/11/05 00:20:08  phase1geo
 Adding development documentation.  Fixing problem with combinational logic
 output in report command and updating full regression.

 Revision 1.6  2002/10/31 23:14:30  phase1geo
 Fixing C compatibility problems with cc and gcc.  Found a few possible problems
 with 64-bit vs. 32-bit compilation of the tool.  Fixed bug in parser that
 lead to bus errors.  Ran full regression in 64-bit mode without error.

 Revision 1.5  2002/10/29 19:57:51  phase1geo
 Fixing problems with beginning block comments within comments which are
 produced automatically by CVS.  Should fix warning messages from compiler.

 Revision 1.4  2002/07/05 16:49:47  phase1geo
 Modified a lot of code this go around.  Fixed VCD reader to handle changes in
 the reverse order (last changes are stored instead of first for timestamp).
 Fixed problem with AEDGE operator to handle vector value changes correctly.
 Added casez2.v diagnostic to verify proper handling of casez with '?' characters.
 Full regression passes; however, the recent changes seem to have impacted
 performance -- need to look into this.

 Revision 1.3  2002/07/03 03:31:11  phase1geo
 Adding RCS Log strings in files that were missing them so that file version
 information is contained in every source and header file.  Reordering src
 Makefile to be alphabetical.  Adding mult1.v diagnostic to regression suite.
*/

#endif

