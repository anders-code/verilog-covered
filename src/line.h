#ifndef __LINE_H__
#define __LINE_H__

/*!
 \file     line.h
 \author   Trevor Williams  (trevorw@charter.net)
 \date     3/31/2002
 \brief    Contains functions for determining/reporting line coverage.
*/

#include <stdio.h>

#include "defines.h"

//! Calculates line coverage numbers for the specified expression list.
void line_get_stats( stmt_link* stmtl, float* total, int* hit );

//! Generates report output for line coverage.
void line_report( FILE* ofile, bool verbose, bool instance );


/* $Log$
/* Revision 1.2  2002/05/03 03:39:36  phase1geo
/* Removing all syntax errors due to addition of statements.  Added more statement
/* support code.  Still have a ways to go before we can try anything.  Removed lines
/* from expressions though we may want to consider putting these back for reporting
/* purposes.
/* */

#endif
