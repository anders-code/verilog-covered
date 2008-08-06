/*
 Copyright (c) 2006 Trevor Williams

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program;
 if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*!
 \file     gui.c
 \author   Trevor Williams  (phase1geo@gmail.com)
 \date     11/24/2003
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STRING_H
#include "string.h"
#endif

#include "gui.h"
#include "util.h"
#include "link.h"
#include "func_unit.h"
#include "ovl.h"


extern db**         db_list;
extern unsigned int curr_db;
extern isuppl       info_suppl;


/*!
 \return Returns a value of TRUE if functional unit was found; otherwise, returns a value of FALSE.

 Finds specified functional unit name in design and returns the starting and ending line numbers of
 the found functional unit, returning a value of TRUE to the calling function.  If the functional unit was
 not found in the design, a value of FALSE is returned.
*/
bool funit_get_start_and_end_lines(
            const char* funit_name,  /*!< Name of functional unit to get start and end line numbers for */
            int         funit_type,  /*!< Type of functional unit to get start and end line numbers for */
  /*@out@*/ int*        start_line,  /*!< Pointer to value that will contain starting line number of this functional unit */
  /*@out@*/ int*        end_line     /*!< Pointer to value that will contain ending line number of this functional unit */
) { PROFILE(FUNIT_GET_START_AND_END_LINES);

  bool        retval = TRUE;  /* Return value of this function */
  funit_link* funitl;         /* Pointer to functional unit line containing matched functional unit */
  
  if( (funitl = funit_link_find( funit_name, funit_type, db_list[curr_db]->funit_head )) != NULL ) {

    *start_line = funitl->funit->start_line;
    *end_line   = funitl->funit->end_line;

  } else {

    retval = FALSE;

  }

  PROFILE_END;

  return( retval );

}

/*
 $Log$
 Revision 1.14.4.2  2008/08/06 20:11:34  phase1geo
 Adding support for instance-based coverage reporting in GUI.  Everything seems to be
 working except for proper exclusion handling.  Checkpointing.

 Revision 1.14.4.1  2008/07/10 22:43:52  phase1geo
 Merging in rank-devel-branch into this branch.  Added -f options for all commands
 to allow files containing command-line arguments to be added.  A few error diagnostics
 are currently failing due to changes in the rank branch that never got fixed in that
 branch.  Checkpointing.

 Revision 1.16  2008/06/27 14:02:01  phase1geo
 Fixing splint and -Wextra warnings.  Also fixing comment formatting.

 Revision 1.15  2008/06/19 16:14:55  phase1geo
 leaned up all warnings in source code from -Wall.  This also seems to have cleared
 up a few runtime issues.  Full regression passes.

 Revision 1.14  2008/04/15 20:37:11  phase1geo
 Fixing database array support.  Full regression passes.

 Revision 1.13  2008/01/16 06:40:37  phase1geo
 More splint updates.

 Revision 1.12  2008/01/07 23:59:54  phase1geo
 More splint updates.

 Revision 1.11  2007/12/11 05:48:25  phase1geo
 Fixing more compile errors with new code changes and adding more profiling.
 Still have a ways to go before we can compile cleanly again (next submission
 should do it).

 Revision 1.10  2007/11/20 05:28:58  phase1geo
 Updating e-mail address from trevorw@charter.net to phase1geo@gmail.com.

 Revision 1.9  2007/04/03 04:15:17  phase1geo
 Fixing bugs in func_iter functionality.  Modified functional unit name
 flattening function (though this does not appear to be working correctly
 at this time).  Added calls to funit_flatten_name in all of the reporting
 files.  Checkpointing.

 Revision 1.8  2006/05/01 22:27:37  phase1geo
 More updates with assertion coverage window.  Still have a ways to go.

 Revision 1.7  2006/03/28 22:28:27  phase1geo
 Updates to user guide and added copyright information to each source file in the
 src directory.  Added test directory in user documentation directory containing the
 example used in line, toggle, combinational logic and FSM descriptions.

 Revision 1.6  2005/11/10 19:28:23  phase1geo
 Updates/fixes for tasks/functions.  Also updated Tcl/Tk scripts for these changes.
 Fixed bug with net_decl_assign statements -- the line, start column and end column
 information was incorrect, causing problems with the GUI output.

 Revision 1.5  2005/11/08 23:12:09  phase1geo
 Fixes for function/task additions.  Still a lot of testing on these structures;
 however, regressions now pass again so we are checkpointing here.

 Revision 1.4  2004/03/16 05:45:43  phase1geo
 Checkin contains a plethora of changes, bug fixes, enhancements...
 Some of which include:  new diagnostics to verify bug fixes found in field,
 test generator script for creating new diagnostics, enhancing error reporting
 output to include filename and line number of failing code (useful for error
 regression testing), support for error regression testing, bug fixes for
 segmentation fault errors found in field, additional data integrity features,
 and code support for GUI tool (this submission does not include TCL files).

 Revision 1.3  2004/01/04 04:52:03  phase1geo
 Updating ChangeLog and TODO files.  Adding merge information to INFO line
 of CDD files and outputting this information to the merged reports.  Adding
 starting and ending line information to modules and added function for GUI
 to retrieve this information.  Updating full regression.

 Revision 1.2  2003/11/29 06:55:48  phase1geo
 Fixing leftover bugs in better report output changes.  Fixed bug in param.c
 where parameters found in RHS expressions that were part of statements that
 were being removed were not being properly removed.  Fixed bug in sim.c where
 expressions in tree above conditional operator were not being evaluated if
 conditional expression was not at the top of tree.

 Revision 1.1  2003/11/24 17:48:56  phase1geo
 Adding gui.c/.h files for functions related to the GUI interface.  Updated
 Makefile.am for the inclusion of these files.

*/

