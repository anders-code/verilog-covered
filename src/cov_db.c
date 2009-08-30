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
 \file     cov_db.c
 \author   Trevor Williams  (phase1geo@gmail.com)
 \date     8/5/2009
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "cov_db.h"
#include "defines.h"
#include "profiler.h"
#include "util.h"
#include "vector.h"


/*!
 Specifies the byte size of the coverage database read buffer.
*/
#define COV_DB_BUFSIZE    32768

/*!
 Reads the next character from the VCD file.
*/
#define cov_db_getch(cdb) ((cov_db_rdbuf_cur != cov_db_rdbuf_end) ? ((int)(*(cov_db_rdbuf_cur++))) : cov_db_getch_fetch( cdb ))

/*!
 Reads in a token that will be discarded.  Breaks the main while loop if an $end or an EOF is encountered.
*/
#define cov_db_get(cdb)   tok = cov_db_get_token1( cdb, 0 );  if( tok==T_EOF ) { Throw 0; }

/*!
 Returns a token in yytext starting at byte 0.
*/
#define cov_db_get_token(cdb) cov_db_get_token1( cdb, 0 )

/*!
 Adds a token to the existing yytext.
*/
#define cov_db_append_token(cdb, new_start) cov_db_get_token1( cdb, (new_start = (cov_db_yylen + 1)) )


/*!
 Main list containing the coverage databases associated with the loaded design database.
*/
cov_db** cov_db_list = NULL;

/*!
 Pointer to the current coverage database index.
*/
unsigned int curr_cov_db = 0;


/*!
 Pointer to start of coverage database read buffer.
*/
static char* cov_db_rdbuf_start = NULL;

/*!
 Pointer to end of VCD read buffer.
*/
static char* cov_db_rdbuf_end = NULL;

/*!
 Pointer to currently read character from read buffer.
*/
static char* cov_db_rdbuf_cur = NULL;

/*!
 Contains the string version of the next read token.
*/
static char* cov_db_yytext = NULL;

/*!
 Number of bytes allocated for the vcd_yytext array.
*/
static int cov_db_yytext_size = 0;

/*!
 Contains the length of the read token.
*/
static int cov_db_yylen = 0;


/*!
 \return Returns pointer to allocated/initialized coverage database.
*/
cov_db* cov_db_create() { PROFILE(COV_DB_CREATE);

  cov_db* cdb;

  /* Allocate and initialize the coverage database */
  cdb         = (cov_db*)malloc_safe( sizeof( cov_db ) );
  cdb->ul_num = 0;
  cdb->ul     = NULL;
  cdb->u8_num = 0;
  cdb->u8     = NULL;

  PROFILE_END;

  return( cdb );

}

/*!
 \return Returns the index of the specified element in the ul array.
*/
ulong cov_db_get_ul_index(
  ulong* addr  /*!< Address of the ul entry in the ul array */
) { PROFILE(COV_DB_GET_UL_INDEX);

  ulong index;

  if( cov_db_list == NULL ) {
    index = UL_SET;
  } else {
    index = (ulong)(addr - cov_db_list[0]->ul);
  }

  PROFILE_END;

  return( index );

} 

/*!
 Adds coverage data to the coverage database for the specified vector.
*/
static void cov_db_add_vector(
  const vector* vec  /*!< Pointer to vector to add coverage information for */
) { PROFILE(COV_DB_ADD_VECTOR);

  if( (vec->suppl.part.data_type == VDATA_UL) && (vec->suppl.part.type != VTYPE_VAL) ) {
    cov_db_list[0]->ul_num += UL_SIZE( vec->width ) * vector_type_sizes[vec->suppl.part.type];
  }

  PROFILE_END;

}

/*!
 Resizes the given vector (occurs when an expression is resized to match signal width.
*/
void cov_db_resize_vector(
  unsigned int  orig_width,  /*!< Original bit width of the vector */
  const vector* vec          /*!< Pointer to vector to add coverage information for */
) { PROFILE(COV_DB_RESIZE_VECTOR);

  if( (vec->suppl.part.data_type == VDATA_UL) && (vec->suppl.part.type != VTYPE_VAL) ) {
    cov_db_list[0]->ul_num -= UL_SIZE( orig_width ) * vector_type_sizes[vec->suppl.part.type];
    cov_db_list[0]->ul_num += UL_SIZE( vec->width ) * vector_type_sizes[vec->suppl.part.type];
  }

  PROFILE_END;

}

