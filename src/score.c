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
 \file     score.c
 \author   Trevor Williams  (phase1geo@gmail.com)
 \date     11/29/2001
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <sys/times.h>
#include <unistd.h>

#include "db.h"
#include "defines.h"
#include "fsm_arg.h"
#include "fsm_var.h"
#include "info.h"
#include "link.h"
#include "ovl.h"
#include "parse.h"
#include "param.h"
#include "perf.h"
#include "score.h"
#include "search.h"
#include "sys_tasks.h"
#include "info.h"
#include "util.h"
#include "vector.h"
#include "vpi.h"


/*! Name of dumpfile to parse */
static char* dump_file = NULL;

/*! Specifies dumpfile format to parse */
static int dump_mode = DUMP_FMT_NONE;

/*! Name of LXT dumpfile to parse */
char* lxt_file = NULL;

/*! Specifies timestep increment to display current time */
uint64 timestep_update = 0;

/*! Specifies the name of an input file to use for debugging */
char* command_line_debug_file = NULL;

/*!
 Pointer to head of string list containing the names of modules that should be ignored for race condition checking.
*/
str_link* race_ignore_mod_head = NULL;

/*!
 Pointer to tail of string list containing the names of modules that should be ignored for race condition checking.
*/
str_link* race_ignore_mod_tail = NULL;

/*!
 Name of output coverage database file.
*/
static char* output_db = NULL;

/*! Specifies if simulation performance information should be output */
static bool flag_display_sim_stats = FALSE;

/*! User-supplied message to include in the CDD database */
char* cdd_message = NULL;


