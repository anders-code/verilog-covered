/*
 Copyright (c) 2006-2009 Trevor Williams

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
 \file     generate.c
 \author   Trevor Williams  (phase1geo@gmail.com)
 \date     8/9/2009
*/

#include <string.h>

#include "profiler.h"


/*!
 Parses the 'generate' command-line options, creates the design database, generated Verilog code
 and empty coverage file.
*/
void command_generate(
  int          argc,      /*!< Number of arguments in score command-line */
  int          last_arg,  /*!< Index of last parsed argument in list */
  const char** argv       /*!< Arguments from command-line to parse */
) { PROFILE(COMMAND_GENERATE);

  PROFILE_END;

}