/*!
 Adds coverage data to the coverage database for the specified FSM.
*/
void cov_db_add_fsm(
  const fsm* table  /*!< Pointer to FSM structure to parse */
) { PROFILE(COV_DB_ADD_FSM);

  /* TBD */

  PROFILE_END;

}

/*!
 Adds coverage data to the coverage database for the specified expression.
*/
void cov_db_add_expr(
  const expression* expr  /*!< Pointer to expression to add coverage for */
) { PROFILE(COV_DB_ADD_EXPR);

  printf( "In cov_db_add_expr...\n" );

  /* Update the coverage and set the current expression's ID */
  cov_db_list[0]->u8_num++;

  if( EXPR_OWNS_VEC( expr->op ) ) {
    cov_db_add_vector( expr->value );
  }

  PROFILE_END;

}

/*!
 Adds coverage data to the coverage database for the specified signal.
*/
void cov_db_add_sig(
  const vsignal* sig  /*!< Pointer to signal to add coverage for */
) { PROFILE(COV_DB_ADD_SIG);

  cov_db_add_vector( sig->value );

  PROFILE_END;

}

/*!
 Writes coverage database file to the specified coverage file.
*/
void cov_db_write(
  const char* odb  /*!< Name of output coverage database file */
) { PROFILE(COV_DB_WRITE);

  FILE*        ofile;
  unsigned int i;

  if( (ofile = fopen( odb, "r" )) != NULL ) { 

    fprintf( ofile, "%u", cov_db_list[0]->ul_num );

    for( i=0; i<cov_db_list[0]->ul_num; i++ ) {
      fprintf( ofile, " %lx", cov_db_list[0]->ul[i] );
    }

    fprintf( ofile, "\n%u", cov_db_list[0]->u8_num );

    for( i=0; i<cov_db_list[0]->u8_num; i++ ) {
      fprintf( ofile, " %" FMT8 "x", cov_db_list[0]->u8[i] );
    }

    fprintf( ofile, "\n" );

  } else {

    unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Unable to write coverage database file \"%s\"", odb );
    assert( rv < USER_MSG_LENGTH );
    Throw 0;

  }

  PROFILE_END;

}

/*!
 Reads up to the next 32 Kb from the coverage file, adjusts the buffer pointers and returns the next character.
*/
static int cov_db_getch_fetch(
  FILE* cdb  /*!< Pointer to coverage database file to read */
) { PROFILE(COV_DB_GETCH_FETCH); 

  int    ch = -1;  /* Return value for this function */
  size_t rd;

  errno = 0;

  if( !feof( cdb ) ) {

    rd = fread( cov_db_rdbuf_start, sizeof( char ), COV_DB_BUFSIZE, cdb );
    cov_db_rdbuf_end = (cov_db_rdbuf_cur = cov_db_rdbuf_start) + rd;

    if( rd && (errno == 0) ) { 
      ch = (int)(*(cov_db_rdbuf_cur++));
    }

  }

  PROFILE_END;

  return( ch );

}

/*!
 \return Returns the next token read from the VCD buffer.

 Reads in the next token, storing the result in vcd_yytext and vcd_yylen.
*/
static int cov_db_get_token1(
  FILE* cdb,   /*!< Pointer to coverage database file to read */
  int   start  /*!< Index of yytext to start adding characters to */
) { PROFILE(COV_DB_GET_TOKEN1);

  int   token = T_UNKNOWN;  /* Return value for this function */
  int   ch;
  int   i;
  int   len   = start;
  char* yyshadow; 
  
  /* Read whitespace */
  for(;;) {
    ch = cov_db_getch( cdb );
    if( ch < 0 ) {
      token = T_EOF;
      break; 
    }
    if( ch > ' ' ) {
      break;
    }
  }

  if( token != T_EOF ) {

    for( cov_db_yytext[len++] = ch; ; cov_db_yytext[len++] = ch ) {
      if( len == cov_db_yytext_size ) {
        cov_db_yytext      = (char*)realloc_safe_nolimit( cov_db_yytext, cov_db_yytext_size, ((cov_db_yytext_size * 2) + 1) );
        cov_db_yytext_size = ((cov_db_yytext_size * 2) + 1);
      }
      ch = cov_db_getch( cdb );
      if( ch <= ' ' ) {
        break;
      }
    }
    cov_db_yytext[len] = '\0';
    cov_db_yylen       = len;

    token = T_STRING;

  }

  PROFILE_END;

  return( token );

}

