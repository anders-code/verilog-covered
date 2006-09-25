#ifndef __MEMORY_H__
#define __MEMORY_H__

/*!
 \file     memory.h
 \author   Trevor Williams  (trevorw@charter.net)
 \date     9/24/2006
 \brief    Contains functions for generating memory coverage reports
*/

#include <stdio.h>

#include "defines.h"


/*! \brief Calculates line coverage numbers for the specified expression list. */
void memory_get_stats( sig_link* sigl, float* ae_total, int* wr_hit, int* rd_hit, float* tog_total, int* tog01_hit, int* tog10_hit );

/*! \brief Generates report output for line coverage. */
void memory_report( FILE* ofile, bool verbose );


/*
 $Log$
 Revision 1.2  2006/09/25 22:22:28  phase1geo
 Adding more support for memory reporting to both score and report commands.
 We are getting closer; however, regressions are currently broken.  Checkpointing.

 Revision 1.1  2006/09/25 04:15:04  phase1geo
 Starting to add support for new memory coverage metric.  This includes changes
 for the report command only at this point.

*/

#endif