/*!
 Displays usage information for score command.
*/
static void score_usage() {

  printf( "\n" );
  printf( "Usage:  covered score (-h | -t <top-level_module_name> [-vcd <dumpfile> | -lxt <dumpfile>] [<options>])\n" );
  printf( "\n" );
  printf( "   Dumpfile formats:\n" );
  printf( "      Both the VCD and LXT style dumpfiles are supported by Covered.\n" );
  printf( "\n" );
  printf( "      If either the -vcd or -lxt option is specified, the design is scored using this dumpfile\n" );
  printf( "      for coverage gathering.  If neither option is specified, Covered will only create an\n" );
  printf( "      initial CDD file from the design and will not attempt to score the design.  An error message\n" );
  printf( "      will be displayed if both options are present on the command-line.\n" );
  printf( "\n" );
  printf( "   -h                              Displays this help information.\n" );
  printf( "\n" );
  printf( "   Options:\n" );
  printf( "      -vpi (<name>)                Generates Verilog module called <name> which contains code to\n" );
  printf( "                                     allow Covered to run as a VPI during simulation.  If <name>\n" );
  printf( "                                     is not specified, the module file is called %s\n", DFLT_VPI_NAME );
  printf( "                                     If the -vcd option is specified along with this option, this\n" );
  printf( "                                     option will not be used.\n" );
  printf( "      -dumpvars (<name>)           Generates Verilog module called <name> which contains code to\n" );
  printf( "                                     dump only the signals in the design that are necessary for coverage\n" );
  printf( "                                     purposes.  If compiled as a top-module along with your design and no\n" );
  printf( "                                     other $dumpfile/$dumpvars calls are made, this dump module will provide\n" );
  printf( "                                     additional performance gains.  The name of the dumpfile created is called\n" );
  printf( "                                     <name>.vcd\n" );
  printf( "      -top_ts <timescale>          This option is only valid when the -vpi or -dumpvars options have been specified.\n" );
  printf( "                                     This option allows the user to specify a timescale for the generated\n" );
  printf( "                                     Verilog module.  If this option is not specified, no timescale will\n" );
  printf( "                                     be created for the generated module.  The value of <timescale> is\n" );
  printf( "                                     specified as follows:\n" );
  printf( "                                       (1|10|100)(s|ms|us|ns|ps|fs)/(1|10|100)(s|ms|us|ns|ps|fs)\n" );
  printf( "                                     If whitespace is needed between the various values, place the\n" );
  printf( "                                     entire contents of <timescale> in double quotes.\n" );
  printf( "      -i <instance_name>           Verilog hierarchical scope of top-level module to score.\n" );
  printf( "                                     Necessary if module to verify coverage is not the top-level\n" );
  printf( "                                     module in the design.  If not specified, -t value is used.\n" );
  printf( "      -o <database_filename>       Name of database to write coverage information to.\n" );
  printf( "      -cdd <database_filename>     Name of database to read coverage information from (same as -o).\n" );
  printf( "      -I <directory>               Directory to find included Verilog files.\n" );
  printf( "      -f <filename>                Name of file containing additional arguments to parse.\n" );
  printf( "      -F <module>=(<ivar>,)<ovar>  Module, input state variable and output state variable of\n" );
  printf( "                                     an FSM state variable.  If input variable (ivar) is not specified,\n" );
  printf( "                                     the output variable (ovar) is also used as the input variable.\n" ); 
  printf( "      -A ovl                       Specifies that any OVL assertion found in the design should be\n" );
  printf( "                                     automatically included for assertion coverage.\n" );
  printf( "      -y <directory>               Directory to find unspecified Verilog files.\n" );
  printf( "      -v <filename>                Name of specific Verilog file to score.\n" );
  printf( "      -D <define_name>(=<value>)   Defines the specified name to 1 or the specified value.\n" );
  printf( "      -p <filename>                Specifies name of file to use for preprocessor output.\n" );
  printf( "      -P <parameter_scope>=<value> Performs a defparam on the specified parameter with value.\n" );
  printf( "      -T min|typ|max               Specifies value to use in delay expressions of the form min:typ:max.\n" );
  printf( "      -ts <number>                 If design is being scored, specifying this option will output\n" );
  printf( "                                     the current timestep (by increments of <number>) to standard output.\n" );
  printf( "      -S                           Outputs simulation performance information after scoring has completed.  This\n" );
  printf( "                                     information is currently only useful for the developers of Covered.\n" );
  printf( "      -g (<module>=)[1|2|3]        Selects generation of Verilog syntax that the parser will handle.  If\n" );
  printf( "                                     <module>= is present, only the specified module will use the provided\n" );
  printf( "                                     generation.  If <module>= is not specified, the entire design will use\n" );
  printf( "                                     the provided generation.  1=Verilog-1995, 2=Verilog-2001, 3=SystemVerilog\n" );
  printf( "                                     By default, the latest generation is parsed.\n" );
  printf( "      -cli (<filename>)            Causes the command-line debugger to be used during VCD/LXT dumpfile scoring.\n" );
  printf( "                                     If <filename> is specified, this file contains information saved in a previous\n" );
  printf( "                                     call to savehist on the CLI and causes the history contained in this file to be\n" );
  printf( "                                     replayed prior to the CLI command prompt.  If <filename> is not specified, the\n" );
  printf( "                                     CLI prompt will be immediately available at the start of simulation.  This option\n" );
  printf( "                                     is only available when Covered is configured with the --enable-debug option.\n" );
  printf( "      -m <message>                 Allows the user to specify information about this CDD file.  This information can\n" );
  printf( "                                     be anything (messages with whitespace should be surrounded by double-quotation marks),\n" );
  printf( "                                     but may include something about the simulation arguments to more easily link the\n" );
  printf( "                                     CDD file to its simulation for purposes of recreating the CDD file.\n" );
  printf( "      -conservative                If this option is specified, any logic blocks that contain code that could cause coverage\n" );
  printf( "                                     discrepancies leading to potentially inaccurate coverage results are removed from\n" );
  printf( "                                     coverage consideration.  See User's Guide for more information on what type of code\n" );
  printf( "                                     can lead to coverage inaccuracies.\n" );
  printf( "      -Wignore                     Suppress the output of warnings during code parsing and simulation.\n" );
  printf( "\n" );
  printf( "      +libext+.<extension>(+.<extension>)+\n" );
  printf( "                                   Extensions of Verilog files to allow in scoring\n" );
  printf( "\n" );
  printf( "      -inline                                Outputs Verilog with inlined code coverage\n" );
  printf( "      -inline-metrics [l][t][m][e][c][f][a]  Specifies which coverage metrics should be inlined for scoring purposes.  Only these metrics\n" );
  printf( "                                               will be available for reporting and ranking.  l=line, t=toggle, m=memory, e=logic events,\n" );
  printf( "                                               c=combinational logic, f=FSM, a=assertions.  Default is ltmecfa.\n" );
  printf( "      -inline-comb-depth <value>             Specifies the depth in an expression tree that combinational logic coverage will be scored for.\n" );
  printf( "                                               By default, combinational logic depth is infinite.\n" );
  printf( "\n" );
  printf( "   Race Condition Options:\n" );
  printf( "\n" );
  printf( "     If race condition checks are violated by one or more blocks in the design, the following options specify to Covered\n" );
  printf( "     how to handle them.\n" );
  printf( "\n" );
  printf( "      -rS                          Silent.  Remove the logic blocks from coverage consideration without reporting the information.\n" );
  printf( "      -rW                          Warning.  Remove the logic blocks from coverage consideration and report the information.  Default.\n" );
  printf( "      -rE                          Error.  Report the race condition violations and stop scoring.\n" );
  printf( "      -rI[=<module name>]          Ignore.  If =<module name> is not specified, do not perform race condition checking for the entire\n" );
  printf( "                                     design.  If =<module name> is specified, do not perform race condition checking on the specified module\n" );
  printf( "                                     only.  This option may be specified more than once.\n" );
  printf( "      -rP[=<name>]                 Use pragmas.  Skip race condition checking for all code surrounded by // racecheck off/on\n" );
  printf( "                                     embedded pragmas.  The \"racecheck\" keyword can be changed by specifying =<name> where <name>\n" );
  printf( "                                     is the new name for the race condition pragma keyword.\n" );
  printf( "\n" );
  printf( "   Optimization Options:\n" );
  printf( "      -e <block_name>              Name of module, task, function or named begin/end block to not score.\n" );
  printf( "      -ec                          Exclude continuous assignment blocks from coverage.\n" );
  printf( "      -ea                          Exclude always blocks from coverage.\n" );
  printf( "      -ei                          Exclude initial blocks from coverage.\n" );
  printf( "      -ef                          Exclude final blocks from coverage.\n" );
  printf( "      -ep [<name>]                 Exclude all code enclosed by pragmas.  By default, the pragmas are of\n" );
  printf( "                                     the format '// coverage (on|off)'; however, if <name> is specified for\n" );
  printf( "                                     this option, the pragma keyword 'coverage' will be replaced with that value.\n" );
  printf( "\n" );
  printf( "    Note:\n" );
  printf( "      The top-level module specifies the module to begin scoring.  All\n" );
  printf( "      modules beneath this module in the hierarchy will also be scored\n" );
  printf( "      unless these modules are explicitly stated to not be scored using\n" );
  printf( "      the -e flag.\n" );
  printf( "\n" );
  printf( "    Note:\n" );
  printf( "      Any plusargs that need to be passed to the score command can be added anywhere in the score command options.\n" );
  printf( "      Example:\n" );
  printf( "        covered score -t main -v top.v -vcd top.vcd +plusarg_option1 +plusarg_option2=13 -o top.cdd\n" );
  printf( "\n" );

}

