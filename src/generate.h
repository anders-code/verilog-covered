#ifndef __GENERATE_H__
#define __GENERATE_H__

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
 \file     generate.h
 \author   Trevor Williams  (phase1geo@gmail.com)
 \date     8/9/2009
 \brief    Contains functions for the generate command.
*/


extern unsigned int inline_comb_depth;
extern char*        pragma_coverage_name;
extern char*        pragma_racecheck_name;
extern int          delay_expr_type;
extern unsigned int flag_global_generation;
extern char*        cdd_message;
extern bool         flag_conservative;
extern char*        directive_filename;
extern str_link*    gen_mod_head;
extern bool         instance_specified;
extern char*        top_instance;
extern char*        ppfilename;
extern char*        dumpvars_file;


/*! \brief Creates a module that contains all of the signals to dump from the design for coverage purposes. */
void generate_top_dumpvars_module(
  const char* dumpvars_file
);

/*! \brief Parses the specified define from the command-line */
void generate_parse_define(
  const char* def
);

/*! \brief Performs the 'generate' command. */
void command_generate(
  int          argc,
  int          last_arg,
  const char** argv
);

#endif

