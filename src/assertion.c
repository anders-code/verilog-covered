/*!
 \file     assertion.c
 \author   Trevor Williams  (trevorw@charter.net)
 \date     4/4/2006
*/


#include "defines.h"
#include "assertion.h"


/*!
 \param arg  Command-line argument specified in -A option to score command to parse

 Parses the specified command-line argument as an assertion to consider for coverage.
*/
void assertion_parse( char* arg ) {

}

/*!
 \param ap     Pointer to attribute to parse
 \param funit  Pointer to current functional unit containing this attribute

 Parses the specified assertion attribute for assertion coverage details.
*/
void assertion_parse_attr( attr_param* ap, func_unit* funit ) {

}

/*
 $Log$
 Revision 1.1  2006/04/05 15:19:18  phase1geo
 Adding support for FSM coverage output in the GUI.  Started adding components
 for assertion coverage to GUI and report functions though there is no functional
 support for this at this time.

*/