/*!
 \return Returns TRUE if the help option was parsed.

 \throws anonymous search_add_directory_path Throw Throw Throw Throw Throw Throw Throw Throw Throw Throw Throw Throw
                   Throw Throw Throw Throw Throw Throw Throw Throw Throw Throw score_parse_args ovl_add_assertions_to_no_score_list
                   fsm_arg_parse read_command_file search_add_file defparam_add search_add_extensions search_add_no_score_funit

 Parses score command argument list and performs specified functions based
 on these arguments.
*/
static bool score_parse_args(
  int          argc,      /*!< Number of arguments specified in argv parameter list */
  int          last_arg,  /*!< Index of last parsed argument in list */
  const char** argv       /*!< List of arguments to parse */
) { PROFILE(SCORE_PARSE_ARGS);

  int   i          = last_arg + 1;  /* Loop iterator */
  int   j;                          /* Loop iterator */
  char* ptr;                        /* Pointer to current character in defined value */
  char* rv;                         /* Return value from snprintf calls */
  bool  help_found = FALSE;

  while( (i < argc) && !help_found ) {

    if( strncmp( "-h", argv[i], 2 ) == 0 ) {

      score_usage();
      help_found = TRUE;

    } else if( (strncmp( "-o", argv[i], 2 ) == 0) || (strncmp( "-cdd", argv[i], 4 ) == 0) ) {

      if( check_option_value( argc, argv, i ) ) {
        i++;
        if( output_db != NULL ) {
          print_output( "Only one -o/-cdd option may be present on the command-line.  Using first value...", WARNING, __FILE__, __LINE__ );
        } else {
          if( file_exists( argv[i] ) || is_legal_filename( argv[i] ) ) {
            output_db = strdup_safe( argv[i] );
          } else {
            unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Output file \"%s\" is not writable", argv[i] );
            assert( rv < USER_MSG_LENGTH );
            print_output( user_msg, FATAL, __FILE__, __LINE__ );
            Throw 0;
          }
        }
      } else {
        Throw 0;
      }

    } else if( strncmp( "-f", argv[i], 2 ) == 0 ) {

      if( check_option_value( argc, argv, i ) ) {
        char** arg_list = NULL;
        int    arg_num  = 0;
        i++;
        Try {
          read_command_file( argv[i], &arg_list, &arg_num );
          help_found = score_parse_args( arg_num, -1, (const char**)arg_list );
        } Catch_anonymous {
          for( j=0; j<arg_num; j++ ) {
            free_safe( arg_list[j], (strlen( arg_list[j] ) + 1) );
          }
          free_safe( arg_list, (sizeof( char* ) * arg_num) );
          Throw 0;
        }
        for( j=0; j<arg_num; j++ ) {
          free_safe( arg_list[j], (strlen( arg_list[j] ) + 1) );
        }
        free_safe( arg_list, (sizeof( char* ) * arg_num) );
      } else {
        Throw 0;
      }

    } else if( strncmp( "-vcd", argv[i], 4 ) == 0 ) {

      if( check_option_value( argc, argv, i ) ) {
        i++;
        switch( dump_mode ) {
          case DUMP_FMT_NONE :
            if( file_exists( argv[i] ) ) {
              dump_file = strdup_safe( argv[i] );
              dump_mode = DUMP_FMT_VCD;
            } else {
              unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "VCD dumpfile not found \"%s\"", argv[i] );
              assert( rv < USER_MSG_LENGTH );
              print_output( user_msg, FATAL, __FILE__, __LINE__ );
              Throw 0;
            }
            break;
          case DUMP_FMT_VCD :
            print_output( "Only one -vcd option is allowed on the score command-line", FATAL, __FILE__, __LINE__ );
            Throw 0;
            /*@-unreachable@*/
            break;
            /*@=unreachable@*/
          case DUMP_FMT_LXT :
            print_output( "Both the -vcd and -lxt options were specified on the command-line", FATAL, __FILE__, __LINE__ );
            Throw 0;
            /*@-unreachable@*/
            break;
            /*@=unreachable@*/
          default :
            assert( 0 );
            break;
        }
      } else {
        Throw 0;
      }

    } else if( strncmp( "-lxt", argv[i], 4 ) == 0 ) {
 
      if( check_option_value( argc, argv, i ) ) {
        i++; 
        switch( dump_mode ) {
          case DUMP_FMT_NONE :
            if( file_exists( argv[i] ) ) {
              dump_file = strdup_safe( argv[i] );
              dump_mode = DUMP_FMT_LXT;
            } else {
              unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT dumpfile not found \"%s\"", argv[i] );
              assert( rv < USER_MSG_LENGTH );
              print_output( user_msg, FATAL, __FILE__, __LINE__ );
              Throw 0;
            }
            break;
          case DUMP_FMT_VCD :
            print_output( "Both the -vcd and -lxt options were specified on the command-line", FATAL, __FILE__, __LINE__ );
            Throw 0;
            /*@-unreachable@*/
            break;
            /*@=unreachable@*/
          case DUMP_FMT_LXT :
            print_output( "Only one -lxt option is allowed on the score command-line", FATAL, __FILE__, __LINE__ );
            Throw 0;
            /*@-unreachable@*/
            break;
            /*@=unreachable@*/
          default :
            assert( 0 );
            break;
        }
      } else {
        Throw 0;
      }

    } else if( strncmp( "-S", argv[i], 2 ) == 0 ) {

      flag_display_sim_stats = TRUE;

    } else if( strncmp( "-m", argv[i], 2 ) == 0 ) {

      if( check_option_value( argc, argv, i ) ) {
        i++;
        if( cdd_message != NULL ) {
          print_output( "Only one -m option is allowed on the score command-line.  Using first value...", WARNING, __FILE__, __LINE__ );
        } else {
          cdd_message = strdup_safe( argv[i] );
        }
      } else {
        Throw 0;
      }

    /* Any other option that is a plusarg will be added to the list of simulation plusargs */
    } else if( strncmp( "+", argv[i], 1 ) == 0 ) {

      sys_task_store_plusarg( argv[i] + 1 );

    } else {

      unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Unknown score command option \"%s\".  See \"covered score -h\" for more information.", argv[i] );
      assert( rv < USER_MSG_LENGTH );
      print_output( user_msg, FATAL, __FILE__, __LINE__ );
      Throw 0;

    }

    i++;

  }

  return( help_found );

}

