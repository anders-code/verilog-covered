#ifndef __PERF_H__
#define __PERF_H__

/*!
 \file     perf.h
 \author   Trevor Williams  (trevorw@charter.net)
 \date     1/1/2006
 \brief    Contains functions for calculating and outputting simulation performance metrics.
*/

#include <stdio.h>
#include "defines.h"


/*!
 \brief Generates a performance report on an instance basis to the specified output file.
*/
void perf_output_inst_report( FILE* ofile );


/*
 $Log$
 Revision 1.1  2006/01/02 21:35:36  phase1geo
 Added simulation performance statistical information to end of score command
 when we are in debug mode.

*/

#endif
