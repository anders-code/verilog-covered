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
 \file     parse.c
 \author   Trevor Williams  (phase1geo@gmail.com)
 \date     11/27/2001
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "binding.h"
#include "db.h"
#include "defines.h"
#include "fsm_var.h"
#include "generate.h"
#include "generator.h"
#include "info.h"
#include "link.h"
#include "lxt.h"
#include "parse.h"
#include "parser_func.h"
#include "parser_misc.h"
#include "score.h"
#include "search.h"
#include "sim.h"
#include "stmt_blk.h"
#include "util.h"
#include "vcd.h"


/*!
 \return Returns the number of characters read from this line.

 Reads from specified file, one character at a time, until either a newline
 or EOF is encountered.  Returns the read line and the number of characters
 stored in the line.  No newline character is added to the line.
*/
int parse_readline(
  FILE* file,  /*!< Pointer to file to read */
  char* line,  /*!< Read line from specified file */
  int   size   /*!< Maximum number of characters that can be stored in line */
) { PROFILE(PARSE_READLINE);

  int i = 0;  /* Loop iterator */

  while( (i < (size - 1)) && !feof( file ) && ((line[i] = fgetc( file )) != '\n') ) {
    i++;
  }

  if( !feof( file ) ) {
    line[i] = '\0';
  }

  if( i == size ) {
    unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Line too long.  Must be less than %d characters.", size );
    assert( rv < USER_MSG_LENGTH );
    print_output( user_msg, FATAL, __FILE__, __LINE__ );
  }

  PROFILE_END;

  return( !feof( file ) );

}

/*!
 \throws anonymous fsm_var_bind Throw bind_perform db_write

 Resets the lexer and parses all Verilog files specified in use_files list.
 After all design files are parsed, their information will be appropriately
 stored in the associated lists.
*/
void parse_design(
  const char* top  /*!< Name of top-level module to score */
) { PROFILE(PARSE_DESIGN);

  unsigned int rv;
  unsigned int slen = strlen( get_cdd() ) + 11;
  char*        odb  = (char*)malloc_safe( slen );

  rv = snprintf( odb, slen, "%s/db/cov.db", get_cdd() );
  assert( rv < slen );

  Try {

    (void)str_link_add( strdup_safe( top ), &modlist_head, &modlist_tail );

    if( use_files_head != NULL ) {

      int parser_ret;

      /* Initialize lexer with first file */
      reset_lexer( use_files_head );

      Try {

        /* Parse the design -- if we catch an exception, remove the temporary ppfilename */
        parser_ret = VLparse();

        if( (parser_ret != 0) || (error_count > 0) ) {
          print_output( "Error in parsing design", FATAL, __FILE__, __LINE__ );
          Throw 0;
        }

      } Catch_anonymous {
        (void)unlink( ppfilename );
        parser_dealloc_sig_range( &curr_urange, FALSE );
        parser_dealloc_sig_range( &curr_prange, FALSE );
        Throw 0;
      }
     
      /* Deallocate any memory in curr_range variable */
      parser_dealloc_sig_range( &curr_urange, FALSE );
      parser_dealloc_sig_range( &curr_prange, FALSE );

#ifdef DEBUG_MODE
      if( debug_mode ) {
        print_output( "========  Completed design parsing  ========\n", DEBUG, __FILE__, __LINE__ );
      }
#endif

      /* Check to make sure that the -t and -i options were specified correctly */
      if( db_check_for_top_module() ) {
        if( instance_specified ) {
          unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Module specified with -t option (%s) is a top-level module.", top_module );
          assert( rv < USER_MSG_LENGTH );
          print_output( user_msg, FATAL, __FILE__, __LINE__ );
          print_output( "The -i option should not have been specified", FATAL_WRAP, __FILE__, __LINE__ );
          Throw 0;
        }
      } else {
        if( !instance_specified ) {
          unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Module specified with -t option (%s) is not a top-level module.", top_module );
          assert( rv < USER_MSG_LENGTH );
          print_output( user_msg, FATAL, __FILE__, __LINE__ );
          print_output( "The -i option must be specified to provide the hierarchy to the module specified with", FATAL_WRAP, __FILE__, __LINE__ );
          print_output( "the -t option.", FATAL_WRAP, __FILE__, __LINE__ );
          Throw 0;
        }
      }

      /* Perform all signal/expression binding */
      bind_perform( FALSE, 0 );
      fsm_var_bind();
  
      /* Remove all statement blocks that cannot be considered for coverage */
      stmt_blk_remove();

#ifdef DEBUG_MODE
      if( debug_mode ) {
        print_output( "========  Completed race condition checking  ========\n", DEBUG, __FILE__, __LINE__ );
      }
#endif

    } else {

      print_output( "No Verilog input files specified", FATAL, __FILE__, __LINE__ );
      Throw 0;

    }

    /* Deallocate module list */
    str_link_delete_list( modlist_head );
    modlist_head = modlist_tail = NULL;

    /* Output the dumpvars module, if specified. */
    if( dumpvars_file != NULL ) {
      unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Outputting dumpvars file %s...", dumpvars_file );
      assert( rv < USER_MSG_LENGTH );
      print_output( user_msg, NORMAL, __FILE__, __LINE__ );
      generate_top_dumpvars_module( dumpvars_file );
    }

    /* Write contents to baseline database file. */
    db_write( odb, TRUE, TRUE );

    /* Generate the needed Verilog */
    generator_output();

  } Catch_anonymous {
    /* Deallocate module list */
    free_safe( odb, slen );
    str_link_delete_list( modlist_head );
    modlist_head = modlist_tail = NULL;
    fsm_var_cleanup();
    sim_dealloc();
    db_close();
    Throw 0;
  }

  /* Deallocate simulator stuff */
  sim_dealloc();

  /* Close database */
  db_close();

#ifdef DEBUG_MODE
  if( debug_mode ) {
    unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "========  Design written to database %s successfully  ========\n\n", odb );
    assert( rv < USER_MSG_LENGTH );
    print_output( user_msg, DEBUG, __FILE__, __LINE__ );
  }