/*!
 Reads in a coverage database from the given file and allocates and populates supplied coverage database structure.
*/
void cov_db_read(
  const char* fname  /*!< Filename of coverage database to read */
) { PROFILE(COV_DB_READ);

  FILE*        ifile;
  char         cdd_name[4096];
  unsigned int rv;

  /* Create relative pathname */
  rv = snprintf( cdd_name, 4096, "covered/cdb/%s", fname );
  assert( rv < 4096 );

  if( (ifile = fopen( cdd_name, "r" )) != NULL ) {

    unsigned int i;
    char*        token;
    cov_db*      cdb = (cov_db*)malloc_safe( sizeof( cov_db ) );

    /* Allocate memory for read buffer */
    cov_db_rdbuf_start = cov_db_rdbuf_end = cov_db_rdbuf_cur = (char*)malloc_safe( COV_DB_BUFSIZE );

    /* Allocate memory for vcd_yytext */
    cov_db_yytext = (char*)malloc_safe( (cov_db_yytext_size = 1024) );

    Try {

      /* Read in unsigned long values */
      assert( cov_db_get_token( ifile ) == T_STRING );
      cdb->ul_num = atoi( cov_db_yytext );
      cdb->ul     = (ulong*)malloc_safe( sizeof( ulong ) * cdb->ul_num );
      for( i=0; i<cdb->ul_num; i++ ) {
        assert( cov_db_get_token( ifile ) == T_STRING );
        cdb->ul[i] = strtoul( cov_db_yytext, NULL, 16 );
      }

      /* Read in uint8 values */
      assert( cov_db_get_token( ifile ) == T_STRING );
      cdb->u8_num = atoi( cov_db_yytext );
      cdb->u8     = (uint8*)malloc_safe( sizeof( uint8 ) * cdb->u8_num );
      for( i=0; i<cdb->u8_num; i++ ) {
        assert( cov_db_get_token( ifile ) == T_STRING );
        cdb->u8[i] = (uint8)strtoul( cov_db_yytext, NULL, 16 );
      }

    } Catch_anonymous {

      rv = snprintf( user_msg, USER_MSG_LENGTH, "Reading coverage database \"%s\" that is improperly formattted", cdd_name );
      assert( rv < USER_MSG_LENGTH );
      print_output( user_msg, FATAL, __FILE__, __LINE__ );

      free_safe( cov_db_rdbuf_start, COV_DB_BUFSIZE );
      free_safe( cov_db_yytext, cov_db_yytext_size );
      cov_db_dealloc( cdb );

      Throw 0;

    }

    /* Deallocate memory */
    free_safe( cov_db_rdbuf_start, COV_DB_BUFSIZE );
    free_safe( cov_db_yytext, cov_db_yytext_size );

    /* Allocate memory for the coverage database in coverage database list and add*/
    cov_db_list                = (cov_db**)realloc_safe( cov_db_list, (sizeof( cov_db ) * curr_cov_db), (sizeof( cov_db ) * (curr_cov_db + 1)) );
    cov_db_list[curr_cov_db++] = cdb;

  } else {

    rv = snprintf( user_msg, USER_MSG_LENGTH, "Unable to read coverage database file \"%s\"", fname );
    assert( rv < USER_MSG_LENGTH );
    print_output( user_msg, FATAL, __FILE__, __LINE__ );
    Throw 0;

  }

  PROFILE_END;

}

/*!
 Deallocates the given coverage database.
*/
void cov_db_dealloc(
  cov_db* cdb  /*!< Pointer to coverage database to deallocate */
) { PROFILE(COV_DB_DEALLOC);

  if( cdb != NULL ) {

    /* Deallocate unsigned long array */
    free_safe( cdb->ul, (sizeof( ulong ) * cdb->ul_num) );

    /* Deallocate unisgned int8 array */
    free_safe( cdb->u8, (sizeof( uint8 ) * cdb->u8_num) );

    /* Deallocate coverage database */
    free_safe( cdb, sizeof( cov_db ) );

  }

  PROFILE_END;

}


