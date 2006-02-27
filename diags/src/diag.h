#ifndef __DIAG_H__
#define __DIAG_H__

/*!
 \file     diag.h
 \author   Trevor Williams  (trevorw@charter.net)
 \date     2/27/2006
 \brief    Contains functions for reading in and running a single diagnostic.
*/


/*! \brief  Reads the specified diagnostic regression information and stores for later use. */
int read_diag_info( char* diag_name );

/*! \brief  Runs the specified diagnostic, returnings its run-time status. */
void run_current_diag( char* diag_name, int* ran, int* failed, char* failing_cmd );


/*
 $Log$
 Revision 1.1  2006/02/27 23:22:10  phase1geo
 Working on C-version of run command.  Initial version only -- does not work
 at this point.

*/

#endif