#endif

  free_safe( odb, slen );

  PROFILE_END;

}

/*!
 \throws anonymous db_do_timestep Throw vcd_parse bind_perform db_read lxt_parse db_write

 Reads in specified CDD database file, reads in specified dumpfile in the specified format,
 performs re-simulation and writes the scored design back to the specified CDD database file
 for merging or reporting.
*/
void parse_and_score_dumpfile(
  const char* cdb,        /*!< Name of output coverage database file to generate */
  const char* dump_file,  /*!< Name of dumpfile to parse for scoring */
  int         dump_mode   /*!< Type of dumpfile being used (see \ref dumpfile_fmt for legal values) */
) { PROFILE(PARSE_AND_SCORE_DUMPFILE);

  unsigned int rv;
  unsigned int olen = strlen( get_cdd() ) + 11 + strlen( cdb ) + 1;
  char*        odb  = (char*)malloc_safe( olen );
  unsigned int ilen = strlen( get_cdd() ) + 11;
  char*        idb  = (char*)malloc_safe( ilen );

  assert( dump_file != NULL );

  /* Output database */
  rv = snprintf( odb, olen, "%s/cov/tests/%s", get_cdd(), cdb );
  assert( rv < olen );

  /* Input database */
  rv = snprintf( idb, ilen, "%s/db/cov.db", get_cdd() );
  assert( rv < ilen );

  Try {

#ifdef DEBUG_MODE
    if( debug_mode ) {
      unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "========  Reading in database %s  ========\n", idb );
      assert( rv < USER_MSG_LENGTH );
      print_output( user_msg, DEBUG, __FILE__, __LINE__ );
    }
#endif

    /* Read in contents of specified database file */
    (void)db_read( idb, READ_MODE_NO_MERGE );
  
    /* Bind expressions to signals/functional units */
    bind_perform( TRUE, 0 );

    /* Add static values to simulator */
    // sim_initialize();

#ifdef DEBUG_MODE
    if( debug_mode ) {
      unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "========  Reading in VCD dumpfile %s  ========\n", dump_file );
      assert( rv < USER_MSG_LENGTH );
      print_output( user_msg, DEBUG, __FILE__, __LINE__ );
    }
#endif
  
    /* Perform the parse */
    switch( dump_mode ) {
      case DUMP_FMT_VCD :  vcd_parse( dump_file );  break;
      case DUMP_FMT_LXT :  lxt_parse( dump_file );  break;
      default           :  assert( (dump_mode == DUMP_FMT_VCD) || (dump_mode == DUMP_FMT_LXT) );
    }
    
    /* Flush any pending statement trees that are waiting for delay */
    (void)db_do_timestep( 0, TRUE );

#ifdef DEBUG_MODE
    if( debug_mode ) {
      unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "========  Writing database %s  ========\n", odb );
      assert( rv < USER_MSG_LENGTH );
      print_output( user_msg, DEBUG, __FILE__, __LINE__ );
    }
#endif

    /* Indicate that this CDD contains scored information */
    // info_suppl.part.scored = 1;

    /* Write contents to database file */
    // db_write( db, FALSE, FALSE );

    /* Write the coverage file */
    cov_db_write( odb );

  } Catch_anonymous {
    sim_dealloc();
    free_safe( odb, olen );
    free_safe( idb, ilen );
    Throw 0;
  }

  /* Deallocate simulator stuff */
  sim_dealloc();

  free_safe( odb, olen );
  free_safe( idb, ilen );

  PROFILE_END;

}

