/*!
 \file     fsm_arg.c
 \author   Trevor Williams  (trevorw@sgi.com)
 \date     10/02/2003
*/

#include "defines.h"
#include "fsm_arg.h"


/*!
 \param arg  Command-line argument following -F specifier.

 \return Returns TRUE if argument is considered legal for the -F specifier;
         otherwise, returns FALSE.

 Parses specified argument string for FSM information.  If the FSM information
 is considered legal, returns TRUE; otherwise, returns FALSE.
*/
bool fsm_arg_parse( char* arg ) {

  bool retval = TRUE;  /* Return value for this function */

  return( retval );

}

/*
 $Log$
 Revision 1.1  2003/10/02 12:30:56  phase1geo
 Initial code modifications to handle more robust FSM cases.

*/

