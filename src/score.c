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
  printf( "Usage:  covered score (-h | -vcd <dumpfile> | -lxt <dumpfile>) [<options>] <coverage filename>\n" );
  printf( "\n" );
  printf( "   The <coverage filename> represents the base name of the file that coverage information for this run will be output\n" );
  printf( "   to.  The coverage database file will be written to %s/cov/tests/<coverage filename>.cdb.\n", get_cdd() );
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
  printf( "      -f <filename>                Name of file containing additional arguments to parse.\n" );
  printf( "      -S                           Outputs simulation performance information after scoring has completed.  This\n" );
  printf( "                                     information is currently only useful for the developers of Covered.\n" );
  printf( "      -m <message>                 Allows the user to specify information about this CDD file.  This information can\n" );
  printf( "                                     be anything (messages with whitespace should be surrounded by double-quotation marks),\n" );
  printf( "                                     but may include something about the simulation arguments to more easily link the\n" );
  printf( "                                     CDD file to its simulation for purposes of recreating the CDD file.\n" );
  printf( "\n" );
  printf( "   Note:\n" );
  printf( "     Any plusargs that need to be passed to the score command can be added anywhere in the score command options.\n" );
  printf( "     Example:\n" );
  printf( "       covered score -vcd top.vcd +plusarg_option1 +plusarg_option2=13 top\n" );
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

    } else if( (i + 1) == argc ) {

      unsigned int slen;
      unsigned int rv;

      /* If the user wants to call the coverage database "merged", it cannot as this is a reserved filename */
      if( strncmp( "merged", argv[i], 6 ) == 0 ) {
        print_output( "The coverage database filename \"merged\" is reserved.  Please specify a different filename to output to.", FATAL, __FILE__, __LINE__ );
        Throw 0;
      }
  
      /* Create the output database name */
      slen      =  strlen( argv[i] ) + 5;
      output_db = (char*)malloc_safe( slen );
      rv        = snprintf( output_db, slen, "%s.cdb", argv[i] );
      assert( rv < slen );

    } else {

      unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Unknown score command option \"%s\".  See \"covered score -h\" for more information.", argv[i] );
      assert( rv < USER_MSG_LENGTH );
      print_output( user_msg, FATAL, __FILE__, __LINE__ );
      Throw 0;

    }

    i++;

  }

  if( !help_found ) {

    /* The user must supply an output coverage filename, if they didn't output an error message */
    if( output_db == NULL ) {
      print_output( "Output coverage filename was not specified on the command-line", FATAL, __FILE__, __LINE__ );
      Throw 0;
    }

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