/*!
 Performs score command functionality.
*/
void command_score(
  int          argc,      /*!< Number of arguments in score command-line */
  int          last_arg,  /*!< Index of last parsed argument in list */
  const char** argv       /*!< Arguments from command-line to parse */
) { PROFILE(COMMAND_SCORE);

  unsigned int rv;  /* Return value from snprintf calls */
  bool         error = FALSE;

  /* Output header information */
  rv = snprintf( user_msg, USER_MSG_LENGTH, COVERED_HEADER );
  assert( rv < USER_MSG_LENGTH );
  print_output( user_msg, NORMAL, __FILE__, __LINE__ );

  Try {

    /* Parse score command-line */
    if( !score_parse_args( argc, last_arg, argv ) ) {

      /* Read dumpfile and score design */
      if( dump_mode != DUMP_FMT_NONE ) {

        switch( dump_mode ) {
          case DUMP_FMT_VCD :  rv = snprintf( user_msg, USER_MSG_LENGTH, "Scoring VCD dumpfile %s...", dump_file );  break;
          case DUMP_FMT_LXT :  rv = snprintf( user_msg, USER_MSG_LENGTH, "Scoring LXT dumpfile %s...", dump_file );  break;
        }
        assert( rv < USER_MSG_LENGTH );
        print_output( user_msg, NORMAL, __FILE__, __LINE__ );
        parse_and_score_dumpfile( output_db, dump_file, dump_mode );
        print_output( "", NORMAL, __FILE__, __LINE__ );

      }

      if( dump_mode != DUMP_FMT_NONE ) {
        print_output( "***  Scoring completed successfully!  ***\n", NORMAL, __FILE__, __LINE__ );
      }

      /* Display simulation statistics if specified */
      if( flag_display_sim_stats ) {
        perf_output_inst_report( stdout );
      }

    }

  } Catch_anonymous {
    error = TRUE;
  }

  /* Close database */
  db_close();

  free_safe( output_db, (strlen( output_db ) + 1) );
  free_safe( dump_file, (strlen( dump_file ) + 1) );

  if( error ) {
    Throw 0;
  }

  PROFILE_END;

}

